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

Image SobelDx(const Image& image);
Image SobelDy(const Image& image);
Image SobelGradient(const Image& dx, const Image& dy);

}
