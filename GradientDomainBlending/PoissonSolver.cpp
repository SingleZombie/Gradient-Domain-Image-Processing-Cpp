#include "PoissonSolver.h"
#include <opencv2/core/eigen.hpp>
#include <vector>
#include <cassert>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <iostream>

void PoissonSolver::getFactorMatrixAndEdgeTerm(
	// Input
	const ImageRegion& region,
	int dstLeft, int dstTop,
	const cv::Mat& dstMat,
	// Output
	Eigen::SparseMatrix<float>& factor,
	Eigen::VectorXf& edgeTerm)
{
	factor = Eigen::SparseMatrix<float>(region.size(), region.size());
	edgeTerm = Eigen::VectorXf::Zero(region.size());

	std::vector< Eigen::Triplet<float> > tripletArray;
	static int dx[4] = { 0, 0, 1, -1 };
	static int dy[4] = { -1, 1, 0, 0 };
	for (int i = 0; i < region.size(); i++)
	{
		auto pos = region[i];
		int x = pos.first, y = pos.second;
		tripletArray.push_back({ i, i, 4 });
		for (int k = 0; k < 4; k++)
		{
			int vx = x + dx[k];
			int vy = y + dy[k];
			if (!region.inRegion(vx, vy))
			{
				edgeTerm(i) += dstMat.at<float>(dstTop + vy, dstLeft + vx);
				continue;
			}
			tripletArray.push_back({ i, region(vx, vy), -1 });
		}
	}
	factor.setFromTriplets(tripletArray.begin(), tripletArray.end());
}

Eigen::VectorXf PoissonSolver::solvePoissonEquation(
	const Eigen::SparseMatrix<float>& lhs,
	const Eigen::VectorXf& rhs)
{
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;
	solver.analyzePattern(lhs);
	solver.factorize(lhs);
	Eigen::VectorXf tmpRes = solver.solve(rhs);
	return tmpRes;
}