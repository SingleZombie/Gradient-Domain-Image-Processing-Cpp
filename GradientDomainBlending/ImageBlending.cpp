#include "ImageBlending.h"

BasicBlending::BasicBlending()
{
	
}

void BasicBlending::imageBlending(
	const cv::Mat& srcImg,
	const cv::Mat& dstImg,
	const ImageRegion& region,
	int dstLeft, int dstTop,
	TaskType taskType,
	cv::Mat& resultImg,
	int& computingTime
)
{
	computingTime = 0;
	cv::Mat gSrc;

	if (taskType == TaskType::DirectBlend)
	{
		gSrc = _gradientFilter.calLaplacianImage(srcImg);
	}
	else if (taskType == TaskType::OpaqueBlend)
	{
		gSrc = cv::Mat(srcImg.size(), srcImg.type());
		cv::Point op[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
		for (int i = 0; i < region.size(); i++)
		{
			const auto& pos = region[i];

			cv::Point srcPos = { pos.first, pos.second }, dstPos = { pos.first + dstLeft, pos.second + dstTop };

			for (int k = 0; k < 4; k++)
			{
				auto srcVPos = srcPos + op[k], dstVPos = dstPos + op[k];
				auto g1 = srcImg.at<float>(srcPos) - srcImg.at<float>(srcVPos);
				auto g2 = dstImg.at<float>(dstPos) - dstImg.at<float>(dstVPos);
				if (std::abs(g1) > std::abs(g2))
				{
					gSrc.at<float>(srcPos) += g1;
				}
				else
				{
					gSrc.at<float>(srcPos) += g2;
				}
			}
		}
	}

	Eigen::SparseMatrix<float> factor;
	Eigen::VectorXf edgeTerm;

	PoissonSolver::getFactorMatrixAndEdgeTerm(region, dstLeft, dstTop, dstImg, factor, edgeTerm);

	int beginTime = clock();
	auto res = PoissonSolver::solvePoissonEquation(region, factor, gSrc, edgeTerm);
	//auto res = PoissonSolver::solvePoissonEquationDiff(region, factor, srcImg, gSrc, edgeTerm);
	int endTime = clock();
	computingTime = endTime - beginTime;

	resultImg = cv::Mat();
	dstImg.copyTo(resultImg);
	for (int i = 0; i < region.size(); i++)
	{
		const auto& pos = region[i];
		resultImg.at<float>(dstTop + pos.second, dstLeft + pos.first) = res[i];
	}	
}