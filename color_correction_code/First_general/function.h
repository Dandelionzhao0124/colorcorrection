#pragma once

#include "basic.h"
#include "transfer.h"


void calMask(const vector<Point2f>& pts, Mat& msk);


void readImage(const string& fn, Mat& im, int METHOD );
void readLabels(const string& fn, int& regionum, vector<int>& labels);
void readMatches(const string& fn, vector<CMatch>& matches, Mat refVisIm, Mat srcVisIm);

void saveLabels(const string& fn, const int& width, const int& height, const vector<int>& labels);

void meanshiftFilter(const Mat& im, const int& regionum, const vector<vector<cv::Point2f>>& segments, Mat& filteredIm);
void Filter(const Mat& im, const int& regionum, const vector<vector<cv::Point2f>>& segments, Mat& filteredIm, vector<int> regionflag);
void GetColorForRegion(const Mat& im, vector<vector<Point2f>> srcPixels, vector<vector<Point2f>> refPixels, vector<vector<int>> src2refmatch, Mat& markref, Mat& marksrc);
Mat mergeCols(Mat A, Mat B);