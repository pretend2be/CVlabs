#pragma once
#include <memory>

namespace cv {

class Matrix
{
public:
    Matrix();
    Matrix(int height, int width);
    Matrix(int height, int width, const double array[]);

    double get(int row, int col) const;
    void set(int row, int col, double val);

    int getHeight() const;
    int getWidth() const;

    void setHeight(int height);
    void setWidth(int width);

    double* begin();
    double* end();

    double* begin() const;
    double* end() const;

protected:
    bool inRange(int row, int col) const;
    void initializeIntensityMap();

private:
    std::unique_ptr<double[]> IntensityMap;
    int Height;
    int Width;
};
}

