#pragma once
#include <memory>

namespace cv {

class Matrix
{
public:
    Matrix();
    Matrix(int height, int width);
    Matrix(int height, int width, const double array[]);

    double get(int row, int col);
    void set(int row, int col, double val);

    int getHeight();
    int getWidth();

    double* begin();
    double* end();

private:
    void initializeIntensityMap();
    bool inRange(int row, int col);

    std::unique_ptr<double[]> IntensityMap;
    int Height;
    int Width;
};
}

