#pragma once

#include "common.h"

//#define INITIAL_MATCH 5
//#define INITIAL_REF_PT 0
//#define INITIAL_SRC_PT 1
//#define REFINE_REF_PT 3
//#define REFINE_SRC_PT 4
enum ShowFlags {INITIAL_MATCH = 0, REFINE_MATCH, OUTLIER_MATCH, 
				INITIAL_REF_PT = 0, INITIAL_SRC_PT, REFINE_REF_PT, REFINE_SRC_PT };

class MatchesRefiner
{

private:
	Mat srcIm, refIm;
	int imgwidth, imgheight;
		
	vector<KeyPoint> refKeypts;
	vector<KeyPoint> srcKeypts;
	
	//DMatch的queryIdx，trainIdx均来自于initial keypoints
	vector<DMatch> initialMatches;
	vector<DMatch> outlierMatches;
	//DMatch的queryIdx, trainIdx来自于refine keypoints
	vector<DMatch> refineMatches;    
	

	//refine result
	vector<KeyPoint> re_refKeypts;
	vector<KeyPoint> re_srcKeypts;

	//outlier points
	vector<KeyPoint> out_refKeypts;
	vector<KeyPoint> out_srcKeypts;

	Mat F;

protected:
	void readImage(const string& refFn, const string& srcFn);
	void readKeypoints(const string& matchFn);
	void removeRebundancy(vector<KeyPoint>& refpts, vector<KeyPoint>& srcpts);
	void transfer2DMatches(const vector<KeyPoint>& refpts, const vector<KeyPoint>& srcpts, vector<DMatch>& dmatches);

	void saveMatches(const vector<KeyPoint>& refpts, const vector<KeyPoint>& srcpts, const string& fn);
public:
	void Initial(const string& refFn, const string& srcFn, const string& matchFn);
	
	//使用RANSAC方法求解基础矩阵F，然后再优化matches
	void Refine();
	//移除refine matches中的冗余
	void Remove();
	//利用disparity信息对refineMatches, outlierMatches进行优化
	void Postprocess(const Mat& disp);

	//保存结果： 保存优化后的matches, 以及结果
	void showMatches(const ShowFlags Flag, const string& sfn = "");
	void showKeypoints(const ShowFlags Flag, const string& sfn = "");

	void saveInitialMatches(const string& sfn);
	void saveRefineResults(const string& sfn);
	void saveOutlierResults(const string& sfn);
	
};