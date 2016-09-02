#include "transfer.h"
#include "function.h"

//像素按照标签值分类，只考虑可见像素
void RWTransfer::labels2pixels(const vector<int>& labels, const int& regionum, vector<vector<Point2f>>& pixels)
{
	pixels.clear();
	pixels.resize(regionum);

	int width = srcIm.size().width;
	int height = srcIm.size().height;

	for(int i = 0; i < labels.size(); ++i)
	{
		int y = i / width;
		int x = i % width;
		int l = labels[i];
		pixels[l].push_back(Point2f(x,y));
	}
}

//可见性?
void RWTransfer::pixels2labels(const vector<vector<Point2f>>& pixels, vector<int>& labels, int regionum)
{
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	labels.clear();
	labels.resize(height * width, -1);

	for(int i = 0; i < regionum; ++i)
	{
		for(int j = 0; j < pixels[i].size(); ++j)
		{
			Point2f p = pixels[i][j];

			int index = static_cast<int>(floor(p.y)) * width + static_cast<int>(floor(p.x));
			labels[index] = i;
		}
	}
}

void RWTransfer::calDispMat()
{
	if(srcDispIm.data == nullptr || refDispIm.data == nullptr)
	{
		cout << "read in disparity first." << endl;
		return;
	}

	float scale = 1.0f;
	int offset = 0;

	srcDispMat = Mat::zeros(srcDispIm.size(), CV_8UC1);
	refDispMat = Mat::zeros(refDispIm.size(), CV_8UC1);
	
	int height = srcDispIm.size().height;
	int width  = srcDispIm.size().width;
	for(int y = 0; y < height; ++y)
	{
		for(int x = 0; x < width; ++x)
		{
			int sd = srcDispIm.at<uchar>(y,x);
			int rd = refDispIm.at<uchar>(y,x);

			srcDispMat.at<uchar>(y,x) = static_cast<int>(sd * (1 / scale) + 0.5f) + offset;
			refDispMat.at<uchar>(y,x) = static_cast<int>(rd * (1 / scale) + 0.5f) + offset;
		}
	}
}

//不考虑可见性
//void RWTransfer::labels2pixelsD(const vector<int>& labels, const int& regionum,  vector<vector<Point2f>>& pixels)
//{
//	pixels.clear();
//	pixels.resize(regionum);
//
//	int width = srcIm.size().width;
//	int height = srcIm.size().height;
//
//	for(int i = 0; i < labels.size(); ++i)
//	{
//		int y = i / width;
//		int x = i % width;
//		int l = labels[i];
//
//		//add visibility
//		/*if(visIm.at<uchar>(y,x) == 0)*/
//			pixels[l].push_back(Point2f(x,y));
//	}
//}

void RWTransfer::initial(const string& folder, const string& sceneFn, const string& refFn, const string& srcFn)
{
	this->sceneFn = sceneFn;

	//读取图像
	string refImFn = folder + "/" +  refFn + ".png";
	string srcImFn = folder + "/" +  srcFn + ".png";
	
	readImage(refImFn, refRAWIm, CV_LOAD_IMAGE_UNCHANGED);
	readImage(srcImFn, srcRAWIm, CV_LOAD_IMAGE_UNCHANGED);

	if(refRAWIm.channels() == 1)
	{
		cvtColor(refRAWIm, refIm, CV_GRAY2BGR);
		cvtColor(srcRAWIm, srcIm, CV_GRAY2BGR);		
	}
	else
	{
		refIm = refRAWIm.clone();
		srcIm = srcRAWIm.clone();
	}

	//读取分割图像
	srcImFn = folder + "/"+ srcFn + "_con_25_9_500.png";
	readImage(srcImFn, srcSegmentsIm, CV_LOAD_IMAGE_UNCHANGED);
	refImFn = folder + "/"+ refFn + "_con_7_4_300.png";
	readImage(refImFn, refSegmentsIm, CV_LOAD_IMAGE_UNCHANGED);

	//读取可见性图像
	refImFn = folder + "/visibility_" + refFn + ".png";
	srcImFn = folder + "/visibility_" + srcFn + ".png";
	readImage(refImFn, refVisIm, CV_LOAD_IMAGE_GRAYSCALE);
	readImage(srcImFn, srcVisIm, CV_LOAD_IMAGE_GRAYSCALE);

	//读取分割标签
	string srcLabelFn = folder + "/" + srcFn + "_labels_25_9_500.txt";
	readLabels(srcLabelFn, srcregionum, srcLabels);
	cout << "src: " << srcregionum << " regions." << endl;

	string refLabelFn = folder + "/" + refFn + "_labels_7_4_300.txt";
	readLabels(refLabelFn, refregionum, refLabels);
	cout << "ref: " << refregionum << " regions." << endl;
	
	//根据标签获得每个区域的Pixels
	labels2pixels(srcLabels, srcregionum, srcPixels);
	labels2pixels(refLabels, refregionum, refPixels);
	cout << "get region pixels correspondence" <<endl;
	
	//读取区域平均颜色，这里颜色用的是reflectance的颜色
	calRColors(srcregioncolor, srcregionum, srcPixels, srcIm);
	calRColors(refregioncolor, refregionum, refPixels, refIm);
	cout << "get region meancolors" << endl;

	//计算每个区域的中心像素
	calRCenters(folder + "/centers_" + srcFn + ".png", srcCenters, srcregionum, srcPixels, srcSegmentsIm);
	calRCenters(folder + "/centers_" + refFn + ".png", refCenters, refregionum, refPixels, refSegmentsIm);	
	cout << "get region centers" << endl;

	//读取匹配点
	string matchFn = folder + "/matches.txt";
	readMatches(matchFn, matches, refVisIm, srcVisIm);
	/*matchFn = folder + "/refine_matches_more_" + srcFn + ".txt";
	readMatches(matchFn, matchesmore);*/
	//为ref2src设置匹配标签
	labelmap.resize(refregionum, -1);

	//邻接表
	calRAList(srcRAlist, srcIm, srcregionum, srcPixels, "src", srcLabels);
	calRAList(refRAlist, refIm, refregionum, refPixels, "ref", refLabels);

	//设置多阶邻域
	//multinum只取2或3 二阶邻域或三阶邻域
	level = 2;
	calmultilevelRA(srcRAlist, multisrcRAlist, level);
	calmultilevelRA(refRAlist, multirefRAlist, level);
	
	//match初始化

	src2refmap.resize(srcregionum);
	srcregionflag.resize(srcregionum, -1);
	refregionflag.resize(refregionum, -1);
}

void RWTransfer::readMatfromfile(string ImFn, Mat& RAWIm, int rows, int cols, int channels)
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

void RWTransfer::calRColors(vector<Scalar>& regioncolor, int regionum, vector<vector<Point2f>> Pixels, Mat Im)
{
	regioncolor.resize(regionum, 0);
	//每个区域的平均颜色和
	Scalar rcolorsum;

	//对于每一个区域，将区域所有颜色相加，再除以区域点个数
	for(int i = 0; i < regionum; ++i)
	{
		int rnum = Pixels[i].size();//区域点个数
		rcolorsum[0] = 0.0;
	    rcolorsum[1] = 0.0;
	    rcolorsum[2] = 0.0;

		for(int j = 0; j < rnum; ++j)
		{
			int x = Pixels[i][j].x;
			int y = Pixels[i][j].y;
			rcolorsum[0] += Im.at<Vec3b>(y, x)[0];
			rcolorsum[1] += Im.at<Vec3b>(y, x)[1];
			rcolorsum[2] += Im.at<Vec3b>(y, x)[2];
		}
		regioncolor[i][0] = rcolorsum[0] / rnum;
		regioncolor[i][1] = rcolorsum[1] / rnum;
		regioncolor[i][2] = rcolorsum[2] / rnum;

	}
}
void RWTransfer::calRCenters(const string& fn, vector<Point2f>& Centers, int regionum, vector<vector<Point2f>> Pixels, Mat SegmentsIm)
{
	Centers.clear();
	Centers.resize(regionum);

	for(int i = 0; i < regionum; ++i)
	{
		// calculate the center of a region

		vector<Point2f>& p = Pixels[i];

		int size = p.size();
		float sumX = 0;
		float sumY = 0;
		for(int j = 0; j < size; ++j)
		{
			sumX += p[j].x;
			sumY += p[j].y;			
		}

		Centers[i].x = sumX / size;
		Centers[i].y = sumY / size;
	}

	// show centers
	Mat tim = SegmentsIm.clone();
	for(int i = 0; i < regionum; ++i)
	{
		cv::circle(tim, Centers[i], 2, Scalar(255, 255, 0), 2);
	}
	imwrite(fn, tim);
}

void RWTransfer::calSegmentsIm(const string sfn, Mat Im, int regionum, vector<vector<Point2f>> Pixels, Mat SegmentsIm, vector<int> regionflag)
{
	Filter(Im, regionum, Pixels, SegmentsIm, regionflag);
	if(sfn != "")
	{
		imwrite(sfn, SegmentsIm);
		cout << "save " << sfn << endl;		
	}
}

void RWTransfer::showSameColor(string sfn, string flag)
{
	Mat markref, marksrc;
	string sfn1 = sfn + "/" + flag + "_samecolor_" + "ref.png";
	string sfn2 = sfn + "/" + flag + "_samecolor_" + "src.png";
	GetColorForRegion(srcIm, srcPixels, refPixels, src2refmap, markref, marksrc);
	if(sfn1 != "")
	{
		imwrite(sfn1, markref);
		cout << "save " << sfn1 <<endl;
	}
	if(sfn2 != "")
	{
		imwrite(sfn2, marksrc);
		cout << "save " << sfn2 <<endl;
	}

}


int RWTransfer::InvalidRegion(vector<int> regionflag)
{
	int sz = regionflag.size();
	int invalidr = 0;
	for(int i = 0; i < sz; ++i)
	{
		if(regionflag[i] == -1)
		{
			invalidr++;
		}
	}
	return invalidr;
}

void RWTransfer::FuseRegion(vector<int>& regionflag, vector<vector<Point2f>>& Pixels, int regionum, vector<vector<bool>> RAlist, 
	vector<Point2f> Centers, vector<Scalar> regioncolor, vector<int>& Labels)
{
	int disp_min = 0;
	int region_min = 0;
	for(int i = 0; i < regionum; ++i)
	{
		if(Pixels[i].size() == 0)//如果被合并了定为-2
		{
			regionflag[i] = -2;
		}
		if(regionflag[i] == -1)//区域i没有匹配点
		{
			disp_min = 10000000;
			region_min = -1;
			int j = 0;
			for(j = 0; j < regionum; ++j)
			{
				if(RAlist[i][j] == true && regionflag[j] == 1)//如果在ref中，区域i和区域j相邻, 且区域j有匹配点
				{
					Point2f ic = Centers[i];
					Point2f jc = Centers[j];
					Scalar iv = regioncolor[i];
					Scalar jv = regioncolor[j];
					int disp = 0.0*((ic.x - jc.x) * (ic.x - jc.x) +
						(ic.y - jc.y) * (ic.y - jc.y)) +
						1.0*((iv.val[0] - jv.val[0]) * (iv.val[0] - jv.val[0]) +
						(iv.val[1] - jv.val[1]) * (iv.val[1] - jv.val[1]) +
						(iv.val[2] - jv.val[2]) * (iv.val[2] - jv.val[2]));
					if(disp < disp_min)
					{
						disp_min = disp;
						region_min = j;
					}
				}
			}
			//找到区域j，把区域i中所有点加到区域j中
			if(region_min != -1)
			{
			    vector<Point2f> tempp = Pixels[i];
			    int st = tempp.size();
			    for(int k = 0; k < st; ++k)
			    {
				    Pixels[region_min].push_back(tempp[k]);
			    }
				Pixels[i].clear();//存储第i个标签的所有像素删除，没有点就可以了，不能删掉，不然标签会乱
				regionflag[i] = -2;//第i个region标签为-2表示该标签已经不存在
				int sz = Labels.size();
				for(int k = 0; k < sz; ++k)
				{
					if(Labels[k] == i)
					{
						Labels[k] = region_min;
					}
				}
			}
		}
	}
}

void RWTransfer::Recombine(vector<vector<Point2f>>& Pixels, vector<int>& labels, Mat Im, int& rnum)
{
	int sz = Pixels.size();
	int width = Im.size().width;
	int height = Im.size().height;
	rnum = 0;
	labels.clear();
	labels.resize(width * height, -1);


	for(int i = 0; i < sz; ++i)
	{
		if(Pixels[i].size() == 0)
		{
			Pixels.erase(Pixels.begin() + i);
			i = i-1;
			sz = sz -1;
			continue;
		}
		if(Pixels[i].size() != 0)
		{
			int szp = Pixels[i].size();
			for(int j = 0; j < szp; ++j)
			{
				int y = Pixels[i][j].y;
				int x = Pixels[i][j].x;
				labels[y * width + x] = i;
			}
			rnum++;
		}
	}
}

void RWTransfer::dealSrcNopoint(const string folder, const string srcFn, vector<int> regionflag)
{
	//找没有匹配点区域
	cout << "deal src image the regions have no match points" << endl;
	int width = srcIm.size().width;
	int height = srcIm.size().height;
	int disp_min = 0, region_min = 0;

	//输出来看看哪些区域没有匹配点，没有匹配时显示黑色
	string sfn = folder + "/F_match2region_" + srcFn + ".png";
	calSegmentsIm(sfn, srcIm, srcregionum, srcPixels, srcSegmentsIm, regionflag);

	//对每个没有匹配点区域，找相邻区域，找到颜色和距离最小的区域编号
	while(int invalidr = InvalidRegion(regionflag))
	{
		FuseRegion(regionflag, srcPixels, srcregionum, srcRAlist, srcCenters, srcregioncolor, srcLabels);
		cout<< "invalidr: " << invalidr <<endl;
		sfn = folder + "/L_match2region_" + srcFn + ".png";
	    calSegmentsIm(sfn, srcIm, srcregionum, srcPixels, srcSegmentsIm, regionflag);
		Mat srctemp;
		readImage(sfn, srctemp, CV_LOAD_IMAGE_UNCHANGED);
		calRAList(srcRAlist, srctemp, srcregionum, srcPixels, "src", srcLabels);
	}

	sfn = folder + "/L_match2region_" + srcFn + ".png";
	calSegmentsIm(sfn, srcIm, srcregionum, srcPixels, srcSegmentsIm, regionflag);
}

void RWTransfer::FirstSrcCombine(const string folder, const string srcFn)
{
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	vector<map<int, int>> ref2srcmatch(refregionum);
	vector<vector<int>> ref2srcmap(refregionum);
	
	//对于每一个match点，找到ref和src的标签
	int sz = matches.size();
	for(int i = 0; i < sz; ++i)
	{
		CMatch m = matches[i];
		int srcid = m.srcPt.y * width + m.srcPt.x;
		int refid = m.refPt.y * width + m.refPt.x;

		int slabel = srcLabels[srcid];
		int rlabel = refLabels[refid];

		if(rlabel == -1 || slabel == -1)//如果有一方没有标签则继续（这里没有标签为-1）
		{
			continue;
		}

		if(ref2srcmatch[rlabel].find(slabel) != ref2srcmatch[rlabel].end())//如果之前有，则++，如果没有，则赋为1
		{
			ref2srcmatch[rlabel][slabel]++;//ref中每一个区域，对应的src的区域标签及数量
		}
		else
		{
		    ref2srcmatch[rlabel][slabel] = 1;
		}
	}
	map<int, int>::iterator it;

	//对于ref的每一个区域，找到src的对应区域，得到r2smap,即ref的i区域内容为其对应的src的区域标签
	for(int i = 0; i < refregionum; ++i)
	{
		for(it = ref2srcmatch[i].begin(); it != ref2srcmatch[i].end(); ++it)
		{
			if((*it).second > 3) {
			    ref2srcmap[i].push_back((*it).first);
			}
		}
	}

	//根据对应好的区域融合，并将label标签信息换掉
	for(int i = 0; i < refregionum; ++i)
	{
		//r2smap需要做一个排序
		if(ref2srcmap[i].size() != 0) {
			//对对应的每一项排序
			sort(ref2srcmap[i].begin(), ref2srcmap[i].end());
			//删除重复项
			ref2srcmap[i].erase(unique(ref2srcmap[i].begin(), ref2srcmap[i].end()), ref2srcmap[i].end());
		}

		int sz = ref2srcmap[i].size();
		if(sz == 0 )//如果ref对应的区域只有0个或1个，则不需要融合
		{
			continue;
		}	
		if(sz == 1)
		{
			srcregionflag[ref2srcmap[i][0]] = 1;//如果有一个对应，则将该对应加进src区域对应中
			continue;
		}

		//如果有多个对应，则取最小区域编号作为src区域对应，合并进最小编号中
		vector<Point2f>::iterator iter;
		int rmin = ref2srcmap[i][0];

		srcregionflag[rmin] = 1;
		for(int j = 1; j < sz; ++j)
		{
			int srcr = ref2srcmap[i][j];//ref对到src的需要被合并的区域编号
			for(iter = srcPixels[srcr].begin(); iter != srcPixels[srcr].end(); ++iter)
			{
				srcPixels[rmin].push_back(*iter);
			}
			srcPixels[srcr].clear();
			srcregionflag[srcr] = -2;
		}
		ref2srcmap[i].erase(ref2srcmap[i].begin()+1, ref2srcmap[i].end());
	}
	for(int i = 0; i < srcregionum; ++i) {
		if(srcPixels[i].size() == 0) {
			srcregionflag[i] = -2;
		}
	}
	pixels2labels(srcPixels, srcLabels, srcregionum);
	string sfn = folder + "/FirstSrcCombine_" + srcFn + ".png";
	calSegmentsIm(sfn, srcIm, srcregionum, srcPixels, srcSegmentsIm, srcregionflag);
	calRAList(srcRAlist, srcIm, srcregionum, srcPixels, "src", srcLabels);
	calmultilevelRA(srcRAlist, multisrcRAlist, level);

	calRColors(srcregioncolor, srcregionum, srcPixels, srcIm);

}

void RWTransfer::FuseSrcNopoint(const string folder, const string srcFn)//把src未匹配区域根据颜色和中心距离，找所有其它区域，找到最相近的合并
{
	double mindis;
	double dis;
	int rnum;
	for(int i = 0; i < srcregionum; ++i)
	{
		if(srcregionflag[i] == -1)//如果该区域没有被匹配
		{
			mindis = 20000000;
			Point2f p1 = srcCenters[i];//该区域中心点
			int x1 = p1.x;
			int y1 = p1.y;
			Scalar color1 = srcregioncolor[i];//i区域平均颜色
			for(int j = 0; j < srcregionum; ++j)
			{
				if(multisrcRAlist[i][j] == true && i != j && srcregionflag[j] == 1)//如果找到了邻接区域且该区域为匹配区域
				{
					Point2f p2 = srcCenters[j];//该区域中心点坐标
				    int x2 = p2.x;
				    int y2 = p2.y;
				    Scalar color2 = srcregioncolor[j];//j区域平均颜色
				    dis = (color1[0] - color2[0])*(color1[0] - color2[0])+
					      (color1[1] - color2[1])*(color1[1] - color2[1])+
					      (color1[2] - color2[2])*(color1[2] - color2[2]);
				    if(dis < mindis)
				    {
					    mindis = dis;
					    rnum = j;
				    }
				}

			}

			vector<Point2f>::iterator it;
			for(it = srcPixels[i].begin(); it != srcPixels[i].end(); ++it)
			{
				srcPixels[rnum].push_back(*it);//把未匹配的i区域中的所有点放入rnum中
			}
			srcPixels[i].clear();
			srcregionflag[i] = -2;
			continue;

		}
	}
	pixels2labels(srcPixels, srcLabels, srcregionum);
	string sfn = folder + "/SecondSrcCombine_" + srcFn + ".png";
	calSegmentsIm(sfn, srcIm, srcregionum, srcPixels, srcSegmentsIm, srcregionflag);

	
	sfn = folder + "/Secondrflag_" + srcFn + ".png";
	Mat marksrc;
	SaveRegionFlag(sfn, marksrc, srcIm, srcPixels, srcregionflag);//保存src图像中同样标记的Pixels

	calRAList(srcRAlist, srcIm, srcregionum, srcPixels, "src", srcLabels);

	calmultilevelRA(srcRAlist, multisrcRAlist, level);

	calRColors(srcregioncolor, srcregionum, srcPixels, srcIm);
}

void RWTransfer::SaveRegionFlag(string sfn, Mat& mark, Mat Im, vector<vector<Point2f>> Pixels, vector<int> regionflag)
{
	//-------------------------------
	int width = Im.size().width;
	int height = Im.size().height;
	int sz = width * height;
	int regionum = Pixels.size();

	//表示标记后的源图像和参考图像
	mark = Mat::zeros(height, width, CV_8UC3);
	Scalar color;
	CvRNG rng;  
	for(int i = 0; i < regionum; ++i)
	{		
		//产生随机颜色		
        rng= cvRNG(cvGetTickCount());  
		for(int j = 0; j < 3; ++j)
		{
			color.val[j] = cvRandInt(&rng)%256;//为每一通道产生一个随机颜色，为0-255之间的正整数
		}
		//用该随机颜色填充对应区域
		if(regionflag[i] == 1)
		{
		    for(int j = 0; j < Pixels[i].size(); ++j)
		    {
			    mark.at<Vec3b>(Pixels[i][j].y, Pixels[i][j].x)[0] = color.val[0];
			    mark.at<Vec3b>(Pixels[i][j].y, Pixels[i][j].x)[1] = color.val[1];
			    mark.at<Vec3b>(Pixels[i][j].y, Pixels[i][j].x)[2] = color.val[2];
		    }
		}
	}
	imwrite(sfn, mark);
}
void RWTransfer::Matches(const string folder, const string srcFn, const string refFn)
{
	vector<map<int, int>> ref2srcmatch(refregionum);
	vector<map<int, int>> src2refmatch(srcregionum);
	vector<vector<int>> ref2srcmap(refregionum);

	int width = srcIm.size().width;
	int height = srcIm.size().height;

	int sz = matches.size();
	for(int i = 0; i < sz; ++i)
	{
		CMatch m = matches[i];
		int srcid = m.srcPt.y * width + m.srcPt.x;
		int refid = m.refPt.y * width + m.refPt.x;

		int slabel = srcLabels[srcid];
		int rlabel = refLabels[refid];

		if(rlabel == -1 || slabel == -1)//没有-1
		{
			continue;
		}
		if(ref2srcmatch[rlabel].find(slabel) != ref2srcmatch[rlabel].end())//如果之前有，则++，如果没有，则赋为1
		{
			ref2srcmatch[rlabel][slabel]++;//ref中每一个区域，对应的src的区域标签及数量
		}
		else
		{
		    ref2srcmatch[rlabel][slabel] = 1;
		}
	}
	map<int, int>::iterator it;
	for(int k = 0; k < refregionum; ++k) {
		for(it = ref2srcmatch[k].begin(); it != ref2srcmatch[k].end(); ++it) {
			if((*it).second > 3) {
				ref2srcmap[k].push_back((*it).first);
				src2refmap[(*it).first].push_back(k);
			}
		}
	}
	for(int i = 0; i < srcregionum; ++i)
	{
		//对对应的每一项排序
		sort(src2refmap[i].begin(), src2refmap[i].end());
		//删除重复项
		src2refmap[i].erase(unique(src2refmap[i].begin(), src2refmap[i].end()), src2refmap[i].end());
	}
	for(int i = 0; i < srcregionum; ++i)
	{
		int sz = src2refmap[i].size();
		if(sz == 0)
		{
			continue;
		}
		if(sz == 1)
		{
			int refnum = src2refmap[i][0];
			refregionflag[refnum] = 1;
			continue;
		}
		vector<Point2f>::iterator it;
		int rmin = src2refmap[i][0];//最小的保存所有点的ref标签
		refregionflag[rmin] = 1;
		for(int j = 1; j < sz; ++j)
		{
			int refnum = src2refmap[i][j];
			for(it = refPixels[refnum].begin(); it != refPixels[refnum].end(); ++it)
			{
				refPixels[rmin].push_back(*it);
			}
			refPixels[refnum].clear();
			refregionflag[refnum] = -2;
		}
		src2refmap[i].erase(src2refmap[i].begin()+1, src2refmap[i].end());//删掉后面无用的标签
	}
	pixels2labels(refPixels, refLabels, refregionum);

	string sfn = folder + "/ref_match_region_" + refFn + ".png";
	calSegmentsIm(sfn, refIm, refregionum, refPixels, refSegmentsIm, refregionflag);
	sfn = folder + "/temp_" + srcFn + ".png";
	calSegmentsIm(sfn, srcIm, srcregionum, srcPixels, srcSegmentsIm, srcregionflag);

	calRAList(refRAlist, refIm, refregionum, refPixels, "ref", refLabels);
	calmultilevelRA(refRAlist, multirefRAlist, level);

	calRColors(refregioncolor, refregionum, refPixels, refIm);

}

void RWTransfer::FuseRef(const string folder, const string refFn)
{
	invalidrefregion = 1;
	while(invalidrefregion)
	{
		FuseRefNopoint(folder, refFn);
	}
}
void RWTransfer::FuseRefNopoint(const string folder, const string refFn)//先看相邻区域平均颜色差异，如果相邻区域最小平均颜色差异大于一定阈值，则在所有范围内搜
{
	double mindis;
	double dis;
	int rnum = -1;
	for(int i = 0; i < refregionum; ++i)
	{
		if(refregionflag[i] == -1)//如果该区域没有被匹配
		{
			rnum = -1;
			mindis = 20000000;
			Scalar color1 = refregioncolor[i];
			for(int j = 0; j < refregionum; ++j)
			{
				if(multirefRAlist[i][j] == true && i != j && refregionflag[j] == 1)//如果找到了相邻区域且该区域为匹配区域
				{
					Point2f p2 = refCenters[j];//该区域中心点坐标
				    int x2 = p2.x;
				    int y2 = p2.y;
				    Scalar color2 = refregioncolor[j];//该区域颜色
				    dis = (color1[0] - color2[0])*(color1[0] - color2[0])+
					      (color1[1] - color2[1])*(color1[1] - color2[1])+
					      (color1[2] - color2[2])*(color1[2] - color2[2]);
				    if(dis < mindis)
				    {
					    mindis = dis;
					    rnum = j;
				    }
				}

			}
			if(mindis < 20000000)
			{
				vector<Point2f>::iterator it;
			    for(it = refPixels[i].begin(); it != refPixels[i].end(); ++it)
			    {
				    refPixels[rnum].push_back(*it);//把未匹配的i区域中的所有点放入rnum中
			    }
				refPixels[i].clear();
				refregionflag[i] = -2;
				continue;
			}
		}
	}
	pixels2labels(refPixels, refLabels, refregionum);
	string rfn = folder + "/RefCombine_" + refFn + ".png";
	calSegmentsIm(rfn, refIm, refregionum, refPixels, refSegmentsIm, refregionflag);
	calRAList(refRAlist, refIm, refregionum, refPixels, "ref", refLabels);
	calmultilevelRA(refRAlist, multirefRAlist, level);

	calRColors(refregioncolor, refregionum, refPixels, refIm);

	invalidrefregion = 0;
	for(int i = 0; i < refregionum; ++i)
	{
		if(refregionflag[i] == -1)
		{
			invalidrefregion++;
		}
	}
}

void RWTransfer::estimateHomography()
{
	isMatched.resize(regionum);
	homographys.resize(regionum);
	
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	vector<vector<Point2f>> srcFeatures(regionum);
	vector<vector<Point2f>> refFeatures(regionum);

	//assigning matches into each region
	int sz = matches.size();
	for(int i = 0; i < sz; ++i)
	{
		CMatch m = matches[i];
		
		int sidx = m.srcPt.y * width + m.srcPt.x;
		int slabel = srcLabels[sidx];

		//only visible matches
		if(srcVisIm.at<uchar>(m.srcPt.y, m.srcPt.x) == 0)
		{
			refFeatures[slabel].push_back(m.refPt);
			srcFeatures[slabel].push_back(m.srcPt);
		}
	}

	//estimating homography matrixes
	for(int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& srcFts = srcFeatures[i];
		vector<Point2f>& refFts = refFeatures[i];

		Mat H;

		if(srcFts.size() == 0)
		{
			H = Mat::zeros(3,3,CV_64FC1);
			isMatched[i] = 0;
		}
		else if(srcFts.size() > 0 && srcFts.size() < 4)
		{
			int deltax = 0, cnt = srcFts.size();
			for (int j = 0; j < cnt; ++j)
			{
				deltax += (refFts[j].x - srcFts[j].x);
			}
			deltax /= cnt;

			H = Mat::zeros(3,3,CV_64FC1);
			H.at<double>(0,2) = deltax * 1.0;
			isMatched[i] = 1;
		}
		else
		{
			H = findHomography(srcFts, refFts, RANSAC);
			isMatched[i] = 2;			
		}
		
		homographys[i] = H.clone();
	}
	
	cout << "estimate homography done." << endl;
}

void RWTransfer::project()
{
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	refPixels.resize(regionum);
	for (int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& srcPts = srcPixels[i];
		Mat H = homographys[i];
		
		//objective
		vector<Point2f>& refPts = refPixels[i];

		if (isMatched[i] == 0)
		{
			//cout << i << "-th region: no matches." << endl; 
			refPixels[i].clear();
			//if(srcDispMat.data != nullptr)
			dealUnMatchedRegion(i, srcPixels[i], refPixels[i]);

		}
		else if (isMatched[i] == 1)
		{
			//cout << i << "-th region: degrade into an translation." << endl;
			int deltax = static_cast<int>(H.at<double>(0,2));
			for (int j = 0; j < srcPts.size(); ++j)
			{
				Point2f sPt = srcPts[j];
				Point2f rPt = Point2f(static_cast<int>(floor(sPt.x + deltax)), static_cast<int>(floor(sPt.y))); 

				if(rPt.y >= 0 && rPt.y < height && rPt.x >= 0 && rPt.x < width)
					refPts.push_back(rPt);
			}
		}
		else
		{
			//cout << i << "-th region: perspective transform." << endl;
			vector<Point2f> trefPts;
			cv::perspectiveTransform(srcPts, trefPts, H);
			for(int j = 0; j < trefPts.size(); ++j)
			{
				Point2f rpt = Point2f(static_cast<int>(floor(trefPts[j].x)), static_cast<int>(floor(trefPts[j].y)));
				if(rpt.y >= 0 && rpt.y < height && rpt.x >= 0 && rpt.x < width)
					refPts.push_back(trefPts[j]);
			}
		}
	}

	//1. 形态学, 投影得到连续的区域
	//2. 去掉reference中可见性为0的点
	cout << "no matched region: " << endl ;
	for(int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& refPts = refPixels[i];
		vector<Point2f> newrefPts(0);
		if(refPts.size() == 0)
		{
			cout << i << endl;
		}
		else
		{
			Mat msk = Mat::zeros(refIm.size(), CV_8UC1);
			calMask(refPts, msk);

			//erosion one time
			Mat element = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
			Mat out;
			cv::erode(msk, out, element);
						
			//dilation two times
			element = getStructuringElement(MORPH_RECT, cv::Size(3,3));
			cv::dilate(out, msk, element);
			cv::dilate(msk, out, element);
			msk = out.clone();
				
			//convert mask into visible ref pixels
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					if (msk.at<uchar>(y,x) == 255 /*&& refVisIm.at<uchar>(y,x)== 0*/)
					{
						newrefPts.push_back(Point2f(x,y));
					}
				}
			}
		}
		refPts.clear();
		copy(newrefPts.begin(), newrefPts.end(), back_inserter(refPts));
	}

	pixels2labels(refPixels, refLabels, refregionum);
	cout << "project source to reference done." << endl;
}

void RWTransfer::dealUnMatchedRegion(const int srcLabel, const vector<Point2f>& srcPts, vector<Point2f>& refPts)
{
	//1. calculate the center of srcPts
	//2. search the nearest matched SIFT　matches
	//3. copy the homography matrix
	//4. copy the colour transfer function.

	cout << "deal un-matched region : " << srcLabel << ", ";

	if(srcPts.size() == 0)		return ;
	
	int height = srcIm.size().width;
	int width = srcIm.size().width;

	Point2f scenter = srcCenters[srcLabel];
	Point2f nearSrcPt ;

	double minima = 0;
	for(int i = 0; i < matches.size(); i++ )
	{
		Point2f t_srcpt = matches[i].srcPt;
		int tidx = t_srcpt.y * width + t_srcpt.x;
		int tlabel = srcLabels[tidx];

		if(isMatched[tlabel] == false) continue;

		double dist2 = (t_srcpt.x - scenter.x) * (t_srcpt.x - scenter.x) + 
			           (t_srcpt.y - scenter.y) * (t_srcpt.y - scenter.y);
		
		if(dist2 > minima)
		{
			minima = dist2;
			nearSrcPt = t_srcpt;
		}
	}

	int nearLabel = srcLabels[nearSrcPt.y * width + nearSrcPt.x];
	homographys[srcLabel] = homographys[nearLabel].clone();
		
	cout << "nearest label " << nearLabel << endl;
	cout << homographys[srcLabel] << endl;
	
	vector<Point2f> trefPts;
	cv::perspectiveTransform(srcPts, trefPts, homographys[srcLabel]);
	cout << "perspective transform done" << endl;

	for(int j = 0; j < trefPts.size(); ++j)
	{
		Point2f rpt = Point2f(static_cast<int>(floor(trefPts[j].x)), static_cast<int>(floor(trefPts[j].y)));
		if(rpt.y >= 0 && rpt.y < height && rpt.x >= 0 && rpt.x < width)
			refPts.push_back(trefPts[j]);
	}
}

//using disparity information
void RWTransfer::projectD()
{
	cout << "project by disparity result." << endl;

	int width = srcIm.size().width;
	int height = srcIm.size().height;

	refPixels.resize(regionum);
	for (int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& srcPts = srcPixels[i];
		vector<Point2f>& refPts = refPixels[i];

		int sz = srcPts.size();
		for(int j = 0; j < sz; ++j)
		{
			Point2f sp = srcPts[j];
			int d = srcDispMat.at<uchar>(sp.y, sp.x);

			if(d == 0)
				continue;
		
			Point2f rp  =  Point2f(sp.x + d, sp.y);

			if(rp.x >= 0 && rp.x < width && rp.y >= 0 && rp.y < height)
				refPts.push_back(rp);
		}
	}

	cout << "no matched region: " << endl ;
	for(int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& refPts = refPixels[i];
		vector<Point2f> newrefPts(0);
		if(refPts.size() == 0)
		{
			cout << i << endl;
		}
		else
		{
			Mat msk = Mat::zeros(refIm.size(), CV_8UC1);
			calMask(refPts, msk);

			Mat element = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
			Mat out;
			cv::erode(msk, out, element);
						
			element = getStructuringElement(MORPH_RECT, cv::Size(3,3));
			cv::dilate(out, msk, element);
			cv::dilate(msk, out, element);
			msk = out.clone();
				
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					if (msk.at<uchar>(y,x) == 255 /*&& refVisIm.at<uchar>(y,x)== 0*/)
					{
						newrefPts.push_back(Point2f(x,y));
					}
				}
			}
		}
		refPts.clear();
		copy(newrefPts.begin(), newrefPts.end(), back_inserter(refPts));
	}

	pixels2labels(refPixels, refLabels, refregionum);
	cout << "project source to reference done." << endl;
}

void RWTransfer::saveRegions()
{
	string dir = "output/" + sceneFn + "_regions";
	_mkdir(dir.c_str());

	for(int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& srcPts = srcPixels[i];
		
		Mat msk = Mat::zeros(srcIm.size(), CV_8UC1);
		calMask(srcPts, msk);

		Mat region;
		srcIm.copyTo(region, msk);
		
		string sfn = dir + "/src_region_" + type2string<int>(i) + ".png";
		imwrite(sfn, region);

		sfn = dir +  "/src_mask_" + type2string<int>(i) + ".png";
		imwrite(sfn, msk);
	}

	for(int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& refPts = refPixels[i];

		Mat msk = Mat::zeros(refIm.size(), CV_8UC1);
		calMask(refPts,msk);

		Mat region;
		refIm.copyTo(region, msk);

		string sfn = dir + "/ref_region_" + type2string<int>(i) + ".png";
		imwrite(sfn, region);

		sfn = dir + "/ref_mask_" + type2string<int>(i) + ".png";
		imwrite(sfn, msk);
	}


	cout << "save regions done. look in folder build/" << dir << endl;
}


//使用homographys中每个投影矩阵的逆
void RWTransfer::backProject()
{
}

//for paper
bool RWTransfer::backProjectD()
{
	int width = srcIm.size().width;
	int height = srcIm.size().height;

	if(srcDispMat.data == nullptr || refDispMat.data == nullptr)
	{
		return false ;
	}

	back_srcPixels.resize(regionum);
	for(int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& refPts = refPixels[i];
		vector<Point2f>& srcPts = back_srcPixels[i];

		srcPts.clear();

		for(int j = 0; j < refPts.size(); ++j)
		{
			Point2f rpt = refPts[j];
			int d = refDispIm.at<uchar>(rpt.y, rpt.x);
			
			Point2f spt = Point2f(rpt.x - d, rpt.y);
			if(spt.x >= 0 && spt.x < width && spt.y >= 0 && spt.y < height)
				srcPts.push_back(spt);				
		}
	}

	for(int i = 0; i < regionum; ++i)
	{
		vector<Point2f>& srcPts = back_srcPixels[i];
		
		Mat msk = Mat::zeros(srcIm.size(), CV_8UC1);
		calMask(srcPts,msk);

		//腐蚀一次
		Mat element = getStructuringElement(MORPH_RECT, cv::Size(3, 3));
		Mat out;
		cv::erode(msk, out, element);
						
		//膨胀一次
		element = getStructuringElement(MORPH_RECT, cv::Size(3,3));
		cv::dilate(out, msk, element);
		cv::dilate(msk, out, element);
		msk = out.clone();

		//mask转换为区域内点
		vector<Point2f> newsrcPts;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (msk.at<uchar>(y,x) == 255 /*&& refVisIm.at<uchar>(y,x)== 0*/)
				{
					newsrcPts.push_back(Point2f(x,y));
				}
			}
		}

		srcPts.clear();
		copy(newsrcPts.begin(), newsrcPts.end(), back_inserter(srcPts));
	}
	cout << "back projection done." << endl;

	//back_srcSegments -> back_srcLabels
	pixels2labels(back_srcPixels, back_srcLabels, srcregionum);
	cout << "back projected source pixels -> labels done." << endl;
	return true;
}

void RWTransfer::calBackSrcSegmentsIm(const string sfn)
{
	meanshiftFilter(srcIm, regionum, back_srcPixels, back_srcSegmentsIm);
	if(sfn != "")
	{
		imwrite(sfn, back_srcSegmentsIm);
		cout << "save " << sfn << endl;
	}
}

void RWTransfer::calCorrespondenceError(const string sfn)
{
	//根据srcLabels,  back_srcLabels得到correspondence结果
	//相同标签为蓝色，不同标签为红色

	Mat errorIm(srcIm.size(), CV_8UC3, Scalar(205,0,0));
	int width = errorIm.size().width;
	int height = errorIm.size().height;
	
	for(int y = 0; y < height; ++y)
		for(int x = 0; x < width; ++x)
		{
			int index  = y * width + x;
			if(srcVisIm.at<uchar>(y,x) == 255)
			{
				errorIm.at<Vec3b>(y,x) = Vec3b(0,0,0);
				continue;
			}

				
			if(srcLabels[index] != back_srcLabels[index] || back_srcLabels[index] == -1 )
			{
				errorIm.at<Vec3b>(y,x) = Vec3b(0,0,205);				
			}
		}

	/*imshow("correspondence", errorIm);
	waitKey(0);
	destroyWindow("correspondence");*/

	if(sfn != "")
	{
		cout << "save " << sfn << endl;
		imwrite(sfn, errorIm);
	}
	
}

void RWTransfer::saveBackSrcLabels(const string sfn)
{
	cout << "save " << sfn << endl;
	int width = srcIm.size().width;
	int height = srcIm.size().height;
	saveLabels(sfn, width, height, back_srcLabels);

}

void RWTransfer::getrefRAList(vector<vector<bool>>& RAList)
{
	RAList = refRAlist;
}

//计数，超过50个像素相邻, 那就是邻接区域（分割块儿较大的，不需要这么大）
void RWTransfer::calRAList(vector<vector<bool>>& ralist, Mat Im, int regionum, vector<vector<Point2f>> Pixels, string str, vector<int> Labels)
{
	int width = Im.size().width;
	int height = Im.size().height;

	//3*3邻域
	int dx[8] = {-1,0,1,-1,1,-1,0,1};
	int dy[8] = {-1,-1,-1,0,0,1,1,1};

	ralist.resize(regionum);
	for (int i = 0; i < regionum; ++i)
	{
		ralist[i].resize(regionum, 0);
		ralist[i][i] = true;
	}

	vector<vector<int>> ralistCnt(regionum);
	for(int i = 0; i < regionum; ++i)
	{
		ralistCnt[i].resize(regionum, 0);
	}

	for (int i = 0; i < regionum; ++ i)
	{

		vector<Point2f>& ppts = Pixels[i];
		
		int idx = i;
		vector<bool> isvisited(width * height, false);
		for(int j = 0; j < ppts.size(); ++j)
		{	
			int x  = ppts[j].x;
			int y  = ppts[j].y;
			
			for (int k = 0; k < 8; ++k)
			{
				int deltax = dx[k];
				int deltay = dy[k];
				int curx = x + deltax;
				int cury = y + deltay;

				if (curx < 0 || curx >= width || cury < 0 || cury >= height )
					continue;

				int pidx = cury * width + curx;    //像素位置
				int newidx = Labels[pidx];      //分割标签

				if(newidx == idx)
					continue;

				if(isvisited[pidx] == true)
					continue;

				ralistCnt[i][newidx] ++;//newidx变成-1了
				isvisited[pidx] = true;
			}
		}
	}

	for(int i = 0; i < regionum; ++i)
	{
		for(int j = 0; j < regionum; ++j)
		{
			if(ralistCnt[i][j] >= 1 && j != i && ralist[i][j] == false)
			{
				ralist[i][j] = true;
				ralist[j][i] = true;
			}
		}
	}

	//保存
	/*fstream fout(str+"RAlist.txt", ios::out);
	if(fout.is_open() == false)
	{
		cout << "failed to open RAList.txt" << endl;
		return ;
	}*/

	/*for(int i = 0; i < regionum; ++i)
	{
		fout << i << ':' << ' ';
		for(int j = 0; j < regionum ; ++j)
		{
			if(ralist[i][j] == true && j != i)
				fout << j << ' ' ;
		}
		fout << endl;
	}
	fout.close();*/

}


void RWTransfer::calmultilevelRA(vector<vector<bool>> ralist, vector<vector<bool>>& multiralist, int multinum)
{
	int sz = ralist.size();//一共有sz个区域
	multiralist = ralist;
	if(multinum == 2)
	{
	    for(int i = 0; i < sz; ++i)
	    {
		    for(int j = 0; j < sz; ++j)
		    {
			    if(ralist[i][j] == true)//i区域与j区域相邻
			    {
				    for(int k = 0; k < sz; ++k)//j区域与k区域相邻
				    {
					    if(ralist[j][k] == true)
					    {
						    multiralist[i][k] = true;//则判定i区域与k区域相邻
						    multiralist[k][i] = true;
					    }
				    }
			    }
		    }
	    }
	}
	else if(multinum == 3)
	{
		for(int i = 0; i < sz; ++i)
	    {
		    for(int j = 0; j < sz; ++j)
		    {
			    if(ralist[i][j] == true)//i区域与j区域相邻
			    {
				    for(int k = 0; k < sz; ++k)//j区域与k区域相邻
				    {
					    if(ralist[j][k] == true)
					    {
						    multiralist[i][k] = true;//则判定i区域与k区域相邻
						    multiralist[k][i] = true;
							for(int m = 0; m < sz; ++m)
							{
								if(ralist[k][m] == true)
								{
									multiralist[i][m] = true;
									multiralist[i][m] = true;
								}
							}
					    }
				    }
			    }
		    }
	    }
	}
}

void RWTransfer::calRefSegmentsIm(const string sfn)
{
	meanshiftFilter(refIm, regionum, refPixels, refSegmentsIm );
	if(sfn != "")
	{
		//imwrite(sfn, refSegmentsIm);
		cout << "save " << sfn << endl;		
	}
}

void RWTransfer::saveRefLabels(const string& sfn)
{
	cout << "save " << sfn << endl;

	int width = srcIm.size().width;
	int height = srcIm.size().height;
	saveLabels(sfn, width, height, refLabels);
}

