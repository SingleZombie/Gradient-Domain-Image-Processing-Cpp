#include "ImageUtil.h"
#include <cmath>

cv::Mat imgUcharToFloat(const cv::Mat& mat)
{
	cv::Mat tmp;
	mat.convertTo(tmp, CV_32FC3, 1 / 255.0);
	return tmp;
}

cv::Mat imgFloatToUcharC1(const cv::Mat& mat)
{
	cv::Mat tmp;
	mat.convertTo(tmp, CV_8U, 255.0f);
	return tmp;
}

cv::Mat imgFloatToUchar(const cv::Mat& mat)
{
	cv::Mat tmp;
	mat.convertTo(tmp, CV_8UC3, 255.0f);
	return tmp;
}

float cvAngle(cv::Vec2f v1, cv::Vec2f v2)
{
	float cosv = v1.dot(v2) / cv::norm(v1) / cv::norm(v2);
	return std::acos(cosv);
}

ImageRegion::ImageRegion(const cv::Mat& maskImg)
{
	calIndexArray(maskImg);
}

ImageRegion::ImageRegion(const cv::Rect& rect)
{
	for (int i = 0; i < rect.height; i++)
	{
		for (int j = 0; j < rect.width; j++)
		{
			_posIndex[{rect.x + j, rect.y + i }] = _posIndexArray.size();
			_posIndexArray.push_back({rect.x + j, rect.y + i});	
		}
	}

	int ci = rect.y - 1, cj = rect.x;
	while (cj < rect.width)
	{
		_vertices.push_back({ ci, cj});
		cj++;
	}
	ci++;
	while (ci < rect.height)
	{
		_vertices.push_back({ ci, cj });
		ci++;
	}
	cj--;
	while (cj >= 0)
	{
		_vertices.push_back({ ci, cj });
		cj--;
	}
	ci--;
	while (ci >= 0)
	{
		_vertices.push_back({ ci, cj });
		ci--;
	}

}

void ImageRegion::calIndexArray(const cv::Mat& maskImg)
{
	int rows = maskImg.rows, cols = maskImg.cols;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (maskImg.at<cv::Vec3f>(i, j)[0] == 1)
			{
				_posIndex[{j, i }] = _posIndexArray.size();
				_posIndexArray.push_back({ j, i });
			}
		}
	}
}

std::pair<int, int> ImageRegion::operator[](int index) const
{
	return _posIndexArray[index];
}

void ImageRegion::imageCopy(const cv::Mat& srcMat, cv::Mat& dstMat, int x, int y) const
{
	for (const auto& pos : _posIndexArray)
	{
		dstMat.at<float>(y + pos.second, x + pos.first) = srcMat.at<float>(pos.second, pos.first);
	}
}

void ImageRegion::imageCopyC3(const cv::Mat& srcMat, cv::Mat& dstMat, int x, int y) const
{
	for (const auto& pos : _posIndexArray)
	{
		dstMat.at<cv::Vec3f>(y + pos.second, x + pos.first)[0] = srcMat.at<cv::Vec3f>(pos.second, pos.first)[0];
		dstMat.at<cv::Vec3f>(y + pos.second, x + pos.first)[1] = srcMat.at<cv::Vec3f>(pos.second, pos.first)[1];
		dstMat.at<cv::Vec3f>(y + pos.second, x + pos.first)[2] = srcMat.at<cv::Vec3f>(pos.second, pos.first)[2];
	}
}