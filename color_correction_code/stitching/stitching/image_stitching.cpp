#include "common.h"

int main(int argc, char* argv[]) {

	string folder = string(argv[1]);
	string scene = string(argv[2]);
	string stdFn = string(argv[3]);
	string srcFn = string(argv[4]);

	folder = folder + "/" + scene;
	
	string stdpath = folder + "/" +stdFn + ".png";
	string srcpath = folder + "/" + srcFn + ".png";
	Mat stdIm, srcIm;

	stdIm = imread(stdpath);
	srcIm = imread(srcpath);

	int width = stdIm.size().width;
	int height = stdIm.size().height;
	
	Mat result1, result2, result3;
	Mat result(height, width, stdIm.type());

	result1 = stdIm(Range(0, 200), Range(0, width));
	result2 = srcIm(Range(200, height-200), Range(0, width));
	result3 = stdIm(Range(height-200, height), Range(0, width));

	Mat imageROI = result(Rect(0, 0, width, 200));
	result1.copyTo(imageROI);
	imageROI = result(Rect(0, 200, width, height-400));
	result2.copyTo(imageROI);
	imageROI = result(Rect(0, height-200, width, 200));
	result3.copyTo(imageROI);
	imwrite(folder + "/" + srcFn +"_stitching.png", result);
	return 0;
}