#include  "correction.h"
#include  "function.h"

void CCorrecter::initial(const string folder, const RWTransfer& rwtransfer, const vector<vector<bool>>& ralist)
{
	//先读取原图像
	srcIm = rwtransfer.srcIm.clone();
	refIm = rwtransfer.refIm.clone();

	//需要图像一定是RGB格式,转Lab
	cv::cvtColor(srcIm, srcLabIm, CV_BGR2Lab);
	cv::cvtColor(refIm, refLabIm, CV_BGR2Lab);
	/*srcRgbIm = srcIm.clone();
	refRgbIm = refIm.clone();*/

	int rows = rwtransfer.srcIm.size().height;
	int cols = rwtransfer.refIm.size().width;

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
	cout << "initial data done" <<endl;
}

void CCorrecter::readMatfromfile(string ImFn, Mat& RAWIm, int rows, int cols, int channels)
{
	fstream fin(ImFn.c_str());
	if(channels == 3) {
		Mat Im(rows, cols, CV_32FC3);
		RAWIm = Im.clone();
		for(int i = 0; i < rows; ++i) {
			for(int j = 0; j < cols; ++j) {
				float x1, x2, x3;
				fin >> x1 >> x2 >> x3;
				RAWIm.at<Vec3f>(i, j)[0] = x1;
				RAWIm.at<Vec3f>(i, j)[1] = x2;
				RAWIm.at<Vec3f>(i, j)[2] = x3;
			}
	    }
	}
	else if(channels == 1) {
		Mat Im(rows, cols, CV_32FC1);
		RAWIm = Im.clone();
		for(int i = 0; i < rows; ++i) {
			for(int j = 0; j < cols; ++j) {
				float x1;
				fin >> x1;
				RAWIm.at<float>(i, j) = x1;
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

void CCorrecter::calGlobalParameters(Mat srcIm, Mat refIm)
{
	//Global 
	meanStdDev(srcIm, srcMeanG, srcStddevG);//自动求出全局的均值和方差
	meanStdDev(refIm, refMeanG, refStddevG);
	//求比值
	rsFactorG = Scalar(refStddevG.val[0]/srcStddevG.val[0], refStddevG.val[1]/srcStddevG.val[1], refStddevG.val[2]/srcStddevG.val[2]) ;
	cout << "Global parameters done" <<endl;
}

void CCorrecter::calLocalParameters(Mat srcIm, Mat refIm)
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
		if(srcPixels.size() == 0 || refPixels.size() == 0) {
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

		rsFactors[i] = Scalar(refStddevs[i].val[0]/srcStddevs[i].val[0], refStddevs[i].val[1]/srcStddevs[i].val[1], refStddevs[i].val[2]/srcStddevs[i].val[2]) ;
	}
	cout << "Local parameters done. " << endl;
}

void CCorrecter::correct(const Mat& msk, const Scalar& srcMean, const Scalar& refMean, const Scalar& rsFactor, Mat& sIm, Mat srcIm)
{
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	double Rfactor, Gfactor, Bfactor;
	Rfactor = rsFactor.val[0];
	Gfactor = rsFactor.val[1];
	Bfactor = rsFactor.val[2];

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if(msk.at<uchar>(y,x) == 0)
				continue;

			double transferR, transferG, transferB;
			transferR = refMean.val[0] + Rfactor * (srcIm.at<Vec3b>(y,x)[0] - srcMean.val[0]);
			transferG = refMean.val[1] + Gfactor * (srcIm.at<Vec3b>(y,x)[1] - srcMean.val[1]);
			transferB = refMean.val[2] + Bfactor * (srcIm.at<Vec3b>(y,x)[2] - srcMean.val[2]);

			transferR = min(255.0, max(0.0, transferR));
			transferG = min(255.0, max(0.0, transferG));
			transferB = min(255.0, max(0.0, transferB));

			sIm.at<Vec3b>(y,x) = Vec3b(transferR, transferG, transferB);		
		}
	}
}

//简单的对应区域的颜色校正~ corretedSrcIm
void CCorrecter::simpleCorrection(Mat srcIm)
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
		correct(tmpSrcMsk, srcMeans[i], refMeans[i], rsFactors[i], tmpIm, srcIm);
	}
	cv::cvtColor(tmpIm, init_sim_corr, CV_Lab2BGR);
	//init_sim_corr = tmpIm.clone();

}

vector<double> CCorrecter::getIMmap(int y, int x, Mat srcIm) {
	IMmap.resize(regionum, 0.0);
	for(int i = 0; i < regionum; ++i) {
		if(srcPixels[i].size() == 0) {
			continue;
		}
		IMmap[i].val[0] = ((srcIm.at<Vec3b>(y, x)[0] - srcMeans[i].val[0]) / 255.0) * ((srcIm.at<Vec3b>(y, x)[0] - srcMeans[i].val[0]) / 255.0);
		IMmap[i].val[1]	= ((srcIm.at<Vec3b>(y, x)[1] - srcMeans[i].val[1]) / 255.0) * ((srcIm.at<Vec3b>(y, x)[1] - srcMeans[i].val[1]) / 255.0);
		IMmap[i].val[2]	= ((srcIm.at<Vec3b>(y, x)[2] - srcMeans[i].val[2]) / 255.0) * ((srcIm.at<Vec3b>(y, x)[2] - srcMeans[i].val[2]) / 255.0);
	}
	vector<double> sumIM(3, 0.0);
	vector<double> tempsum(3, 0.0);
	for(int i = 0; i < regionum; ++i) {
		if(srcPixels[i].size() == 0) {
			continue;
		}
		for(int k = 0; k < 3; ++k) {
			IMmap[i].val[k] = exp(-14.0 * IMmap[i].val[k]);

			sumIM[k] += IMmap[i].val[k];
	    }
		
	}
	for(int i = 0; i < regionum; ++i) {
		if(srcPixels[i].size() == 0) {
			continue;
		}
		for(int k = 0; k < 3; ++k) {
			IMmap[i].val[k] = IMmap[i].val[k] / sumIM[k];
			tempsum[k] += IMmap[i].val[k];
		}
	}
	return tempsum;
}
//加权的对应区域的颜色校正~ wcorrectedSrcIm
//void CCorrecter::weightedCorrection(Mat srcIm)
//{
//	Mat tmpIm = Mat::zeros(srcIm.size(), srcIm.type());
//	Mat tmpSrcMsk = Mat(srcIm.size(), CV_8UC1, Scalar(255));;
//
//	int width = srcIm.size().width;
//	int height = srcIm.size().height;
//
//	for(int y = 0; y < height; ++y)
//	{
//		for(int x = 0; x < width; ++x)
//		{
//			vector<double> sumIM = getIMmap(y, x, srcIm);		
//			Vec3b rawcolor = srcIm.at<Vec3b>(y,x);
//			double sumR = 0.0, sumG = 0.0, sumB = 0.0;
//			
//			for(int i = 0; i < regionum; ++i)
//			{
//				if(srcPixels[i].size() == 0) {
//					continue;
//				}
//				Scalar srcMean = srcMeans[i];
//				Scalar refMean = refMeans[i];
//				Scalar rsFactor = rsFactors[i];
//
//				double deltaR = rawcolor[0] - srcMean.val[0];
//				double deltaG = rawcolor[1] - srcMean.val[1];
//				double deltaB = rawcolor[2] - srcMean.val[2];
//
//				sumR += ( IMmap[i].val[0] * (rsFactor.val[0] * deltaR + refMean.val[0] ) );
//				sumG += ( IMmap[i].val[1] * (rsFactor.val[1] * deltaG + refMean.val[1] ) );
//				sumB += ( IMmap[i].val[2] * (rsFactor.val[2] * deltaB + refMean.val[2] ) );
//			}
//			if (sumIM[0] != 0.0 && sumIM[1] != 0.0 && sumIM[2] != 0.0)
//			{
//				tmpIm.at<Vec3b>(y,x)[0] = static_cast<uchar>(min(255.0, max(0.0, sumR)));
//				tmpIm.at<Vec3b>(y,x)[1] = static_cast<uchar>(min(255.0, max(0.0, sumG)));
//				tmpIm.at<Vec3b>(y,x)[2] = static_cast<uchar>(min(255.0, max(0.0, sumB)));	
//			}
//		}
//	}
//
//	//cv::cvtColor(tmpIm, init_weight_corr, CV_Lab2BGR);
//	init_weight_corr = tmpIm.clone();
//	cout << "init weighted correction done" <<endl;
//}

void CCorrecter::weightedCorrection(Mat srcIm)
{
	Mat tmpIm = Mat::zeros(srcIm.size(), srcIm.type());
	Mat tmpSrcMsk = Mat(srcIm.size(), CV_8UC1, Scalar(255));;
	
	//correct(tmpSrcMsk, srcMeanG, refMeanG, rsFactorG, tmpIm, srcLabIm);

	int width = srcIm.size().width;
	int height = srcIm.size().height;

	double alpha = 14;
	double beta = 100;

	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			Vec3b rawcolor = srcIm.at<Vec3b>(y,x);
				
			double sumL = 0.0, sumA = 0.0, sumB = 0.0;
			double weightSumL = 0.0, weightSumA = 0.0, weightSumB = 0.0;
					
			int idx = srcLabels[y*width+x];
			
			for(int i = 0; i < regionum; ++i)
			{
				if(srcPixels[i].size() == 0)
					continue;

				double dist ;
				/*if(idx == i)
					dist = 0;
				else 
					dist = calMinDist(Point2f(x,y), srcPixels[i]);*/
				/*if(idx == i)
					dist = 0;
				else 
					dist = sqrt((srcCenters[i].x - x)*(srcCenters[i].x - x) + (srcCenters[i].y - y)*(srcCenters[i].y - y));*/
				
				//double weightD = exp(-(dist * dist) / 2 * beta * beta) / (sqrt(2*_PI)*beta);
				double weightD = 1;

				Scalar srcMean = srcMeans[i];
				Scalar refMean = refMeans[i];
				Scalar rsFactor = rsFactors[i];

				double deltaL = rawcolor[0] - srcMean.val[0];
				double deltaA = rawcolor[1] - srcMean.val[1];
				double deltaB = rawcolor[2] - srcMean.val[2];

				/*double sigmaL = srcStddevs[i].val[0];
				double sigmaA = srcStddevs[i].val[1];
				double sigmaB = srcStddevs[i].val[2];*/

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
	}

	/*imwrite("Lab.png", tmpLabIm);*/

	cvtColor(tmpIm, init_weight_corr, CV_Lab2BGR);
	//init_weight_corr = tmpIm.clone();
	cout << "init weighted correction done." << endl;

	/*imshow("weighted correction", wcorrectedSrcIm);
	waitKey(0);
	destroyWindow("weighted correction");*/
	
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


void CCorrecter::showCorrectedResult()
{
	imshow("simple correction", final_correctedSrcIm);
	waitKey(0);
	destroyWindow("simple correction");

	imshow("weighted correction", final_wcorrectedSrcIm);
	waitKey(0);
	destroyWindow("weighted correction");
}

void CCorrecter::Correction(Mat srcIm, Mat refIm, string sceneFn, string folder, string srcFn) {

	calGlobalParameters(srcIm, refIm);//计算全局参数

	calLocalParameters(srcIm, refIm);//计算局部参数
		
	simpleCorrection(srcIm);

	weightedCorrection(srcIm);
}

void CCorrecter::Combine(Mat reflectance, Mat shading, string resultFn, string folder) {
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
				if(val[k] <= 0.0031308) {
					val[k] = val[k] * 12.92;
				}
				else if(val[k] > 0.0031308) {
					val[k] = pow(1.055 * val[k], 1.0 / 2.4) - 0.055;
				}
			}
			srcresult.at<Vec3b>(y, x)[0] = (unsigned char)(max(0.0, min(val[2] * 255.0, 255.0)));
			srcresult.at<Vec3b>(y, x)[1] = (unsigned char)(max(0.0, min(val[1] * 255.0, 255.0)));
			srcresult.at<Vec3b>(y, x)[2] = (unsigned char)(max(0.0, min(val[0] * 255.0, 255.0)));
		}
	}
	string fileFn = folder + "/" + resultFn + ".png";
	cout << "save " + fileFn + " done"<<endl;
	imwrite(fileFn, srcresult);
}