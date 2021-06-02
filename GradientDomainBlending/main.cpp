#include "main.h"
#include <iostream>
#include <fstream>

void tripleCharToFloat(cv::Mat& mat)
{
	mat.convertTo(mat, CV_32F, 1 / 255.0);
}

void floatToTripleChar(cv::Mat& mat)
{
	mat.convertTo(mat, CV_8U, 255.0f);
}

const std::string defaultOutputMode = "s";
const std::string defaultCfgFilename = ".\\TestData.txt";
const std::string defaultMethod = "d";

int main(int argc, char* argv[])
{
	std::string outputMode, cfgFilename, methodName;
	if (argc < 2)
	{
		outputMode = defaultOutputMode;
	}
	else
	{
		outputMode = argv[1];
	}
	if (argc < 3)
	{
		cfgFilename = defaultCfgFilename;
	}
	else
	{
		cfgFilename = argv[2];
	}
	if (argc < 4)
	{
		methodName = defaultMethod;
	}
	else
	{
		methodName = argv[3];
	}

	InputModel inputModel(cfgFilename);
	OutputModel outputModel(outputMode);

	while (inputModel.hasNext())
	{
		auto inputMsg = inputModel.getNextInput();

		auto srcMat = inputMsg.srcImg;
		auto dstMat = inputMsg.dstImg;

		std::shared_ptr<ImageRegion> pRegion;

		if (inputMsg.regionType == RegionType::Rect)
		{
			cv::Rect rect(inputMsg.rectLeft, inputMsg.rectTop, inputMsg.rectWidth, inputMsg.rectHeight);
			pRegion = std::make_shared<ImageRegion>(rect);
		}
		else
		{
			pRegion = std::make_shared<ImageRegion>(inputMsg.maskImg);
		}

		const ImageRegion& region = *pRegion;
		//printf("region size = %d\n", region.size());

		int dstLeft = inputMsg.dstLeft, dstTop = inputMsg.dstTop;

		std::vector<cv::Mat> srcChannels;
		std::vector<cv::Mat> dstChannels;
		cv::split(srcMat, srcChannels);
		cv::split(dstMat, dstChannels);
		std::vector<cv::Mat> outputChannels;

		GradientFilter gradientFilter;
		int totalTimeInMs = 0;
		
		// *******************************************************************************
		// Change blending method here!
		// *******************************************************************************
		std::shared_ptr<BlendingMethodBase> blendingMethod;
		if (methodName == "d")
		{
			blendingMethod = std::make_shared<BasicBlending>();
		}
		else
		{
			blendingMethod = std::make_shared<AdvancedBlending>();
		}

		blendingMethod->init(srcMat, dstMat, region, dstLeft, dstTop, inputMsg.taskType);

		for (int i = 0; i < srcChannels.size(); i++)
		{
			const auto& srcImg = srcChannels[i];
			const auto& dstImg = dstChannels[i];
			
			int computingTime = 0;
			cv::Mat outputMat;

			blendingMethod->imageBlending(srcImg, dstImg, region, dstLeft, dstTop, inputMsg.taskType, outputMat, computingTime);
			
			outputChannels.push_back(outputMat);
		}
		cv::Mat resImg;
		cv::merge(outputChannels, resImg);

		OutputMsg outputMsg;
		outputMsg.computingTime = totalTimeInMs;
		outputMsg.srcImg = srcMat;
		outputMsg.dstImg = dstMat;

		// Direct copy
		cv::Mat mat3;
		dstMat.copyTo(mat3);
		region.imageCopyC3(srcMat, mat3, dstLeft, dstTop);
		outputMsg.outputMat.push_back(mat3);

		// Image Blending
		outputMsg.outputMat.push_back(resImg);

		blendingMethod->addOutputInfo(outputMsg.outputMat);

		outputModel.pushOutputParameters(outputMsg);
	}

	return 0;
}