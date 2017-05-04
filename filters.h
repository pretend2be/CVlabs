#pragma once

#include "image.h"
#include "matrix.h"

namespace cv {

Image Convolution(const Image& image, const Matrix& kernel);
Image SeparableConvolution(const Image& image, const double* rows, const double* columns, int size);

Image Blur(const Image& image);
Image Sharpness(const Image& image);

Matrix getGaussKernel(double sigma);
Image GaussianFilter(const Image& image, double sigma);
Image GaussianFilterSeparable(const Image& image, double sigma);

double getGradientValue(double gx, double gy);
double getGradientDirection(double gx, double gy);

Image SobelDx(const Image& image);
Image SobelDy(const Image& image);
Image SobelGradient(const Image& dx, const Image& dy);
Image GradientDirections(const Image& dx, const Image& dy);

double getDx(const Image& image, int row, int col);
double getDy(const Image& image, int row, int col);
double getDx2(const Image& image, int row, int col);
double getDy2(const Image& image, int row, int col);
double getDxDy(const Image& image, int row, int col);

}
