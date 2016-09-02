#include"common.h"

int main(int argc, char* argv[]) {

	string folder = string(argv[1]);
	string sceneFn = string(argv[2]);
	string gtFn = string(argv[3]);
	string baseFn = string(argv[4]);
	string methodFn = string(argv[5]);
	string csFn = string(argv[6]);

	string gtpath = folder + "/" + sceneFn + "/" + gtFn + ".png";
	string basepath = folder + "/" + sceneFn + "/" + baseFn + ".png";
	string methodpath = folder + "/" + sceneFn + "/" + methodFn + ".png";
	string cspath = folder + "/" + sceneFn + "/" + csFn;
	string vispath = folder + "/" + sceneFn + "/visibility_" + baseFn + ".png";

	Mat gtIm = imread(gtpath);
	Mat baseIm = imread(basepath);
	Mat methodIm = imread(methodpath);
	//Mat visIm = imread(vispath, CV_LOAD_IMAGE_GRAYSCALE);

	int width = gtIm.size().width;
	int height = gtIm.size().height;

	float csbase = 0.0, csmethod = 0.0;
	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
		/*	if(visIm.at<uchar>(y, x) == 255) {
				continue;
			}*/
			float csbasev = 0;
			csbasev += (gtIm.at<Vec3b>(y, x)[0] - baseIm.at<Vec3b>(y, x)[0]) * (gtIm.at<Vec3b>(y, x)[0] - baseIm.at<Vec3b>(y, x)[0]);
			csbasev += (gtIm.at<Vec3b>(y, x)[1] - baseIm.at<Vec3b>(y, x)[1]) * (gtIm.at<Vec3b>(y, x)[1] - baseIm.at<Vec3b>(y, x)[1]);
			csbasev += (gtIm.at<Vec3b>(y, x)[2] - baseIm.at<Vec3b>(y, x)[2]) * (gtIm.at<Vec3b>(y, x)[2] - baseIm.at<Vec3b>(y, x)[2]);
            csbase += sqrt(csbasev);

			float csmethodv = 0;
			csmethodv += (gtIm.at<Vec3b>(y, x)[0] - methodIm.at<Vec3b>(y, x)[0]) * (gtIm.at<Vec3b>(y, x)[0] - methodIm.at<Vec3b>(y, x)[0]);
			csmethodv += (gtIm.at<Vec3b>(y, x)[1] - methodIm.at<Vec3b>(y, x)[1]) * (gtIm.at<Vec3b>(y, x)[1] - methodIm.at<Vec3b>(y, x)[1]);
			csmethodv += (gtIm.at<Vec3b>(y, x)[2] - methodIm.at<Vec3b>(y, x)[2]) * (gtIm.at<Vec3b>(y, x)[2] - methodIm.at<Vec3b>(y, x)[2]);
			csmethod += sqrt(csmethodv);
		}
	}

	float ccmethod;
	ccmethod = (( (csbase*1.0) - (csmethod*1.0) ) / (csbase*1.0))*100;
	cout << methodFn << " " << ccmethod <<endl;
	fstream fout(cspath.c_str(), ios_base::app);
	if (fout.is_open() == false)
	{
		cout << "failed to open " << cspath << endl;
		return -1;
	}
	fout << methodFn << " " << ccmethod <<endl;
	fout.close();

	return 0;

}