#include "common.h"
#include "basic.h"
#include "ms_cv.h"

#include "msImageProcessor.h"
#include <opencv2/core/core.hpp>

/**
 * @param src Image to segment
 * @param labels_dst	cv::Mat where the (int) labels will be written in
 * @param segment_dst	cv::Mat the segmentation results
 * @param density_dst	cv::Mat number of pixels in each region
 * @return Number of different labels
 */

int mean_shift_segmentation(const cv::Mat& src, cv::Mat& labels_dst, cv::Mat& segment_dst, cv::Mat& density_dst, int spatial_variance, float color_variance, int minsize)
{
	msImageProcessor proc;
	proc.DefineImage(src.data, (src.channels() == 3 ? COLOR : GRAYSCALE), src.rows, src.cols);//定义图像数据 彩色或灰度
	proc.Segment(spatial_variance,color_variance, minsize, MED_SPEEDUP);//HIGH_SPEEDUP, MED_SPEEDUP, NO_SPEEDUP; high: set speedupThreshold, otherwise the algorithm uses it uninitialized!

	int regions_count = proc.GetRegionsCnt();//获取区域数量

	//标签结果
	labels_dst = cv::Mat(src.size(), CV_32SC1);
	proc.GetRegionsLabels(labels_dst.data);

	//每个区域像素数目
	density_dst = cv::Mat(regions_count, 1, CV_32SC1);
	proc.GetRegionsDensitys(density_dst.data);

	//meanshift分割图像
	segment_dst = cv::Mat(src.size(), CV_8UC3);
	proc.GetResults(segment_dst.data);	

	//test
	/*imshow("segmentation", segment_dst);
	waitKey(0);
	destroyWindow("segmentation");*/

	return regions_count;
} 

void DrawContours(Mat segments, Mat& contours)
{
	// Pixel offsets around the centre pixels starting from left, going clockwise
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	int width = segments.size().width;    // w * h
	int height = segments.size().height;
	int ch = segments.channels();

	int sz = width * height;
	vector<bool> istaken(sz, false);
	vector<uchar> segmentsvec(0);
	Mat2PixelsVector(segments, segmentsvec);

	int mainindex  = 0;
	for( int j = 0; j < height; j++ )
	{
		for( int k = 0; k < width; k++ )
		{
			int np = 0;
			for( int i = 0; i < 8; i++ )
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if( (x >= 0 && x < width) && (y >= 0 && y < height) )
				{
					int index = y*width + x;
					if( false == istaken[index] )
					{
						if( (int)segmentsvec[mainindex * ch] != (int)segmentsvec[index * ch] ) np++;
					}
				}
			}
			if( np > 2 )//1 for thicker lines and 2 for thinner lines
			{
				segmentsvec[(j*width + k) * ch] = 255;
				segmentsvec[(j*width + k) * ch + 1] = 255;
				segmentsvec[(j*width + k) * ch + 2] = 255;
				istaken[mainindex] = true;
			}
			mainindex++;
		}
	}
	
	PixelsVector2Mat(segmentsvec, width, height, ch, contours);

}

int main(int argc, char * argv[])
{
	if (argc != 7)
	{
		cout << "Usage: meanshift-cv.exe folder image-name sptial color minR" << endl;
		cout << "example: meanshift-cv.exe Art view1 7 5 100" << endl;

		getchar();
		exit(-1);
	}
	
	string folder = string(argv[1]);
	string sceneFn = string(argv[2]);
	string imname = string(argv[3]);
	string rs = string(argv[4]);
	string rc = string(argv[5]);
	string minR = string(argv[6]);
	folder = folder + "/" + sceneFn;

	string fn  =  folder + "/" + imname + ".png";
	Mat src = imread(fn);
	if (src.data == nullptr)
	{

		cout << "failed to open " << fn << endl;
		return 1;
	}
	cout << "load input image done." << endl;

	//Meanshift参数
	int spatial_variance = string2type<int>(rs);
	int color_variance = string2type<int>(rc);
	int minsize = string2type<int>(minR);

	cout << spatial_variance << ' ' << color_variance << ' ' << minsize << endl;
	
	Mat labels;
	Mat segments;
	Mat densitys;

	int regioncnt = mean_shift_segmentation(src, labels, segments, densitys, spatial_variance, color_variance, minsize);
	cout << "mean-shift done:  " << regioncnt << " regions." << endl;

	Mat contours;
	DrawContours(segments, contours);

	string savefn;

	//save labels
	savefn = folder + "/" + imname + "_labels_" + rs + "_" + rc + "_" + minR + ".txt";
	cout << "save " << savefn << endl;
	saveMat(labels, savefn);

	/*savefn = folder + "/densitys_" + imname + ".txt";
	saveMat(densitys, savefn);*/

	//save segments
	savefn = folder + "/" + imname + "_seg_" + rs + "_" + rc + "_" + minR + ".png";
	cout << "save " << savefn << endl;
	imwrite(savefn, segments);
	savefn = folder + "/" + imname + "_con_" + rs + "_" + rc + "_" + minR + ".png";
	imwrite(savefn, contours);

	return 1;
}