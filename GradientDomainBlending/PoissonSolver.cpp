#include "PoissonSolver.h"
#include <opencv2/core/eigen.hpp>
#include <vector>
#include <cassert>

Eigen::SparseMatrix<float> PoissonSolver::getFactorMatrix(int rows, int cols)
{
	auto getId = [&](int i, int j) -> int
	{
		return i * cols + j;
	};

	Eigen::SparseMatrix<float> res(rows * cols, rows * cols);
	std::vector< Eigen::Triplet<float> > tripletArray;
	static int ox[4] = {0, 0, 1, -1 };
	static int oy[4] = {-1, 1, 0, 0 };
	int cid = 0;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++, cid++)
		{
			tripletArray.push_back({ cid, cid, 4 });
			for (int k = 0; k < 4; k++)
			{
				int vi = i + ox[k];
				int vj = j + oy[k];
				if (vi < 0 || vj < 0 || vi >= rows || vj >= cols)
				{
					continue;
				}
				tripletArray.push_back({ cid, getId(vi, vj), -1 });
			}
		}
	}
	res.setFromTriplets(tripletArray.begin(), tripletArray.end());
	return res;
}

Eigen::VectorXf PoissonSolver::getEdgeTerm(const cv::Mat& image, const cv::Rect& rect)
{
	Eigen::VectorXf res(rect.area());


	auto getId = [&](int i, int j) -> int
	{
		return i * rect.width + j;
	};

	// top
	for (int x = 0; x < rect.width; x++)
	{
		cv::Point p = rect.tl() + cv::Point(x, 0);
		res(getId(0, x)) = image.at<float>(p.y, p.x);
	}

	// bottom
	for (int x = 0; x < rect.width; x++)
	{
		cv::Point p = rect.tl() + cv::Point(x, rect.height - 1);
		res(getId(rect.height - 1, x)) = image.at<float>(p.y, p.x);
	}

	// left
	for (int y = 0; y < rect.height; y++)
	{
		cv::Point p = rect.tl() + cv::Point(0, y);
		res(getId(y, 0)) = image.at<float>(p.y, p.x);
	}

	// right
	for (int y = 0; y < rect.height; y++)
	{
		cv::Point p = rect.tl() + cv::Point(rect.width - 1, y);
		res(getId(y, rect.width - 1)) = image.at<float>(p.y, p.x);
	}

	return res;
}

cv::Mat PoissonSolver::solvePoissonEquation(int rows, int cols, const Eigen::SparseMatrix<float>& A, const cv::Mat& b, const Eigen::VectorXf& rhsTerm)
{
	assert(rows * cols == A.rows());
	assert(A.rows() == b.size().area());
	cv::Mat res(cv::Size(rows, cols), b.type());

	Eigen::VectorXf rhs(rows * cols);
	for (int i = 0, id = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++, id++)
		{
			rhs(id) = b.at<float>(i, j);
		}
	}
	rhs += rhsTerm;
	//cv::cv2eigen(b, rhs);

	Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
	solver.analyzePattern(A);
	solver.factorize(A);
	Eigen::VectorXf tmpRes = solver.solve(rhs);

	for (int i = 0, id = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++, id++)
		{
			res.at<float>(i, j) = tmpRes(id);
		}
	}

	//cv::eigen2cv(tmpRes, res);
	return res;
}