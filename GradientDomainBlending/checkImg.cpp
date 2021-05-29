#include "main.h"

using namespace cv;

int main(int argc, char* argv[])
{
	auto src = imread(argv[1]);
	auto dst = imread(argv[2]);
	if (src.size() != dst.size() || src.channels() != dst.channels())
	{
		printf("img size doesn't match\n");
		return 0;
	}
	std::vector<cv::Mat> srcChannels, dstChannels;
	cv::split(src, srcChannels);
	cv::split(dst, dstChannels);
	double sum = 0;
	long long cnt = 0;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			for (int k = 0; k < srcChannels.size(); k++)
			{
				if (srcChannels[0].type() == CV_8U)
				{
					sum += abs(int(srcChannels[k].at<uchar>(i, j)) - int(dstChannels[k].at<uchar>(i, j))) / 255.0;
				}
				else if (srcChannels[0].type() == CV_32F)
				{
					sum += abs(srcChannels[k].at<float>(i, j) - dstChannels[k].at<float>(i, j));
				}
				cnt++;
			}
		}
	}
	printf("%lf\n", sum / cnt);

	return 0;
}