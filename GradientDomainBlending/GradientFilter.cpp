#include "GraidentFilter.h"
GradientFilter::GradientFilter()
{

}

cv::Mat GradientFilter::calLaplacianImage(const cv::Mat& input)
{
	cv::Mat dstImg;
	cv::filter2D(input, dstImg, input.depth(), getLaplacianKernel());
	return dstImg;
}

cv::Mat GradientFilter::getLaplacianKernel()
{
	static cv::Mat res = (cv::Mat_<char>(3, 3) <<
		0, -1, 0,
		-1, 4, -1,
		0, -1, 0);

	return res;
}