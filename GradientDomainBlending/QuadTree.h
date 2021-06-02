#ifndef QUAD_TREE_H
#define QUAD_TREE_H

#include <algorithm>
#include "ImageUtil.h"
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

class QuadTree;

using pQuadTree = QuadTree*;

class QuadTree
{
public:
	int xl, xr, yl, yr;
	int length;
	pQuadTree tl, tr, bl, br;

	QuadTree(int _xl, int _yl, int _xr, int _yr);
	~QuadTree();
	void createSons();
	bool isLeaf() const { return tl == nullptr; }
	bool inNode(int x, int y) const { return x >= xl && x < xr && y >= yl && y < yr; }
	bool inSquare(int x, int y) const { return x >= xl && x <= xr && y >= yl && y <= yr; }
	pQuadTree find(int x, int y);
	pQuadTree findNextSon(int x, int y) const;
private:
	
};

class QuadTreeRoot
{
public:
	QuadTreeRoot(const ImageRegion& region, int ox, int oy, int width, int height);
	pQuadTree root;
	int width, height;
	int ox, oy;

	// Main function
	void calKeyPoints();
	cv::Mat toImg() const;
	cv::Mat getDiffMat() const;
	cv::Mat getKeyMat() const { return _keyPointMat; }

	void setEquations(
		// Input
		const cv::Mat& srcMat,
		const cv::Mat& dstMat,
		// Output
		Eigen::SparseMatrix<float>& lhs,
		Eigen::VectorXf& rhs);

	cv::Mat recoverImg(
		const cv::Mat& srcMat,
		const cv::Mat& dstMat,
		Eigen::VectorXf& result
	);

private:
	ImageRegion _region;
	std::vector<int> xArr, yArr;
	std::vector<cv::Point> keyPoints;
	std::map<std::pair<int, int>, int> keyPointId;
	std::map<std::pair<int, int>, std::pair<cv::Point, cv::Point>> nonkeyPointNeighbor;

	// Utility
	bool inRegion(int x, int y) const;
	bool inBlendingRegion(int x, int y) const;
	int queryLeafLength(int x, int y);
	void traverse(std::function<void(pQuadTree)> func);
	void traverse(std::function<void(int, int, int, int)> func) const;
	void traverseDFS(std::function<void(pQuadTree)> func, pQuadTree cNode);
	void traverseDFS(std::function<void(int, int, int, int)> func, pQuadTree cNode) const;
	// logic functions

	// Let a pixel have shortest length(1 x 1) in quadtree
	// Recursive split the neighbors to satisfy the length constrain(see details in paper)
	void split(int x, int y);
	void split(int x, int y, int length, pQuadTree cNode);

	void fillDFS(cv::Mat& mat, pQuadTree cNode);
	// main functions
	void buildTree();

	static float INF;
	std::vector<cv::Mat> _diffMat;
	cv::Mat _keyPointMat;
};

#endif