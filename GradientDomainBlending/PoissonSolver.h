#ifndef POISSON_SOLVER_H
#define POISSON_SOLVER_H

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp> 
#include "ImageUtil.h"

class PoissonSolver
{
public:
	static void getFactorMatrixAndEdgeTerm(
		// Input
		const ImageRegion& region,
		int dstLeft, int dstTop,
		const cv::Mat& dstMat,
		// Output
		Eigen::SparseMatrix<float>& factor, 
		Eigen::VectorXf& edgeTerm);
	static Eigen::VectorXf solvePoissonEquation(
		const ImageRegion& region, 
		const Eigen::SparseMatrix<float>& A, 
		const cv::Mat& g, 
		const Eigen::VectorXf& rhsTerm);
	static Eigen::VectorXf solvePoissonEquationDiff(
		const ImageRegion& region,
		const Eigen::SparseMatrix<float>& A,
		const cv::Mat& x0,
		const cv::Mat& g,
		const Eigen::VectorXf& rhsTerm);
private:
	PoissonSolver()
	{
	}
};

#endif // !POSSION_SOLVER_H
