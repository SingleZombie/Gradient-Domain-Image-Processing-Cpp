//#include "main.h"
//#include <iostream>
//#include <fstream>
//
//void tripleCharToFloat(cv::Mat& mat)
//{
//	mat.convertTo(mat, CV_32F, 1 / 255.0);
//}
//
//void floatToTripleChar(cv::Mat& mat)
//{
//	mat.convertTo(mat, CV_8U, 255.0f);
//}
//
//
//using namespace std;
//using namespace Eigen;
//
//const std::string defaultOutputMode = "s";
//const std::string defaultCfgFilename = ".\\TestData.txt";
//
//int main(int argc, char* argv[])
//{
//	std::string outputMode, cfgFilename;
//	if (argc < 2)
//	{
//		outputMode = defaultOutputMode;
//	}
//	else
//	{
//		outputMode = argv[1];
//	}
//	if (argc < 3)
//	{
//		cfgFilename = defaultCfgFilename;
//	}
//	else
//	{
//		cfgFilename = argv[2];
//	}
//
//	InputModel inputModel(cfgFilename);
//	OutputModel outputModel(outputMode);
//
//	while (inputModel.hasNext())
//	{
//		auto inputMsg = inputModel.getNextInput();
//
//		auto srcMat = inputMsg.srcImg;
//		auto dstMat = inputMsg.dstImg;
//
//		std::shared_ptr<ImageRegion> pRegion;
//
//		if (inputMsg.regionType == RegionType::Rect)
//		{
//			cv::Rect rect(inputMsg.rectLeft, inputMsg.rectTop, inputMsg.rectWidth, inputMsg.rectHeight);
//			pRegion = std::make_shared<ImageRegion>(rect);
//		}
//		else
//		{
//			pRegion = std::make_shared<ImageRegion>(inputMsg.maskImg);
//		}
//
//		const ImageRegion& region = *pRegion;
//		printf("region size = %d\n", region.size());
//
//		int dstLeft = inputMsg.dstLeft, dstTop = inputMsg.dstTop;
//
//		std::vector<cv::Mat> srcChannels;
//		std::vector<cv::Mat> dstChannels;
//		cv::split(srcMat, srcChannels);
//		cv::split(dstMat, dstChannels);
//		std::vector<cv::Mat> outputChannels;
//
//		GradientFilter gradientFilter;
//		int totalTimeInMs = 0;
//		// Laplacian Filter
//		for (int i = 0; i < srcChannels.size(); i++)
//		{
//			const auto& srcImg = srcChannels[i];
//			const auto& dstImg = dstChannels[i];
//			cv::Mat gSrc;
//
//			if (inputMsg.taskType == TaskType::DirectBlend)
//			{
//				gSrc = gradientFilter.calLaplacianImage(srcImg);
//			}
//			else if (inputMsg.taskType == TaskType::OpaqueBlend)
//			{
//				gSrc = cv::Mat(srcImg.size(), srcImg.type());
//				cv::Point op[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
//				for (int i = 0; i < region.size(); i++)
//				{
//					const auto& pos = region[i];
//
//					cv::Point srcPos = { pos.first, pos.second }, dstPos = {pos.first + dstLeft, pos.second + dstTop};
//
//					for (int k = 0; k < 4; k++)
//					{
//						auto srcVPos = srcPos + op[k], dstVPos = dstPos + op[k];
//						auto g1 = srcImg.at<float>(srcPos) - srcImg.at<float>(srcVPos);
//						auto g2 = dstImg.at<float>(dstPos) - dstImg.at<float>(dstVPos);
//						if (std::abs(g1) > std::abs(g2))
//						{
//							gSrc.at<float>(srcPos) += g1;
//						}
//						else
//						{
//							gSrc.at<float>(srcPos) += g2;
//						}
//					}
//				}
//			}
//
//			//cv::imwrite("s " + std::to_string(i) + ".jpg", imgFloatToUcharC1(srcImg));
//			//cv::imwrite("d " + std::to_string(i) + ".jpg", imgFloatToUcharC1(dstImg));
//			//cv::imwrite("g " + std::to_string(i) + ".jpg", imgFloatToUcharC1(gSrc));
//
//			Eigen::SparseMatrix<float> factor;
//			Eigen::VectorXf edgeTerm;
//
//			PoissonSolver::getFactorMatrixAndEdgeTerm(region, dstLeft, dstTop, dstImg, factor, edgeTerm);
//
//			int beginTime = clock();
//			auto res = PoissonSolver::solvePoissonEquation(region, factor, gSrc, edgeTerm);
//			//auto res = PoissonSolver::solvePoissonEquationDiff(region, factor, srcImg, gSrc, edgeTerm);
//			int endTime = clock();
//			totalTimeInMs += endTime - beginTime;
//
//			cv::Mat tmpMat;
//			dstImg.copyTo(tmpMat);
//			for (int i = 0; i < region.size(); i++)
//			{
//				const auto& pos = region[i];
//				tmpMat.at<float>(dstTop + pos.second, dstLeft + pos.first) = res[i];
//				//tmpMat.at<float>(dstTop + pos.second, dstLeft + pos.first) = srcImg.at<float>(pos.second, pos.first) + res[i];
//			}
//			//cv::imwrite("output " + std::to_string(i) + ".jpg", imgFloatToUcharC1(tmpMat));
//			outputChannels.push_back(tmpMat);
//		}
//		cv::Mat resImg;
//		cv::merge(outputChannels, resImg);
//
//		OutputMsg outputMsg;
//		outputMsg.computingTime = totalTimeInMs;
//		outputMsg.srcImg = srcMat;
//		outputMsg.dstImg = dstMat;
//
//		// Direct copy
//		cv::Mat mat3;
//		dstMat.copyTo(mat3);
//		region.imageCopyC3(srcMat, mat3, dstLeft, dstTop);
//		outputMsg.outputMat.push_back(mat3);
//
//		// Image Blending
//		outputMsg.outputMat.push_back(resImg);
//
//		outputModel.pushOutputParameters(outputMsg);
//	}
//
//	return 0;
//}