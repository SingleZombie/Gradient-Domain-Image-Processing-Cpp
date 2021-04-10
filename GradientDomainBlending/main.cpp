#include "main.h"


void tripleCharToFloat(cv::Mat& mat)
{
	mat.convertTo(mat, CV_32F, 1 / 255.0);
}

void floatToTripleChar(cv::Mat& mat)
{
	mat.convertTo(mat, CV_8U, 255.0f);
}

int main(int argc, char* argv[])
{
	std::string filename = "Resource\\p1.jpg";
	std::string filename2 = "Resource\\p2.jpg";
	cv::Rect copyRect(220, 120, 50, 50);

	auto srcMat = cv::imread(filename);
	auto dstMat = cv::imread(filename2);

	tripleCharToFloat(srcMat);
	tripleCharToFloat(dstMat);

	auto srcRect = copyRect;
	auto dstRect = cv::Rect(cv::Point(0, 0), dstMat.size());
	srcRect = srcRect & dstRect;

	std::vector<cv::Mat> channels;
	std::vector<cv::Mat> dstChannels;
	cv::split(srcMat, channels);
	cv::split(dstMat, dstChannels);
	std::vector<cv::Mat> outputChannels;

	GradientFilter gradientFilter;
	// Laplacian Filter
	for (int i = 0; i < channels.size(); i++)
	{
		const auto& img = channels[i];
		const auto& dstImg = dstChannels[i];
		cv::Mat gDstMat = gradientFilter.calLaplacianImage(img(srcRect));
		auto lhs = PoissonSolver::getFactorMatrix(srcRect.height, srcRect.width);
		auto rhsTerm = PoissonSolver::getEdgeTerm(dstImg, srcRect);
		auto res = PoissonSolver::solvePoissonEquation(srcRect.height, srcRect.width, lhs, gDstMat, rhsTerm);
		outputChannels.push_back(res);
	}
	cv::Mat resImg;
	cv::merge(outputChannels, resImg);

	//// copy
	cv::Mat mat3;
	dstMat.copyTo(mat3);
	srcMat(srcRect).copyTo(mat3(srcRect));
	resImg.copyTo(dstMat(srcRect));


	floatToTripleChar(srcMat);
	floatToTripleChar(dstMat);
	floatToTripleChar(mat3);

	cv::imshow("img1", srcMat);
	cv::imshow("img2", dstMat);
	cv::imshow("img3", mat3);
	cv::waitKey();

	return 0;
}