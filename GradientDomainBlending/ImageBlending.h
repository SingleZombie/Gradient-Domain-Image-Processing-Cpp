#ifndef IMAGE_BLENDING_H
#define IMAGE_BLENDING_H

#include "Task.h"
#include "InputModel.h"
#include "OutputModel.h"
#include "ImageUtil.h"
#include "GraidentFilter.h"
#include "PoissonSolver.h"
#include "QuadTree.h"
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <memory>

class BlendingMethodBase
{
public:
	virtual void init(
		// Input
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& regoin,
		int dstLeft, int dstTop,
		TaskType taskType
	) = 0;
	virtual void imageBlending(
		// Input
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& regoin,
		int dstLeft, int dstTop,
		TaskType taskType,
		// Output
		cv::Mat& resultImg,
		int& computingTime
	) = 0;
	virtual void addOutputInfo(
		std::vector<cv::Mat>& outputMat
	) = 0;
};

class BasicBlending : public BlendingMethodBase
{
public:
	BasicBlending();
	void init(
		// Input
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& regoin,
		int dstLeft, int dstTop,
		TaskType taskType
		) {}
	void imageBlending(
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& region,
		int dstLeft, int dstTop,
		TaskType taskType,
		cv::Mat& resultImg,
		int& computingTime
	);
	void addOutputInfo(
		std::vector<cv::Mat>& outputMat
	) {};
private:
	GradientFilter _gradientFilter;
};

class AdvancedBlending : public BlendingMethodBase
{
public:
	AdvancedBlending();
	void init(
		// Input
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& regoin,
		int dstLeft, int dstTop,
		TaskType taskType
	);
	void imageBlending(
		const cv::Mat& srcImg,
		const cv::Mat& dstImg,
		const ImageRegion& region,
		int dstLeft, int dstTop,
		TaskType taskType,
		cv::Mat& resultImg,
		int& computingTime
	);
	void addOutputInfo(
		std::vector<cv::Mat>& outputMat
	);
private:
	std::shared_ptr<QuadTreeRoot> _root;
};

#endif