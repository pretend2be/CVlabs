#include <QCoreApplication>
#include "filters.h"
#include "image.h"
#include "matrix.h"
#include "pyramid.h"
#include "poi.h"
#include "qimagemaker.h"

void Lab1();
void Lab2();
void Lab3();

int main()
{
    Lab3();

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

    cv::toQImage(sobelX).save("E:\\Magistr\\CV\\sobelx.png");
    cv::toQImage(sobelY).save("E:\\Magistr\\CV\\sobely.png");
    cv::toQImage(sobelGrad).save("E:\\Magistr\\CV\\sobelgrad.png");

    cv::toQImage(blur).save("E:\\Magistr\\CV\\blur.png");
    cv::toQImage(sharpness).save("E:\\Magistr\\CV\\sharp.png");

    cv::toQImage(gauss).save("E:\\Magistr\\CV\\gauss2.png");
    cv::toQImage(gaussSeparable).save("E:\\Magistr\\CV\\gauss2Separable.png");

    cv::toQImage(original).save("E:\\Magistr\\CV\\orig.png");
    printf("done");
}

void Lab2(){
    QImage qimageorig("E:\\Magistr\\CV\\image1.png");
    cv::Image original(qimageorig);

    cv::Pyramid pyramid(original, 1.6, 0.5, 5, 5);
    pyramid.savePyramid("E:\\Magistr\\CV\\1\\");
    printf("done");
}

void Lab3(){
    QImage qimageorig("E:\\Magistr\\CV\\Lenna1.png");
    cv::Image original(qimageorig);

    auto moravec = cv::Moravec(original, 7);
    auto harris = cv:: Harris(original, 3);

    auto pointsMoravec = cv::getPoi(moravec, 0.05, 7);
    auto pointsHarris = cv::getPoi(harris, 0.55, 3);

    cv::drawPoints(original, pointsHarris).save("E:\\Magistr\\CV\\harris.png");
    cv::drawPoints(original, pointsMoravec).save("E:\\Magistr\\CV\\moravec.png");
    cv::drawPoints(original, cv::filterPoints(pointsHarris, 200)).save("E:\\Magistr\\CV\\harris_filter.png");
    cv::drawPoints(original, cv::filterPoints(pointsMoravec, 200)).save("E:\\Magistr\\CV\\moravec_filter.png");
    printf("done");
}
