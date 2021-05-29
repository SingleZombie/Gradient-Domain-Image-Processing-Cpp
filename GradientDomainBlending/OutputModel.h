#ifndef OUTPUT_MODEL_H
#define OUTPUT_MODEL_H

#include <string>
#include <vector>
#include "InputModel.h"
#include "Task.h"
#include "ImageUtil.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  

struct OutputMsg
{
	cv::Mat srcImg, dstImg;
	std::vector<cv::Mat> outputMat;
	float computingTime;
	TaskType taskType;
};

class OutputModel
{
public:
	OutputModel(const std::string& mode);
	void pushOutputParameters(const OutputMsg& outputMsg);

private:
	std::string _mode;
	int _batchId;
};

#endif