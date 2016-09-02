#ifndef MS_CV_H_
#define MS_CV_H_

namespace cv {
class Mat;
}

int mean_shift_segmentation(const cv::Mat& src, cv::Mat& labels_dst, int spatial_variance, float color_variance, int minsize);

#endif