#include"common.h"

void calGlobalParameters(Mat srcIm, Mat refIm, Scalar& srcMeanG, Scalar& refMeanG, Scalar& srcStddevG, Scalar& refStddevG, Scalar& rsFactorG)
{
	//Global 
	meanStdDev(srcIm, srcMeanG, srcStddevG);//自动求出全局的均值和方差
	meanStdDev(refIm, refMeanG, refStddevG);
	//求比值
	rsFactorG = Scalar(refStddevG.val[0]/srcStddevG.val[0], refStddevG.val[1]/srcStddevG.val[1], refStddevG.val[2]/srcStddevG.val[2]) ;

}

void correct(const Mat& msk, const Scalar& srcMean, const Scalar& refMean, const Scalar& rsFactor, Mat& sIm, Mat srcIm)
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

			double transferR, transferG, transferB, transferS;

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

int main(int argc, char* argv[]) {

	string folder = string(argv[1]);
	string sceneFn = string(argv[2]);
	string refFn = string(argv[3]);
	string srcFn = string(argv[4]);

	string refpath = folder + "/" + sceneFn + "/" + refFn + ".png";
	string srcpath = folder + "/" + sceneFn + "/" + srcFn + ".png";
	Mat refIm = imread(refpath);
	Mat srcIm = imread(srcpath);

	Mat srcLabIm, refLabIm;
	cv::cvtColor(srcIm, srcLabIm, CV_BGR2Lab);
	cv::cvtColor(refIm, refLabIm, CV_BGR2Lab);

	Mat tmpSrcMsk = Mat(srcLabIm.size(), CV_8UC1, Scalar(255));
	Mat tmpIm1 = Mat::zeros(srcLabIm.size(), srcLabIm.type());

	Scalar srcMeanG1, refMeanG1, srcStddevG1, refStddevG1, rsFactorG1;
	Scalar srcMeanG2, refMeanG2, srcStddevG2, refStddevG2, rsFactorG2;

	calGlobalParameters(srcLabIm, refLabIm, srcMeanG1, refMeanG1, srcStddevG1, refStddevG1, rsFactorG1);
	correct(tmpSrcMsk, srcMeanG1, refMeanG1, rsFactorG1, tmpIm1, srcLabIm);
	Mat result;
	cv::cvtColor(tmpIm1, result, CV_Lab2BGR);
	imwrite(folder + "/" + sceneFn + "/" + srcFn + "_corr_Lab.png", result);

	calGlobalParameters(srcIm, refIm, srcMeanG2, refMeanG2, srcStddevG2, refStddevG2, rsFactorG2);
	Mat tmpIm2 = Mat::zeros(srcIm.size(), srcIm.type());
	correct(tmpSrcMsk, srcMeanG2, refMeanG2, rsFactorG2, tmpIm2, srcIm);
	imwrite(folder + "/" + sceneFn + "/" + srcFn + "_corr_Rgb.png", tmpIm2);
	return 0;


}