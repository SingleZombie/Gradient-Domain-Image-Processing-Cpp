#include "OutputModel.h"
#include <iostream>

OutputModel::OutputModel(const std::string& mode) :
	_mode(mode),
	_batchId(0)
{
	if (mode != "s" && mode != "c")
	{
		throw "The mode parameter is wrong";
	}
}

void OutputModel::pushOutputParameters(const OutputMsg& outputMsg)
{
	
	if (_mode == "s")
	{
		cv::imshow("src", outputMsg.srcImg);
		cv::imshow("dst", outputMsg.dstImg);
		for (int i = 0; i < outputMsg.outputMat.size(); i++)
		{
			cv::imshow(std::to_string(i), outputMsg.outputMat[i]);
		}
		cv::waitKey();
		cv::destroyAllWindows();
	}
	else if (_mode == "c")
	{
		std::cout << "task" << _batchId << ": " << outputMsg.computingTime << "ms" << std::endl;
		for (int i = 0; i < outputMsg.outputMat.size(); i++)
		{
			std::string fn = std::to_string(_batchId) + "_" + std::to_string(i) + ".jpg";
			cv::imwrite(fn, 
				imgFloatToUchar(outputMsg.outputMat[i]));
		}
	}
	_batchId++;
}