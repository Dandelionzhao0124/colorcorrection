#include "common.h"

int main(int argc, char* argv[]) {
	string folder = string(argv[1]);
	string scene = string(argv[2]);
	string refFn = string(argv[3]);
	string srcFn = string(argv[4]);

	/*string refdisprPath = folder + "/" + scene + "/disp1.png";
	string srcdisprPath = folder + "/" + scene + "/disp5.png";*/
	/*string refdisprPath = folder + "/" + scene + "/disp_" + refFn + ".png";
	string srcdisprPath = folder + "/" + scene + "/disp_" + srcFn + ".png";*/

	string refdisprPath = folder + "/" + scene + "/disp_" + refFn + ".png";
	string srcdisprPath = folder + "/" + scene + "/disp_" + srcFn + ".png";

	string refdispwPath = folder + "/" + scene + "/disp_" + refFn + ".png";
	string srcdispwPath = folder + "/" + scene + "/disp_" + srcFn + ".png";

	string refpath = folder + "/" + scene + "/" + refFn + ".png";
	string srcpath = folder + "/" + scene + "/" + srcFn + ".png";

	Mat refIm, srcIm, disprefIm, dispsrcIm;

	refIm = imread(refpath);
	srcIm = imread(srcpath);
	disprefIm = imread(refdisprPath, CV_BGR2GRAY);
	dispsrcIm = imread(srcdisprPath, CV_BGR2GRAY);

	Mat refVisIm(disprefIm.size(), disprefIm.type());
	Mat srcVisIm(dispsrcIm.size(), dispsrcIm.type());

	int height = srcIm.size().height;
	int width = srcIm.size().width;

	int thresh = 2;
	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			int delta = disprefIm.at<uchar>(y, x);
			int xc = x - delta;
			if(xc < 0) {
				refVisIm.at<uchar>(y, x) = 255;
				continue;
			}
			int dispc = dispsrcIm.at<uchar>(y, xc);
			if(abs(delta - dispc) >= thresh) {
				refVisIm.at<uchar>(y, x) = 255;
			}
			else if(abs(delta - dispc) < thresh) {
				refVisIm.at<uchar>(y, x) = 0;
			}
		}
	}

	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			int delta = dispsrcIm.at<uchar>(y, x);
			int xc = x + delta;
			if(xc >= width) {
				srcVisIm.at<uchar>(y, x) = 255;
				continue;
			}
			int dispc = disprefIm.at<uchar>(y, xc);
			if(abs(delta - dispc) >= thresh) {
				srcVisIm.at<uchar>(y, x) = 255;
			}
			else if(abs(delta - dispc) < thresh) {
				srcVisIm.at<uchar>(y, x) = 0;
			}
		}
	}
	imwrite(refdispwPath, disprefIm);
	imwrite(srcdispwPath, dispsrcIm);
	imwrite(folder + "/" + scene + "/visibility_" + refFn + ".png", refVisIm);
	imwrite(folder + "/" + scene + "/visibility_" + srcFn + ".png", srcVisIm);
	return 0;
}