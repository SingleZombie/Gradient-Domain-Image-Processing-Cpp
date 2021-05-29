#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <map>

cv::Mat imgUcharToFloat(const cv::Mat& mat);
cv::Mat imgFloatToUcharC1(const cv::Mat& mat);
cv::Mat imgFloatToUchar(const cv::Mat& mat);

float cvAngle(cv::Vec2f v1, cv::Vec2f v2);

class ImageRegion
{
public:
	ImageRegion(const cv::Mat& maskImg);
	ImageRegion(const cv::Rect& rect);
	std::pair<int, int> operator[](int index) const;
	int operator()(int x, int y) const{ return _posIndex.at({x, y}); }
	bool inRegion(int x, int y) const { return _posIndex.count({ x, y }); }
	size_t size() const { return _posIndexArray.size(); }
	void imageCopy(const cv::Mat& srcMat, cv::Mat& dstMat, int x, int y) const;
	void imageCopyC3(const cv::Mat& srcMat, cv::Mat& dstMat, int x, int y) const;
	const std::vector<cv::Point>& getVertices() const { return _vertices; }
private:
	void calIndexArray(const cv::Mat& maskImg);
	std::vector<std::pair<int, int>> _posIndexArray;
	std::vector<cv::Point> _vertices;
	std::map<std::pair<int, int>, int> _posIndex;
};

#endif