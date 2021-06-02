#include "QuadTree.h"
#include <cstdio>

QuadTree::QuadTree(int _xl, int _yl, int _xr, int _yr)
	: xl(_xl), xr(_xr), yl(_yl), yr(_yr), length(_xr - _xl), 
	tl(nullptr), tr(nullptr), bl(nullptr), br(nullptr)
{

}

QuadTree::~QuadTree()
{
	if (tl)
	{
		delete tl, tr, bl, br;
	}
}

void QuadTree::createSons()
{
	int xm = (xl + xr) / 2;
	int ym = (yl + yr) / 2;
	tl = new QuadTree(xl, yl, xm, ym);
	tr = new QuadTree(xm, yl, xr, ym);
	bl = new QuadTree(xl, ym, xm, yr);
	br = new QuadTree(xm, ym, xr, yr);
}

pQuadTree QuadTree::find(int x, int y)
{
	if (x < xl || x >= xr || y < yl || y >= yr)
	{
		return nullptr;
	}
	if (isLeaf())
	{
		return this;
	}
	return findNextSon(x, y)->find(x, y);
}
pQuadTree QuadTree::findNextSon(int x, int y) const
{
	int xm = (xl + xr) / 2;
	int ym = (yl + yr) / 2;
	if (x < xm)
	{
		return y < ym ? tl : bl;
	}
	else
	{
		return y < ym ? tr : br;
	}
}


// QuadTreeRoot

float QuadTreeRoot::INF = 10000000;


QuadTreeRoot::QuadTreeRoot(const ImageRegion& region, int _ox, int _oy, int _width, int _height) :
	_region(region),
	ox(_ox),
	oy(_oy),
	width(_width),
	height(_height)
{
	buildTree();
}

void QuadTreeRoot::calKeyPoints()
{
	std::fill(xArr.begin(), xArr.end(), -1);
	std::fill(yArr.begin(), yArr.end(), -1);
	_keyPointMat = cv::Mat(cv::Size(width, height), CV_8UC3);
	auto keyPointFunc = [&](pQuadTree cNode)
	{
		if (cNode->xl == 0 || cNode->tl == 0 || cNode->length == 1 || (xArr[cNode->xl] != -1 && yArr[cNode->yl] != -1) )
		{
			if (inBlendingRegion(cNode->xl, cNode->yl))
			{
				int cId = keyPoints.size();
				keyPointId[{ cNode->xl, cNode->yl }] = cId;
				keyPoints.push_back({ cNode->xl, cNode->yl });
			}
		}
		else
		{
			
		}
		xArr[cNode->xl] = 1;
		yArr[cNode->yl] = 1;
		//for (int x = cNode->xl + 1; x < cNode->xr; x++)
		//{
		//	xArr[x] = -1;
		//}
		//for (int y = cNode->yl + 1; y < cNode->yr; y++)
		//{
		//	yArr[y] = -1;
		//}
	};
	traverse(keyPointFunc);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (keyPointId.count({ i, j }))
			{
				_keyPointMat.at<cv::Vec3b>(j, i)[2] = 255;
				_keyPointMat.at<cv::Vec3b>(j, i)[1] = 0;
				_keyPointMat.at<cv::Vec3b>(j, i)[0] = 0;
			}
			else
			{
				_keyPointMat.at<cv::Vec3b>(j, i)[2] = 0;
				_keyPointMat.at<cv::Vec3b>(j, i)[1] = 0;
				_keyPointMat.at<cv::Vec3b>(j, i)[0] = 0;
			}
		}
	}
}

void QuadTreeRoot::setEquations(
	// Input
	const cv::Mat& srcMat,
	const cv::Mat& dstMat,
	// Output
	Eigen::SparseMatrix<float>& lhs,
	Eigen::VectorXf& rhs)
{
	std::fill(xArr.begin(), xArr.end(), -1);
	std::fill(yArr.begin(), yArr.end(), -1);

	int n = keyPoints.size();
	lhs = Eigen::SparseMatrix<float>(n, n);
	rhs = Eigen::VectorXf::Zero(n);

	std::vector< Eigen::Triplet<float> > tripletArray;
	std::vector<float> weightArr = std::vector<float>(n);

	auto keyPointFunc = [&](pQuadTree cNode)
	{
		int xl = cNode->xl, yl = cNode->yl;
		if (cNode->xl == 0 || cNode->tl == 0 || cNode->length == 1 || (xArr[cNode->xl] != -1 && yArr[cNode->yl] != -1))
		{
			std::pair<int, int> pos = { xl, yl };
			int topX = xl, topY = xArr[xl];
			int leftX = yArr[yl], leftY = yl;
			if (keyPointId.count(pos))
			{
				int cId = keyPointId[pos];
				weightArr[cId] = 4.0;

				auto checkNode = [&](int x, int y)
				{ 
					if (!inRegion(x, y))
					{
						return;
					}
					int l = std::max(std::abs(x - xl), std::abs(y - yl));
					if (keyPointId.count({ x, y }))
					{
						int id = keyPointId[{x, y}];
						weightArr[cId] -= (l - 1.0f) / l;
						weightArr[id] -= (l - 1.0f) / l;                      
						tripletArray.push_back({ cId, id, -1.0f / l });
						tripletArray.push_back({ id, cId, -1.0f / l });
					}
					else
					{
						if (x < xl)
						{
							rhs(cId) += dstMat.at<float>(yl, xl - 1) - srcMat.at<float>(yl - oy, xl - ox - 1);
						}
						else // if (y < yl)
						{
							rhs(cId) += dstMat.at<float>(yl - 1, xl) - srcMat.at<float>(yl - oy -1, xl - ox);
						}
					}
				};
				checkNode(topX, topY);
				checkNode(leftX, leftY);
	
			}
			else
			{
				auto checkNode = [&](int x, int y)
				{
					if (!inRegion(x, y))
					{
						return;
					}
					int l = std::max(std::abs(x - xl), std::abs(y - yl));
					if (keyPointId.count({ x, y }))
					{
						int id = keyPointId[{x, y}];
						if (xl > x) // right
						{
							rhs(id) += dstMat.at<float>(y, x + 1) - srcMat.at<float>(y - oy, x - ox + 1);
						}
						else // yl > y // bottom
						{
							rhs(id) += dstMat.at<float>(y + 1, x)- srcMat.at<float>(y - oy + 1, x - ox);
						}
					}
				};
				checkNode(topX, topY);
				checkNode(leftX, leftY);
			}

			xArr[cNode->xl] = yl;
			yArr[cNode->yl] = xl;

		}
		
		//for (int x = cNode->xl + 1; x < cNode->xr; x++)
		//{
		//	xArr[x] = -1;
		//}
		//for (int y = cNode->yl + 1; y < cNode->yr; y++)
		//{
		//	yArr[y] = -1;
		//}
	};
	traverse(keyPointFunc);

 	for (int i = 0 ; i < n; i++)
	{
		tripletArray.push_back({ i, i, weightArr[i] });
	}
	lhs.setFromTriplets(tripletArray.begin(), tripletArray.end());
}

void  QuadTreeRoot::fillDFS(cv::Mat& mat, pQuadTree cNode)
{
	auto getColor = [&](int x, int y) -> float
	{
		return y < mat.rows&& x < mat.cols ? mat.at<float>(y, x) : 0;
	};
	auto setColor = [&](int x, int y, float c)
	{
		if (y < mat.rows && x < mat.cols && mat.at<float>(y, x) == INF)
		{
			mat.at<float>(y, x) = c;
		}
	};

	int xl = cNode->xl, yl = cNode->yl, xr = cNode->xr, yr = cNode->yr;
	int xm = (xl + xr) / 2;
	int ym = (yl + yr) / 2;
	float c1 = getColor(xl, yl);
	float c2 = getColor(xr, yl);
	float c3 = getColor(xl, yr);
	float c4 = getColor(xr, yr);
	
	if (cNode->isLeaf())
	{
		int l = cNode->length;
		for (int x = 0; x < cNode->length; x++)
		{
			for (int y = 0; y < cNode->length; y++)
			{
				if (inBlendingRegion(x + cNode->xl, y + cNode->yl))
				{
					float colorLeft = c3 * y / l + c1 * (l - y) / l;
					float colorRight = c4 * y / l + c2 * (l - y) / l;
					float color = colorRight * x / l + colorLeft * (l - x) / l;
					mat.at<float>(y + cNode->yl, x + cNode->xl) = color;
				}
			}
		}
	}
	else
	{
		setColor(xm, yl, (c1 + c2) / 2);
		setColor(xm, yr, (c3 + c4) / 2);
		setColor(xl, ym, (c1 + c3) / 2);
		setColor(xr, ym, (c2 + c4) / 2);
		fillDFS(mat, cNode->tl);
		fillDFS(mat, cNode->tr);
		fillDFS(mat, cNode->bl);
		fillDFS(mat, cNode->br);
	}
}

cv::Mat QuadTreeRoot::recoverImg(
	const cv::Mat& srcMat,
	const cv::Mat& dstMat,
	Eigen::VectorXf& result
)
{
	cv::Mat mat = cv::Mat(dstMat.size(), dstMat.type());
	int width = mat.cols, height = mat.rows;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (inBlendingRegion(j, i))
			{
				if (keyPointId.count({ j, i }))
				{
					int id = keyPointId.at({ j, i });
					mat.at<float>(i, j) = result[id];
				}
				else
				{
					mat.at<float>(i, j) = INF;
				}
			}
			else
			{
				mat.at<float>(i, j) = 0;
			}
		}
	}
	fillDFS(mat, root);

	cv::Mat res;
	dstMat.copyTo(res);
	for (int i = 0; i < _region.size(); i++)
	{
		auto pos = _region[i];
		res.at<float>(oy + pos.second, ox + pos.first) = srcMat.at<float>(pos.second, pos.first);
	}

	res += mat;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			mat.at<float>(i, j) = std::abs(mat.at<float>(i, j));
		}
	}
	_diffMat.push_back(mat);
	return res;
}

cv::Mat QuadTreeRoot::getDiffMat() const
{
	cv::Mat res;
	cv::merge(_diffMat, res);
	return res;
}

bool QuadTreeRoot::inRegion(int x, int y) const
{
	return x >= 0 && x < width && y >= 0 && y < height;
}

bool QuadTreeRoot::inBlendingRegion(int x, int y) const
{
	return _region.inRegion(x - ox, y - oy);
}

int QuadTreeRoot::queryLeafLength(int x, int y)
{
	auto node = root->find(x, y);
	return node->length;
}

void  QuadTreeRoot::traverse(std::function<void(pQuadTree)> func)
{
	traverseDFS(func, root);
}

void QuadTreeRoot::traverse(std::function<void(int, int, int, int)> func) const
{
	traverseDFS(func, root);
}

void QuadTreeRoot::traverseDFS(std::function<void(pQuadTree)> func, pQuadTree cNode)
{
	if (cNode->isLeaf())
	{
		func(cNode);
	}
	else
	{
		traverseDFS(func, cNode->tl);
		traverseDFS(func, cNode->tr);
		traverseDFS(func, cNode->bl);
		traverseDFS(func, cNode->br);
	}
}

void QuadTreeRoot::traverseDFS(std::function<void(int, int, int, int)> func, pQuadTree cNode) const
{
	if (cNode->isLeaf())
	{
		func(cNode->xl, cNode->yl, cNode->xr, cNode->yr);
	}
	else
	{
		traverseDFS(func, cNode->tl);
		traverseDFS(func, cNode->tr);
		traverseDFS(func, cNode->bl);
		traverseDFS(func, cNode->br);
	}
}


cv::Mat QuadTreeRoot::toImg() const
{
	cv::Mat res = cv::Mat(height, width, CV_8UC3);
	auto func = [&](int xl, int yl, int xr, int yr)
	{
		unsigned char b = rand() % 256, g = rand() % 256, r = rand() % 256;
		for (int x = xl; x < xr; x++)
		{
			for (int y = yl; y < yr; y++)
			{
				if (x < width && y < height)
				{
					res.at<cv::Vec3b>(y, x)[0] = b;
					res.at<cv::Vec3b>(y, x)[1] = g;
					res.at<cv::Vec3b>(y, x)[2] = r;
				}
			}
		}
	};
	traverse(func);
	return res;
}

void QuadTreeRoot::split(int x, int y)
{
	split(x, y, 1, root);
}

void QuadTreeRoot::split(int x, int y, int length, pQuadTree cNode)
{
	if (!inRegion(x, y) || cNode->length <= length)
	{
		return;
	}
	if (cNode->isLeaf())
	{
		cNode->createSons();
		split(cNode->xl - 1, cNode->yl, cNode->length, root);
		split(cNode->xl, cNode->yl - 1, cNode->length, root);
		split(cNode->xr - 1, cNode->yr, cNode->length, root);
		split(cNode->xr, cNode->yr - 1, cNode->length, root);
	}
	auto nextNode = cNode->findNextSon(x, y);
	split(x, y, length, nextNode);
}

void QuadTreeRoot::buildTree()
{
	int sidelength = 1;
	int squareLength = std::max(width, height);
	while (sidelength < squareLength)
	{
		sidelength <<= 1;
	}

	xArr = std::vector<int>(sidelength);
	yArr = std::vector<int>(sidelength);
	root = new QuadTree(0, 0, sidelength, sidelength);
	
	for (int i = 0; i < _region.size(); i++)
	{
		auto pos = _region[i];
		bool isEdge = false;;
		auto func = [&](int x, int y)
		{
			if (!_region.inRegion(x, y))
			{
				isEdge = true;
			}
		};
		traverseNeighbor(pos.first, pos.second, func);
		if (isEdge)
		{
			split(pos.first + ox, pos.second + oy);
		}
	}
	for (int i = 0; i < height; i++)
	{
		split(width - 1, i);
	}
	for (int j = 0; j < width; j++)
	{
		split(j, height - 1);
	}
}