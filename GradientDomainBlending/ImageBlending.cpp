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

	Eigen::VectorXf rhs(region.size());
	for (int i = 0; i < region.size(); i++)
	{
		auto pr = region[i];
		rhs(i) = gSrc.at<float>(pr.second, pr.first);
	}
	rhs += edgeTerm;

	int beginTime = clock();

	auto res = PoissonSolver::solvePoissonEquation(factor, rhs);

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

AdvancedBlending::AdvancedBlending()
{

}
void AdvancedBlending::init(
	// Input
	const cv::Mat& srcImg,
	const cv::Mat& dstImg,
	const ImageRegion& region,
	int dstLeft, int dstTop,
	TaskType taskType
)
{
	_root = std::make_shared<QuadTreeRoot>(region, dstLeft, dstTop, dstImg.cols, dstImg.rows);
	_root->calKeyPoints();
}
void AdvancedBlending::imageBlending(
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

	Eigen::SparseMatrix<float> lhs;
	Eigen::VectorXf rhs;

	_root->setEquations(srcImg, dstImg, lhs, rhs);

	int beginTime = clock();
	auto res = PoissonSolver::solvePoissonEquation(lhs, rhs);
	int endTime = clock();
	computingTime = endTime - beginTime;
	resultImg = _root->recoverImg(srcImg, dstImg, res);
}

void AdvancedBlending::addOutputInfo(
	std::vector<cv::Mat>& outputMat
)
{
	outputMat.push_back(_root->toImg());
	outputMat.push_back(_root->getDiffMat());
	outputMat.push_back(_root->getKeyMat());
}