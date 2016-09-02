#include "common.h"

//这个是针对三通道的，所以不能单通道用
//reshape() matlab 列优先， opencv行优先，所以opencv若要和matlab一样，需要先转置再reshape

int main(int argc, char* argv[]) {
	string folder = string(argv[1]);
	string scene = string(argv[2]);
	string refFn = string(argv[3]);
	string srcFn = string(argv[4]);

	string srcpath = folder + "/" + scene + "/" + srcFn + ".png";
	string refpath = folder + "/" + scene + "/" + refFn + ".png";

	Mat srcIm, refIm, srcImn, refImn;
	Mat srcIm_norm, refIm_norm;
	vector<Mat> srcch, refch;

	srcIm = imread(srcpath, CV_LOAD_IMAGE_UNCHANGED);
	refIm = imread(refpath, CV_LOAD_IMAGE_UNCHANGED);

	int height = srcIm.size().height;
	int width = srcIm.size().width;

	// 1/255
	srcIm.convertTo(srcIm_norm, CV_32FC3, 1.0);
	refIm.convertTo(refIm_norm, CV_32FC3, 1.0);

	srcIm_norm = srcIm_norm.reshape(0, 1);
	refIm_norm = refIm_norm.reshape(0, 1);

	//cov & mean
	Mat srcmat, refmat;
	split(srcIm_norm, srcch);
	split(refIm_norm, refch);

	for(int k = 0; k < 3; ++k) {
		srcImn = srcch.at(k);
		refImn = refch.at(k);
		srcmat.push_back(srcImn);
		refmat.push_back(refImn);
	}	

	Mat srccov, srcmean, refcov, refmean;
	calcCovarMatrix(srcmat, srccov, srcmean, cv::COVAR_NORMAL|CV_COVAR_COLS);
	srccov.convertTo(srccov, CV_32F, 1.0/(height*width*1.0-1));
	calcCovarMatrix(refmat, refcov, refmean, cv::COVAR_NORMAL|CV_COVAR_COLS);
	refcov.convertTo(refcov, CV_32F, 1.0/(height*width*1.0-1));
	srcmean.convertTo(srcmean, CV_32F, 1.0);
	refmean.convertTo(refmean, CV_32F, 1.0);

	Mat srcU, srcStmp, srcV, refU, refStmp, refV;
	SVD::compute(srccov, srcStmp, srcU, srcV);
	SVD::compute(refcov, refStmp, refU, refV);
	
	Mat srcS = Mat::eye(3, 3, CV_32F);
	Mat refS = Mat::eye(3, 3, CV_32F);
	for(int i = 0; i < 3; ++i) {
		srcS.at<float>(i, i) = srcStmp.at<float>(i, 0);
		refS.at<float>(i, i) = refStmp.at<float>(i, 0);
	}

	Mat addone = Mat::ones(1, height*width, CV_32F);
	Mat srcImhs = srcmat.clone();
	srcImhs.push_back(addone);

	Mat T_s = Mat::eye(4, 4, CV_32F);
	Mat T_t = Mat::eye(4, 4, CV_32F);

	for(int i = 0; i < 3; ++i) {
		T_s.at<float>(i, 3) = -srcmean.at<float>(i, 0);
		T_t.at<float>(i, 3) = refmean.at<float>(i, 0);
	}

	Mat R_s = Mat::eye(4, 4, CV_32F);
	Mat R_t = Mat::eye(4, 4, CV_32F);

	srcU = srcU.t();
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			R_s.at<float>(i, j) = srcU.at<float>(i, j);
			R_t.at<float>(i, j) = refU.at<float>(i, j);
		}
	}
	
	Mat S_s = Mat::eye(4, 4, CV_32F);
	Mat S_t = Mat::eye(4, 4, CV_32F);
	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			if(srcS.at<float>(i, j) != 0) {
				S_s.at<float>(i, j) = 1.0 / ( sqrt(srcS.at<float>(i, j))*1.0 );
			}			
			S_t.at<float>(i, j) = sqrt(refS.at<float>(i, j));
		}
	}
	Mat tmp = T_t*R_t*S_t*S_s*R_s*T_s;
	Mat srcImhe = T_t*R_t*S_t*S_s*R_s*T_s*srcImhs;
	for(int i=0;i<srcImhe.rows;++i)
	{
		divide(srcImhe.row(i),srcImhe.row(3),srcImhe.row(i));
	}
	/*FileStorage fs(folder +"/test.xml", FileStorage::WRITE);
	fs << "refmat" << refmat;
	fs.release();*/


	Mat src_corr(height, width, CV_8UC3);
	vector<Mat> srcrch;
	for(int i = 0; i < 3; ++i) {
		Mat srctmp = srcImhe.row(i);
		srctmp = srctmp.reshape(0, height);
		srctmp.convertTo(srctmp, CV_8UC1, 1.0);
		srcrch.push_back(srctmp);
	}
	merge(srcrch,src_corr); 
	string resultpath = folder + "/" + scene + "/" + srcFn + "_corr.png";
	imwrite(resultpath, src_corr);
	return 0;

	
}