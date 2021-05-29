#ifndef IMAGE_BLENDING_H
#define IMAGE_BLENDING_H

#include "Task.h"
#include "InputModel.h"
#include "OutputModel.h"
#include "ImageUtil.h"
#include "GraidentFilter.h"
#include "PoissonSolver.h"
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

class BlendingMethodBase
{
public:
	virtual void imageBlending(
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& regoin,
		int dstLeft, int dstTop,
		TaskType taskType,
		cv::Mat& resultImg,
		int& computingTime
	) = 0;
};

class BasicBlending : public BlendingMethodBase
{
public:
	BasicBlending();
	void imageBlending(
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& region,
		int dstLeft, int dstTop,
		TaskType taskType,
		cv::Mat& resultImg,
		int& computingTime
	);
private:
	GradientFilter _gradientFilter;
};

#endif