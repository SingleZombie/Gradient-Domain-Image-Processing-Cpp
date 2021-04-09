#ifndef GRADIENT_FILTER_H
#define GRADIENT_FILTER_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp> 

class GradientFilter
{
public:
	GradientFilter();
	cv::Mat calLaplacianImage(const cv::Mat& input);
private:
	static cv::Mat getLaplacianKernel();
};

#endif