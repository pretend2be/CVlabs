#include <QCoreApplication>
#include "filters.h"
#include "image.h"
#include "matrix.h"
#include "pyramid.h"
#include "poi.h"
#include "qimagemaker.h"
#include "descriptors.h"

void Lab1();
void Lab2();
void Lab3();
void Lab4();
void Lab5();
void Lab6();

int main()
{
    Lab6();

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
    QImage qimageorig("E:\\Magistr\\CV\\m.png");
    cv::Image original(qimageorig);

    cv::Pyramid pyramid = cv::getGaussPyramid(original, 1.6, 0.5, 5, 5);
    cv::savePyramid(pyramid, "E:\\Magistr\\CV\\3\\");
    printf("done");
}

void Lab3(){
    QImage qimageorig("E:\\Magistr\\CV\\Lenna1.png");
    cv::Image original(qimageorig);

    auto moravec = cv::Moravec(original, 7);
    auto harris = cv:: Harris(original, 3);

    auto pointsMoravec = cv::getPoi(moravec, 0.05, 7);
    auto pointsHarris = cv::getPoi(harris, 0.55, 3);

    cv::drawPoints(original, pointsHarris).save("E:\\Magistr\\CV\\harris1.png");
    cv::drawPoints(original, pointsMoravec).save("E:\\Magistr\\CV\\moravec.png");
    cv::drawPoints(original, cv::filterPoints(pointsHarris, 200)).save("E:\\Magistr\\CV\\harris_filter1.png");
    cv::drawPoints(original, cv::filterPoints(pointsMoravec, 200)).save("E:\\Magistr\\CV\\moravec_filter.png");
    printf("done");
}

void Lab4(){
    QImage qimage1("E:\\Magistr\\CV\\m.png");
    QImage qimage2("E:\\Magistr\\CV\\m.png");

    QTransform rotating;
    rotating.rotate(37);
    qimage2 = qimage2.transformed(rotating);
    //qimage2.save("E:\\Magistr\\CV\\m90'.png");

    cv::Image image1(qimage1);
    cv::Image image2(qimage2);

    auto harris_first = cv::Harris(image1, 3);
    auto harris_second = cv::Harris(image2, 3);

    auto first_poi = cv::getPoi(harris_first, 5.0, 5);
    auto second_poi = cv::getPoi(harris_second, 5.0, 5);

    auto first_filtered = cv::filterPoints(first_poi, 1000);
    auto second_filtered = cv::filterPoints(second_poi, 1000);

    auto first_descriptors = cv::getDescriptors(image1, first_filtered, 4, 4, 8);
    auto second_descriptors = cv::getDescriptors(image2, second_filtered, 4, 4, 8);

    auto matches = cv::getMatches(first_descriptors, second_descriptors);

    std::cout << matches.size() << std::endl;

    cv::drawMatches(image1, first_filtered, image2, second_filtered, matches).save("E:\\Magistr\\CV\\test9.png");
    printf("done");
}

void Lab5(){
    QImage qimage1("E:\\Magistr\\CV\\m.png");
    QImage qimage2("E:\\Magistr\\CV\\m.png");
    cv::Image image1(qimage1);
    cv::Image image2(qimage2);

    auto harris_first = cv::Harris(image1, 3);
    auto harris_second = cv::Harris(image2, 3);

    auto first_poi = cv::getPoi(harris_first, 0.6, 3);
    auto second_poi = cv::getPoi(harris_second, 0.6, 3);

    auto first_filtered = cv::filterPoints(first_poi, 50);
    auto second_filtered = cv::filterPoints(second_poi, 50);

    auto first_descriptors = cv::getDescriptors(image1, first_filtered, 4, 4, 8);
    auto second_descriptors = cv::getDescriptors(image2, second_filtered, 4, 4, 8);

    auto matches = cv::getMatches(first_descriptors, second_descriptors);

    cv::drawMatches(image1, first_filtered, image2, second_filtered, matches).save("E:\\Magistr\\CV\\test1.png");
    printf("done");
}

void Lab6(){
    QImage qimage1("E:\\Magistr\\CV\\m.png");
    QImage qimage2("E:\\Magistr\\CV\\m.png");

    size_t w = qimage2.width() / 1.5;
    size_t h = qimage2.height() / 1.5;
    qimage2 = qimage2.scaled(w, h, Qt::KeepAspectRatio);
    qimage2.save("E:\\Magistr\\CV\\scaled.png");

    cv::Image image1(qimage1);
    cv::Image image2(qimage2);

    auto first_descriptors = cv::getDescriptors(image1, 16, 8);
    auto second_descriptors = cv::getDescriptors(image2, 16, 8);

    auto matches = cv::getMatches(std::get<0>(first_descriptors), std::get<0>(second_descriptors));

    std::cout << matches.size() << std::endl;

    cv::drawMatches(image1, std::get<1>(first_descriptors), image2, std::get<1>(second_descriptors), matches).save("E:\\Magistr\\CV\\test10.png");
    printf("done");
}
