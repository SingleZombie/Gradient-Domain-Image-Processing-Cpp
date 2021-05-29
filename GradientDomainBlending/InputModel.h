#ifndef INPUT_MODEL_H
#define INPUT_MODEL_H

#include <string>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include "Task.h"

struct InputMsg
{
	cv::Mat srcImg;
	cv::Mat dstImg;
	cv::Mat maskImg;
	TaskType taskType;
	RegionType regionType;
	int rectLeft, rectTop, rectWidth, rectHeight;
	int dstLeft, dstTop;
};

class InputModel
{
public:
	InputModel(const std::string& cfgFilename);
	bool hasNext() const;
	InputMsg getNextInput();
private:
	std::ifstream _fileStream;
};

#endif