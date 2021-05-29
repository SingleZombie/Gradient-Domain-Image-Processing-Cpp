#include "InputModel.h"
#include "ImageUtil.h"

InputModel::InputModel(const std::string& cfgFilename)
{
	_fileStream = std::ifstream(cfgFilename, std::ios::in);
}

bool InputModel::hasNext() const
{
	return !_fileStream.eof();
}

InputMsg InputModel::getNextInput()
{
	std::string typeStr, regionTypeStr;
	_fileStream >> typeStr >> regionTypeStr;
	TaskType taskType = strToTaskType[typeStr];
	RegionType regionType = strToRegionType[regionTypeStr];

	InputMsg inputMsg;
	inputMsg.taskType = taskType;
	inputMsg.regionType = regionType;

	// TODO: if the files do not exist, throw exception
	if (taskType == TaskType::Tiling)
	{
		std::string filename;
		_fileStream >> filename;
		cv::Mat mat = imgUcharToFloat(cv::imread(filename));

		inputMsg.srcImg = inputMsg.dstImg = mat;
	}
	else
	{
		std::string srcFn, dstFn;
		_fileStream >> srcFn >> dstFn;

		inputMsg.srcImg = imgUcharToFloat(cv::imread(srcFn));
		inputMsg.dstImg = imgUcharToFloat(cv::imread(dstFn));

		if (regionType == RegionType::Rect)
		{
			_fileStream >> inputMsg.rectLeft >> inputMsg.rectTop >> inputMsg.rectWidth >> inputMsg.rectHeight;
			_fileStream >> inputMsg.dstLeft >> inputMsg.dstTop;
		}
		else
		{
			std::string maskFn;
			_fileStream >> maskFn;
			inputMsg.maskImg = imgUcharToFloat(cv::imread(maskFn));
			_fileStream >> inputMsg.dstLeft >> inputMsg.dstTop;
		}
	}

	return inputMsg;
}