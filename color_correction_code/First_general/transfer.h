#pragma once

#include "basic.h"

class CMatch
{
public:
	cv::Point2f srcPt, refPt; 

	CMatch(Point2f rpt, Point2f spt)
	{
		srcPt = spt;
		refPt = rpt;
	}

};

class RWTransfer
{
friend class CCorrecter;

private:
	string sceneFn;

	int invalidrefregion;
	int level;//表示level阶邻域寻找相似颜色

	Mat srcRAWIm, refRAWIm;		//以CV_LOAD_IMAGE_UNCHANGED读入数据
	Mat srcIm, refIm;			//BGR三通道图像
	Mat srcVisIm, refVisIm;		//可见性图像
	Mat srcDispIm, refDispIm;   //视差图像
	Mat srcDispMat, refDispMat; //视差值
	
	int srcregionum;
	int refregionum;
	int regionum;
	
	Mat srcSegmentsIm;
	vector<int> srcLabels;                 //分割标签
	vector<vector<Point2f>> srcPixels;     //分割区域内像素
	vector<Point2f> srcCenters;            //src分割区域的中心像素
	vector<Point2f> refCenters;            //ref分割区域中心像素
	vector<CMatch> matches;                //全局匹配点（更正确）
	vector<CMatch> matchesmore;            //分割匹配点（更多）

	vector<vector<int>> src2refmap;

	//需要求解
	vector<Mat> homographys;
	vector<int> isMatched;     //0,1,2  

	vector<int> refLabels;
	vector<vector<Point2f>> refPixels;	
	Mat refSegmentsIm;

	vector<vector<Point2f>> back_srcPixels;
	vector<int> back_srcLabels;
	Mat back_srcSegmentsIm;

	vector<int> labelmap;//存储最终src和ref的对应标签

	vector<Scalar> srcregioncolor;//src和ref的每个区域的平均颜色
	vector<Scalar> refregioncolor;

	vector<vector<bool>> srcRAlist;//图像内部的区域邻接表
	vector<vector<bool>> refRAlist;
	vector<vector<bool>> multisrcRAlist;//多阶邻域
	vector<vector<bool>> multirefRAlist;

	vector<int> srcregionflag;//src图像有哪些区域未被匹配，有匹配点区域赋1，没有匹配点区域赋-1，区域被合并赋-2
	vector<int> refregionflag;
protected:
	//void labels2pixels(const vector<int>& labels, const int& regionum, vector<vector<Point2f>>& pixels);
	void labels2pixels(const vector<int>& labels, const int& regionum, vector<vector<Point2f>>& pixels);
	void pixels2labels(const vector<vector<Point2f>>& pixels, vector<int>& labels, int regionum);
	
	void calDispMat();

public:

	void initial(const string& folder,  const string& sceneFn,  const string& refFn, const string& srcFn);
	void readMatfromfile(string ImFn, Mat& RAWIm, int rows, int cols, int channels);
	void calRColors(vector<Scalar>& regioncolor, int regionum, vector<vector<Point2f>> Pixels, Mat Im);
	void dealRefNopoint(const string folder, const string refFn);
	void dealSrcNopoint(const string folder, const string refFn, vector<int> regionflag);

	void Recombine(vector<vector<Point2f>>& Pixels, vector<int>& labels, Mat Im, int& rnum);
	void getrefRAList(vector<vector<bool>>& RAList);

	void FirstSrcCombine(const string folder, const string srcFn);
	void FuseSrcNopoint(const string folder, const string srcFn);
	void FuseRefNopoint(const string folder, const string refFn);
	void Matches(const string folder, const string srcFn, const string refFn);
	void matchregions(const string folder, const string srcFn, const string refFn);

	void FuseRegion(vector<int>& regionflag, vector<vector<Point2f>>& Pixels, int regionum, vector<vector<bool>> RAlist, 
	vector<Point2f> Centers, vector<Scalar> regioncolor, vector<int>& Labels);

	int InvalidRegion(vector<int> regionflag);
	void calSegmentsIm(const string sfn, Mat Im, int regionum, vector<vector<Point2f>> Pixels, Mat SegmentsIm, vector<int> regionflag);

	void showSameColor(string sfn, string flag);

	void calRCenters(const string& fn, vector<Point2f>& Centers, int regionum, vector<vector<Point2f>> Pixels, Mat SegmentsIm);
	
	void estimateHomography();
	void project();
	void dealUnMatchedRegion(const int srcLabel, const vector<Point2f>& srcPts, vector<Point2f>& refPts);

	void calRefSegmentsIm(const string sfn = "");
	void saveRefLabels(const string& sfn);

	// saving regions in src and ref
	void saveRegions();
		
	// calculate regions adajcent list
	void calRAList(vector<vector<bool>>& ralist, Mat Im, int regionum, vector<vector<Point2f>> Pixels, string str, vector<int> Labels);

	void calmultilevelRA(vector<vector<bool>> ralist, vector<vector<bool>>& multiralist, int multinum);

	void backProject();
	void calBackSrcSegmentsIm(const string sfn = "");
	void calCorrespondenceError(const string sfn = "");
	void saveBackSrcLabels(const string sfn = "");

	//using disparity information
	void projectD();
	bool backProjectD();	

	void FuseRef(const string folder, const string refFn);
	void SaveRegionFlag(string sfn, Mat& mark, Mat Im, vector<vector<Point2f>> Pixels, vector<int> regionflag);
};