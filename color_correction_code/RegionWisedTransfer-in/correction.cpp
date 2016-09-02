#include  "correction.h"
#include  "function.h"

void CCorrecter::initial(const string folder, const string srcsFn, const string refsFn, const string srcrFn, const string refrFn, const RWTransfer& rwtransfer, const vector<vector<bool>>& ralist)
{
	//先读取原图像
	srcIm = rwtransfer.srcIm.clone();
	refIm = rwtransfer.refIm.clone();

	//需要图像一定是RGB格式,转Lab
	/*cv::cvtColor(srcIm, srcLabIm, CV_BGR2Lab);
	cv::cvtColor(refIm, refLabIm, CV_BGR2Lab);*/

	srcRgbIm = srcIm.clone();
	refRgbIm = refIm.clone();

	int rows = rwtransfer.srcIm.size().height;
	int cols = rwtransfer.refIm.size().width;

	string srcsImFn = folder + "/" + srcsFn + "_lrgb.txt";
	string refsImFn = folder + "/" + refsFn + "_lrgb.txt";
	readMatfromfile(srcsImFn, srcsIm, rows, cols, 1);
	readMatfromfile(refsImFn, refsIm, rows, cols, 1);
	string srcrImFn = folder + "/" + srcrFn + "_lrgb.txt";
	string refrImFn = folder + "/" + refrFn + "_lrgb.txt";
	readMatfromfile(srcrImFn, srcrIm, rows, cols, 3);
	readMatfromfile(refrImFn, refrIm, rows, cols, 3);
	cout << "read files done"<<endl;




	/*reflect_weight_corr = srcrIm;
	reflect_sim_corr = srcrIm;*/
	shading_sim_corr = srcsIm;
	shading_weight_corr = srcsIm;

	/*for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			srcsIm.at<float>(i, j) = srcsIm.at<float>(i, j)*125;
			refsIm.at<float>(i, j) = refsIm.at<float>(i, j)*125;
			for(int k = 0; k < 3; ++k) {
				srcrIm.at<Vec3f>(i, j)[k] = srcrIm.at<Vec3f>(i, j)[k]*255;
				refrIm.at<Vec3f>(i, j)[k] = refrIm.at<Vec3f>(i, j)[k]*255;
			}
		}
	}*/
	//FileStorage fs(folder + "/srcrIm.xml", FileStorage::WRITE);
	//fs << "srcrIm" << srcrIm;
	//fs.release();

	//fs.open(folder + "/refrIm.xml", FileStorage::WRITE);
	//fs << "refrIm" << refrIm;
	//fs.release();

	//fs.open(folder + "/srcsIm.xml", FileStorage::WRITE);
	//fs << "srcsIm" << srcsIm;
	//fs.release();

	//fs.open(folder + "/refsIm.xml", FileStorage::WRITE);
	//fs << "refsIm" << refsIm;
	//fs.release();

	vector<vector<int>> lmap = rwtransfer.src2refmap;
	regionum = rwtransfer.srcregionum;
	labelmap.resize(regionum);

	for(int i = 0; i < regionum; ++i)
	{
		if(lmap[i].size() != 0)
		{
		    labelmap[i] = lmap[i][0];
		}
		else
		{
			labelmap[i] = -1;
		}
	}

	//label和pixels不能直接get

	srcPixels.resize(regionum);
	refPixels.resize(regionum);
	for(int i = 0; i < regionum; ++i)
	{
		int j = labelmap[i];//src:i ref:j

		if(j != -1)
		{
		    refPixels[i].resize(rwtransfer.refPixels[j].size());
		    srcPixels[i].resize(rwtransfer.srcPixels[i].size());
		    refPixels[i] = rwtransfer.refPixels[j];
		    srcPixels[i] = rwtransfer.srcPixels[i];
		}
		else
		{
			refPixels[i].resize(0);
		}
	}

	//pixels->labels
	pixels2labels(srcPixels, srcLabels);
	pixels2labels(refPixels, refLabels);

	//regions centers;
	srcCenters.resize(regionum);
	srcCenters = rwtransfer.srcCenters;
	//copy(rwtransfer.srcCenters.begin(), rwtransfer.srcCenters.end(), back_inserter(srcCenters));

	//ralist : 区域邻接表
	RAList.resize(regionum);
	for(int i = 0; i < regionum; ++i)
	{
		RAList[i] = ralist[i];
	}
}

void CCorrecter::readMatfromfile(string ImFn, Mat& RAWIm, int rows, int cols, int channels)
{
	FILE *file = fopen(ImFn.c_str(), "r");
	if(channels == 3) {
		Mat Im(rows, cols, CV_32FC3);
		RAWIm = Im.clone();
		for(int i = 0; i < rows; ++i) {
			for(int j = 0; j < cols; ++j) {
				fscanf( file, "%f %f %f", &(RAWIm.at<Vec3f>(i, j)[0]), &(RAWIm.at<Vec3f>(i, j)[1]), &(RAWIm.at<Vec3f>(i, j)[2]) );
			}
	    }
	}
	else if(channels == 1) {
		Mat Im(rows, cols, CV_32FC1);
		RAWIm = Im.clone();
		for(int i = 0; i < rows; ++i) {
			for(int j = 0; j < cols; ++j) {
				fscanf( file, "%f", &(RAWIm.at<float>(i, j)));
			}
		}
	}
}

void CCorrecter::pixels2labels(const vector<vector<Point2f>>& pixels, vector<int>& labels)
{
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	labels.clear();
	labels.resize(height * width, -1);

	int rnum = pixels.size();

	for(int i = 0; i < rnum; ++i)
	{
		for(int j = 0; j < pixels[i].size(); ++j)
		{
			Point2f p = pixels[i][j];

			int index = static_cast<int>(floor(p.y)) * width + static_cast<int>(floor(p.x));
			labels[index] = i;
		}
	}
}

void CCorrecter::calGlobalParameters(Mat srcIm, Mat refIm, string mode)
{
	//Global 
	meanStdDev(srcIm, srcMeanG, srcStddevG);//自动求出全局的均值和方差
	meanStdDev(refIm, refMeanG, refStddevG);
	//求比值
	if(mode == "r" || mode == "i" || mode == "l") {
		rsFactorG = Scalar(refStddevG.val[0]/srcStddevG.val[0], refStddevG.val[1]/srcStddevG.val[1], refStddevG.val[2]/srcStddevG.val[2]) ;
	}
	else if(mode == "s") {
		rsFactorG = Scalar(refStddevG.val[0]/srcStddevG.val[0]);
	}	
	/*cout << "Global: " << endl;
	cout << "srcMeanG" << srcMeanG <<endl;
	cout << "refMeanG" << refMeanG <<endl;
	cout << "srcStddevG" << srcStddevG <<endl;
	cout << "refStddevG" << refStddevG <<endl;
	cout << "rsFactorG" << rsFactorG << endl;*/
}

void CCorrecter::calLocalParameters(Mat srcIm, Mat refIm, string mode)
{
	//计算每个区域的均值方差

	//对每一个region都要算一个参数
	srcMeans.resize(regionum);
	refMeans.resize(regionum);
	srcStddevs.resize(regionum);
	refStddevs.resize(regionum);
	rsFactors.resize(regionum);

	//计算srcMeans, refMeans, rfFactors
	for(int i = 0; i < regionum; ++i)
	{
		if(srcPixels[i].size() == 0 || refPixels[i].size() == 0) {
			srcMeans[i] = srcMeanG;
			refMeans[i] = refMeanG;
			srcStddevs[i] = srcStddevG;
			refStddevs[i] = refStddevG;
			rsFactors[i] = rsFactorG;
			continue;
		}
		vector<Point2f>& srcPts = srcPixels[i];
		vector<Point2f>& refPts = refPixels[i];

		Mat srcMsk = Mat::zeros(srcIm.size(), CV_8UC1);
		calMask(srcPts, srcMsk);
		Mat refMsk = Mat::zeros(refIm.size(), CV_8UC1);
		calMask(refPts, refMsk);

		meanStdDev(srcIm, srcMeans[i], srcStddevs[i], srcMsk);
		meanStdDev(refIm, refMeans[i], refStddevs[i], refMsk);

		if(mode == "r" || mode == "i" || mode == "l") {
			rsFactors[i] = Scalar(refStddevs[i].val[0]/srcStddevs[i].val[0], refStddevs[i].val[1]/srcStddevs[i].val[1], refStddevs[i].val[2]/srcStddevs[i].val[2]) ;
		}
		else if(mode == "s") {
			rsFactors[i] = Scalar(refStddevs[i].val[0]/srcStddevs[i].val[0]);
		}
	}
	cout << "Local parameters done. " << endl;
}

void CCorrecter::correct(const Mat& msk, const Scalar& srcMean, const Scalar& refMean, const Scalar& rsFactor, Mat& sIm, Mat srcIm, string mode)
{
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	double Rfactor, Gfactor, Bfactor, Sfactor;
	if(mode == "r" || mode == "i" || mode == "l") {
		Rfactor = rsFactor.val[0];
		Gfactor = rsFactor.val[1];
		Bfactor = rsFactor.val[2];
	}
	else if(mode == "s") {
		Sfactor = rsFactor.val[0];
	}

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if(msk.at<uchar>(y,x) == 0)
				continue;

			double transferR, transferG, transferB, transferS;
			if(mode == "r") {
				transferR = refMean.val[0] + Rfactor * (srcIm.at<Vec3f>(y,x)[0] - srcMean.val[0]);
				transferG = refMean.val[1] + Gfactor * (srcIm.at<Vec3f>(y,x)[1] - srcMean.val[1]);
				transferB = refMean.val[2] + Bfactor * (srcIm.at<Vec3f>(y,x)[2] - srcMean.val[2]);

				sIm.at<Vec3f>(y,x) = Vec3f(transferR, transferG, transferB);
			}
			else if(mode == "s") {
				transferS = refMean.val[0] + Sfactor * (srcIm.at<float>(y, x) - srcMean.val[0]);
				sIm.at<float>(y, x) = (float)transferS;
			}
			else if(mode == "i" || mode == "l") {
				transferR = refMean.val[0] + Rfactor * (srcIm.at<Vec3b>(y,x)[0] - srcMean.val[0]);
				transferG = refMean.val[1] + Gfactor * (srcIm.at<Vec3b>(y,x)[1] - srcMean.val[1]);
				transferB = refMean.val[2] + Bfactor * (srcIm.at<Vec3b>(y,x)[2] - srcMean.val[2]);

				transferR = min(max(0.0, transferR), 255.0);
				transferG = min(max(0.0, transferG), 255.0);
				transferB = min(max(0.0, transferB), 255.0);

				sIm.at<Vec3b>(y,x) = Vec3b(transferR, transferG, transferB);
			}
		}
	}
}

//简单的对应区域的颜色校正~ corretedSrcIm
void CCorrecter::simpleCorrection(Mat srcIm, string mode)
{
	Mat tmpIm = Mat::zeros(srcIm.size(), srcIm.type());
	Mat tmpSrcMsk = Mat(srcIm.size(), CV_8UC1, Scalar(255));;
	
	//global 计算一次
	//把global计算的这次取消
	//correct(tmpSrcMsk, srcMeanG, refMeanG, rsFactorG, tmpIm, srcIm, mode);

	for(int i = 0; i < regionum; ++i)
	{
		if(srcPixels[i].size() == 0) {
			continue;
		}
		vector<Point2f>& srcPts = srcPixels[i];
		tmpSrcMsk = Mat::zeros(srcIm.size(), CV_8UC1);
		calMask(srcPts, tmpSrcMsk);
		correct(tmpSrcMsk, srcMeans[i], refMeans[i], rsFactors[i], tmpIm, srcIm, mode);
	}
	if(mode == "r") {
	    reflect_sim_corr = tmpIm.clone();
		cout << "simple reflectance correction done" <<endl;
	}
	else if(mode == "s") {
		shading_sim_corr = tmpIm.clone();
		cout << "simple shading correction done" <<endl;
	}
	else if(mode == "i") {
		init_sim_corr = tmpIm.clone();
		/*cv::cvtColor(tmpIm, init_sim_corr, CV_Lab2BGR);*/
		cout << "simple init image correction done" <<endl;
	}
	else if(mode == "l") {
		last_in_sim_corr = tmpIm.clone();
		cout << "simple last image correction done" <<endl;
	}
}

//加权的对应区域的颜色校正~ wcorrectedSrcIm
void CCorrecter::weightedCorrection(Mat srcIm, string mode)
{
	Mat tmpIm = Mat::zeros(srcIm.size(), srcIm.type());
	Mat tmpSrcMsk = Mat(srcIm.size(), CV_8UC1, Scalar(255));;

	int width = srcIm.size().width;
	int height = srcIm.size().height;

	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			if(mode == "r") {
				double alpha = 0.2;
				//alpha = 0.105
				Vec3f rawcolor = srcIm.at<Vec3f>(y,x);				
				double sumL = 0.0, sumA = 0.0, sumB = 0.0;
				double weightSumL = 0.0, weightSumA = 0.0, weightSumB = 0.0;					
				int idx = srcLabels[y*width+x];
			
				for(int i = 0; i < regionum; ++i)
				{
					if(srcPixels[i].size() == 0)
						continue;

					double dist ;
					double weightD = 1;

					Scalar srcMean = srcMeans[i];
					Scalar refMean = refMeans[i];
					Scalar rsFactor = rsFactors[i];

					double deltaL = rawcolor[0] - srcMean.val[0];
					double deltaA = rawcolor[1] - srcMean.val[1];
					double deltaB = rawcolor[2] - srcMean.val[2];

					double sigmaL = alpha;
					double sigmaA = alpha;
					double sigmaB = alpha;

					double weightL = exp(-deltaL*deltaL / (2*sigmaL*sigmaL) ) / (sqrt(2*_PI)*sigmaL);
					double weightA = exp(-deltaA*deltaA / (2*sigmaA*sigmaA) ) / (sqrt(2*_PI)*sigmaA);
					double weightB = exp(-deltaB*deltaB / (2*sigmaB*sigmaB) ) / (sqrt(2*_PI)*sigmaB);

					weightSumL += weightL * weightD;
					weightSumA += weightA * weightD;
					weightSumB += weightB * weightD;

					sumL += ( weightL * (rsFactor.val[0] * deltaL + refMean.val[0] ) );
					sumA += ( weightA * (rsFactor.val[1] * deltaA + refMean.val[1] ) );
					sumB += ( weightB * (rsFactor.val[2] * deltaB + refMean.val[2] ) );
				}
				if (weightSumL != 0.0 && weightSumA != 0.0 && weightSumB != 0.0)
				{
					tmpIm.at<Vec3f>(y,x)[0] = sumL / weightSumL;
					tmpIm.at<Vec3f>(y,x)[1] = sumA / weightSumA;
					tmpIm.at<Vec3f>(y,x)[2] = sumB / weightSumB;			
				}
			}
			else if(mode == "i" || mode == "l") {
				double alpha;
				if(mode == "i") {
					alpha = 15;
				}
				if(mode == "l") {
					alpha = 14.9;
				}
				Vec3b rawcolor = srcIm.at<Vec3b>(y,x);
				
				double sumL = 0.0, sumA = 0.0, sumB = 0.0;
				double weightSumL = 0.0, weightSumA = 0.0, weightSumB = 0.0;
					
				int idx = srcLabels[y*width+x];
			
				for(int i = 0; i < regionum; ++i)
				{
					if(srcPixels[i].size() == 0)
						continue;

					double dist ;
					double weightD = 1;

					Scalar srcMean = srcMeans[i];
					Scalar refMean = refMeans[i];
					Scalar rsFactor = rsFactors[i];

					double deltaL = rawcolor[0] - srcMean.val[0];
					double deltaA = rawcolor[1] - srcMean.val[1];
					double deltaB = rawcolor[2] - srcMean.val[2];

					double sigmaL = alpha;
					double sigmaA = alpha;
					double sigmaB = alpha;

					double weightL = exp(-deltaL*deltaL / (2*sigmaL*sigmaL) ) / (sqrt(2*_PI)*sigmaL);
					double weightA = exp(-deltaA*deltaA / (2*sigmaA*sigmaA) ) / (sqrt(2*_PI)*sigmaA);
					double weightB = exp(-deltaB*deltaB / (2*sigmaB*sigmaB) ) / (sqrt(2*_PI)*sigmaB);

					weightSumL += weightL * weightD;
					weightSumA += weightA * weightD;
					weightSumB += weightB * weightD;

					sumL += ( weightL * (rsFactor.val[0] * deltaL + refMean.val[0] ) );
					sumA += ( weightA * (rsFactor.val[1] * deltaA + refMean.val[1] ) );
					sumB += ( weightB * (rsFactor.val[2] * deltaB + refMean.val[2] ) );
				}

				if (weightSumL != 0.0 && weightSumA != 0.0 && weightSumB != 0.0)
				{
					tmpIm.at<Vec3b>(y,x)[0] = static_cast<uchar>(max(min(255.0, sumL / weightSumL), 0.0));
					tmpIm.at<Vec3b>(y,x)[1] = static_cast<uchar>(max(min(255.0, sumA / weightSumA), 0.0));
					tmpIm.at<Vec3b>(y,x)[2] = static_cast<uchar>(max(min(255.0, sumB / weightSumB), 0.0));			
				}
			}
			else if(mode == "s") {
				double alpha = 0.246;
				//alpha = 1.2;
				float rawcolor = srcIm.at<float>(y,x);				
				double sumL = 0.0;
				double weightSumL = 0.0;
			
				for(int i = 0; i < regionum; ++i)
				{
					if(srcPixels[i].size() == 0)
						continue;

					double weightD = 1;

					Scalar srcMean = srcMeans[i];
					Scalar refMean = refMeans[i];
					Scalar rsFactor = rsFactors[i];

					double deltaL = rawcolor - srcMean.val[0];

					double sigmaL = alpha;

					double weightL = exp(-deltaL*deltaL / (2*sigmaL*sigmaL) ) / (sqrt(2*_PI)*sigmaL);

					weightSumL += weightL * weightD;

					sumL += ( weightL * (rsFactor.val[0] * deltaL + refMean.val[0] ) );
				}
				if (weightSumL != 0.0)
				{
					tmpIm.at<float>(y,x) = sumL / weightSumL;	
				}
			}
		}
	}
	if(mode == "r") {
	    reflect_weight_corr = tmpIm.clone();
		cout << "reflectance weighted correction done" <<endl;
	}
	else if(mode == "s") {
		shading_weight_corr = tmpIm.clone();
		cout << "shading weighted correction done" <<endl;
	}	
	else if(mode == "i") {
		init_weight_corr = tmpIm.clone();
		//cv::cvtColor(tmpIm, init_weight_corr, CV_Lab2BGR);
		cout << "init weighted correction done" <<endl;
	}
	else if(mode == "l") {
		last_in_weight_corr = tmpIm.clone();
		cout << "last weighted correction done" <<endl;
	}
}

double CCorrecter::calMinDist(const Point2f pt, const vector<Point2f>& pixels)
{
	int size = pixels.size();
	double min = 100000.0;
	double dist;
	for(int i = 0; i < size; ++i)
	{
		Point2f deltaPt = pixels[i] - pt;
		dist = sqrt(deltaPt.x * deltaPt.x + deltaPt.y * deltaPt.y);
		if(dist < min)
			min = dist;
	}
	return min;
}

void CCorrecter::saveWeightedMap(const string& sfn, Mat srcIm) 
{
	string folder = sfn + "_w_maps";
	_mkdir(folder.c_str());

	//为每个区域计算weight map
	for(int i = 0; i < regionum; ++i)
	{
		Mat wmapL = Mat::zeros(srcIm.size(), CV_64FC1);
		Mat wmapA = Mat::zeros(srcIm.size(), CV_64FC1);
		Mat wmapB = Mat::zeros(srcIm.size(), CV_64FC1);

		Scalar mean = srcMeans[i];
		Scalar stddev = srcStddevs[i];

		for(int j = 0; j < regionum; ++j)
		{
			if(RAList[i][j] == false)
				continue;

			//某邻接区域内所有的像素点
			for(int k = 0; k < srcPixels[j].size(); ++k)
			{
				Point2i pt = srcPixels[j][k];

				double deltaL = srcIm.at<Vec3b>(pt.y, pt.x)[0] - mean.val[0];
				double deltaA = srcIm.at<Vec3b>(pt.y, pt.x)[1] - mean.val[1];
				double deltaB = srcIm.at<Vec3b>(pt.y, pt.x)[2] - mean.val[2];

				double sigmaL = stddev.val[0]; 
				double sigmaA = stddev.val[1];
				double sigmaB = stddev.val[2];

				wmapL.at<double>(pt.y, pt.x) =  exp( -(deltaL * deltaL)/(2*sigmaL*sigmaL ) ) / sqrt(2*_PI)*sigmaL;
				wmapA.at<double>(pt.y, pt.x) =  exp( -(deltaA * deltaA)/(2*sigmaA*sigmaA ) ) / sqrt(2*_PI)*sigmaA;
				wmapB.at<double>(pt.y, pt.x) =  exp( -(deltaB * deltaB)/(2*sigmaB*sigmaB ) ) / sqrt(2*_PI)*sigmaB;				
			}

			Mat tmp, wmapLU, wmapAU, wmapBU;
			normalize(wmapL,tmp,1.0,0.0,NORM_MINMAX);
			tmp.convertTo(wmapLU, CV_8UC1, 255);
			normalize(wmapA,tmp,1.0,0.0,NORM_MINMAX);
			tmp.convertTo(wmapAU, CV_8UC1, 255);
			normalize(wmapB,tmp,1.0,0.0,NORM_MINMAX);
			tmp.convertTo(wmapBU, CV_8UC1, 255);

			/*string savefn = folder + "/w_L_" + type2string<int>(i) + ".png";
			imwrite(savefn, wmapLU);
			savefn = folder + "/w_A_" + type2string<int>(i) + ".png";
			imwrite(savefn, wmapAU);
			savefn = folder + "/w_B_" + type2string<int>(i) + ".png";
			imwrite(savefn, wmapBU);*/
		}
	}
	cout << "weight maps done." << endl;
}

void CCorrecter::showCorrectedResult()
{
	imshow("simple correction", final_correctedSrcIm);
	waitKey(0);
	destroyWindow("simple correction");

	imshow("weighted correction", final_wcorrectedSrcIm);
	waitKey(0);
	destroyWindow("weighted correction");
}

void CCorrecter::Correction(Mat srcIm, Mat refIm, string sceneFn, string folder, string srcFn, string mode)
{
	calGlobalParameters(srcIm, refIm, mode);//计算全局参数

	calLocalParameters(srcIm, refIm, mode);//计算局部参数
		
	simpleCorrection(srcIm, mode);

	weightedCorrection(srcIm, mode);
}

void CCorrecter::Showlayers(Mat layer, string mode, string folder, string resultFn) {
	int rows = layer.size().height;
	int cols = layer.size().width;

	Mat rlayertmp(rows, cols, CV_32FC3);
	Mat slayertmp(rows, cols, CV_32FC1);
	Mat rtmp(rows, cols, CV_8UC3);
	Mat stmp(rows, cols, CV_8UC1);

	if(mode == "r") {
		vector<float> maxv(3, 0.0);
		for(int y = 0; y < rows; ++y) {
			for(int x = 0; x < cols; ++x) {
				for(int k = 0; k < 3; ++k) {
					maxv[k] = (layer.at<Vec3f>(y, x)[k] > maxv[k]) ? layer.at<Vec3f>(y, x)[k] : maxv[k];
				}
			}
		}
		float maxvk = max(max(maxv[0], maxv[1]), maxv[2]);
		for(int y = 0; y < rows; ++y) {
			for(int x = 0; x < cols; ++x) {
				for(int k = 0; k < 3; ++k) {
					rlayertmp.at<Vec3f>(y, x)[k] = (layer.at<Vec3f>(y, x)[k]*1.0) / (maxvk*1.0);
					rlayertmp.at<Vec3f>(y, x)[k] = (rlayertmp.at<Vec3f>(y, x)[k] > 0.0031308) ? (1.055 * pow(rlayertmp.at<Vec3f>(y, x)[k], 1.0 / 2.4) - 0.055) : (rlayertmp.at<Vec3f>(y, x)[k] * 12.92);
				}
				rtmp.at<Vec3b>(y, x)[0] = static_cast<uchar>(max(0.0, min(rlayertmp.at<Vec3f>(y, x)[2]*255.0, 255.0)));
				rtmp.at<Vec3b>(y, x)[1] = static_cast<uchar>(max(0.0, min(rlayertmp.at<Vec3f>(y, x)[1]*255.0, 255.0)));
				rtmp.at<Vec3b>(y, x)[2] = static_cast<uchar>(max(0.0, min(rlayertmp.at<Vec3f>(y, x)[0]*255.0, 255.0)));
			}
		}
	}
	else if(mode == "s") {
		vector<float> maxv(1, 0.0);
		for(int y = 0; y < rows; ++y) {
			for(int x = 0; x < cols; ++x) {
				maxv[0] = (layer.at<float>(y, x) > maxv[0]) ? layer.at<float>(y, x) : maxv[0];
			}
		}
		for(int y = 0; y < rows; ++y) {
			for(int x = 0; x < cols; ++x) {
				slayertmp.at<float>(y, x) = (layer.at<float>(y, x)*1.0) / ((maxv[0]*1.0));
				slayertmp.at<float>(y, x) = (slayertmp.at<float>(y, x) > 0.0031308) ? (1.055 * pow( slayertmp.at<float>(y, x), 1.0 / 2.4) - 0.055) : (slayertmp.at<float>(y, x) * 12.92);
				stmp.at<uchar>(y, x) = static_cast<uchar>(max(0.0, min(slayertmp.at<float>(y, x)*255.0, 255.0)));
			}
		}
	}
	
	string fileFn = folder + "/" + mode + resultFn + "_layer" + ".png";
	if(mode == "s") {
		imwrite(fileFn, stmp);
	}
	else if(mode == "r") {
		imwrite(fileFn, rtmp);
	}
}
void CCorrecter::Combine(Mat reflectance, Mat shading, string resultFn, string folder) {
	Showlayers(reflectance, "r", folder, resultFn);
	Showlayers(shading, "s", folder, resultFn);

	int rows = shading.size().height;
	int cols = shading.size().width;
	Mat srcresult(rows, cols, CV_8UC3);
	for(int y = 0; y < rows; ++y) {
		for(int x = 0; x < cols; ++x) {
			double val[3];
			val[0] = reflectance.at<Vec3f>(y, x)[0] * shading.at<float>(y, x);
			val[1] = reflectance.at<Vec3f>(y, x)[1] * shading.at<float>(y, x);
			val[2] = reflectance.at<Vec3f>(y, x)[2] * shading.at<float>(y, x);
			for(int k = 0; k < 3; ++k) {
				val[k] = (val[k] > 0.0031308) ? (1.055 * pow(val[k], 1.0 / 2.4) - 0.055) : (val[k] * 12.92);
				/*val[k] = (val[k] > 0.0031308) ? (1.055 * pow(val[k], 1.0 / 2.4) - 0.055) : (val[k] * 12.92);*/
			}
			srcresult.at<Vec3b>(y, x)[0] = static_cast<uchar>(max(0.0, min(val[2] * 255.0, 255.0)));
			srcresult.at<Vec3b>(y, x)[1] = static_cast<uchar>(max(0.0, min(val[1] * 255.0, 255.0)));
			srcresult.at<Vec3b>(y, x)[2] = static_cast<uchar>(max(0.0, min(val[0] * 255.0, 255.0)));
		}
	}
	string fileFn = folder + "/" + resultFn + ".png";
	cout << "save " + fileFn + " done"<<endl;
	imwrite(fileFn, srcresult);
	srcinresult = srcresult.clone();
}