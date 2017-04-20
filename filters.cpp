#define _USE_MATH_DEFINES
#include "filters.h"
#include "kernels.h"
#include <numeric>

namespace cv {

Image Convolution(const Image& image, const Matrix& kernel){
    int H = image.getHeight(), W = image.getWidth();
    int kH = kernel.getHeight(), kW = kernel.getWidth();
    Image result(H , W);

    int half_height = kH / 2;
    int half_width = kW / 2;

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            double result_intensity = 0;
            for(int x = 0; x < kH; x++)
                for(int y = 0; y < kW; y++){
                    auto intensity = image.get(i + x - half_height, j + y - half_width);
                    result_intensity += kernel.get(x, y) * intensity;
                }
            result.set(i, j, result_intensity);
        }
    return result;
}

Image SeparableConvolution(const Image& image, const double* rows, const double* cols, int size){
    int H = image.getHeight(), W = image.getWidth(), half = size / 2;
    Image temp(H, W);

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            double result_intensity = 0;
            for(int y = 0; y < size; y++){
                    auto intensity = image.get(i, j + y - half);
                    result_intensity += rows[y] * intensity;
            }
            temp.set(i, j, result_intensity);
        }

    Image result(H, W);

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            double result_intensity = 0;
            for(int x = 0; x < size; x++){
                    auto intensity = temp.get(i + x - half, j);
                    result_intensity += cols[x] * intensity;
            }
            result.set(i, j, result_intensity);
        }

    return result;
}

Image SobelDx(const Image& image){
    Matrix kernelX(3, 3, kernelSobelX);
    auto result = Convolution(image, kernelX);
    return result;
}

Image SobelDy(const Image& image){
    Matrix kernelY(3, 3, kernelSobelY);
    auto result = Convolution(image, kernelY);
    return result;
}

Image SobelGradient(const Image& dx, const Image& dy){
    int H = dx.getHeight(), W = dx.getWidth();
    Image result(H, W);

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            double x = dx.get(i, j), y = dy.get(i, j);
            auto gradient = sqrt(x*x+y*y);
            result.set(i, j, gradient);
        }

    return result;
}

Image Sharpness(const Image& image){
    Matrix kernel(3, 3, kernelSharpness);
    auto result = Convolution(image, kernel);
    return result;
}

Image Blur(const Image& image){
    Matrix kernel(3, 3, kernelBlur);
    auto result = Convolution(image, kernel);
    return result;
}

Matrix getGaussKernel(double sigma){
    int kernel_size = int(std::ceil(sigma)) * 6 + 1;
    Matrix kernel(kernel_size,kernel_size);

    int half = kernel_size / 2;

    for(int i = 0; i < kernel_size; i++)
        for(int j = 0; j < kernel_size; j++){
            double sqr = 2 * sigma * sigma;
            double ex = ((i - half) * (i - half) + (j - half) * (j - half)) / sqr;
            auto gauss = 1.0 / (M_PI * sqr ) * exp(-ex);
            kernel.set(i, j, gauss);
        }

    return kernel;
}

Image GaussianFilter(const Image& image, double sigma){
    Matrix kernel = getGaussKernel(sigma);
    auto result = Convolution(image, kernel);
    return result;
}

Image GaussianFilterSeparable(const Image& image, double sigma){
    Matrix kernel = getGaussKernel(sigma);
    int kernel_size = kernel.getHeight();

    double* rows = new double[kernel_size];
    double* cols = new double[kernel_size];

    for(int i = 0; i < kernel_size; i++)
        rows[i] = cols[i] = sqrt(kernel.get(i,i));

    auto result = SeparableConvolution(image, rows, cols, kernel_size);
    return result;
}

}
