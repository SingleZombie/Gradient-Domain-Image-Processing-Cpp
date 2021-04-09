#ifndef POISSON_SOLVER_H
#define POISSON_SOLVER_H

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp> 

class PoissonSolver
{
public:
	static Eigen::SparseMatrix<float> getFactorMatrix(int rows, int cols);
	static Eigen::VectorXf getEdgeTerm(const cv::Mat& image, const cv::Rect& rect);
	static cv::Mat solvePoissonEquation(int rows, int cols, const Eigen::SparseMatrix<float>& A, const cv::Mat& b, const Eigen::VectorXf& rhsTerm);
private:
	PoissonSolver()
	{
	}
};

#endif // !POSSION_SOLVER_H
