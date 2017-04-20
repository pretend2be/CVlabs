#include <QCoreApplication>
#include "filters.h"
#include "image.h"
#include "matrix.h"
#include "pyramid.h"

void Lab1();
void Lab2();

int main()
{
    Lab2();

    return 0;
}

void Lab1(){
    QImage qimage("E:\\Magistr\\CV\\image1.png");
    cv::Image original(qimage);

    auto sobelX = cv::SobelDx(original);
    auto sobelY = cv::SobelDy(original);
    auto sobelGrad = cv::SobelGradient(sobelX, sobelY);

    auto blur = cv::Blur(original);
    auto sharpness = cv::Sharpness(original);

    auto gauss = cv::GaussianFilter(original, 2);
    auto gaussSeparable = cv::GaussianFilterSeparable(original, 2);

    sobelX.save("E:\\Magistr\\CV\\sobelx.png");
    sobelY.save("E:\\Magistr\\CV\\sobely.png");
    sobelGrad.save("E:\\Magistr\\CV\\sobelgrad.png");

    blur.save("E:\\Magistr\\CV\\blur.png");
    sharpness.save("E:\\Magistr\\CV\\sharp.png");

    gauss.save("E:\\Magistr\\CV\\gauss2.png");
    gaussSeparable.save("E:\\Magistr\\CV\\gauss2Separable.png");

    original.save("E:\\Magistr\\CV\\orig.png");
}

void Lab2(){
    QImage qimageorig("E:\\Magistr\\CV\\image.png");
    cv::Image original(qimageorig);

    cv::Pyramid pyramid(original, 1.6, 0.5, 5, 5);
    pyramid.savePyramid("E:\\Magistr\\CV\\1\\");
}
