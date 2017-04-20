#include "matrix.h"

namespace cv {

Matrix::Matrix()
    : IntensityMap(nullptr)
    , Height(0)
    , Width(0)
{}

Matrix::Matrix(int height, int width)
    : Height(height)
    , Width(width)
{
    initializeIntensityMap();
}

Matrix::Matrix(int height, int width, const double array[])
    : Matrix(height, width)
{
    std::copy(array, array + getHeight() * getWidth(), begin());
}

int Matrix::getHeight() const{
    return Height;
}

int Matrix::getWidth() const{
    return Width;
}

void Matrix::setHeight(int height){
    Height = height;
}

void Matrix::setWidth(int width){
    Width = width;
}

double* Matrix::begin(){
    return IntensityMap.get();
}

double* Matrix::end(){
    return begin() + Height * Width;
}

double* Matrix::begin() const{
    return IntensityMap.get();
}

double* Matrix::end() const{
    return begin() + Height * Width;
}

void Matrix::set(int row, int col, double val) {
    if(inRange(row, col)){
        IntensityMap[row * Width + col] = val;
    }
}

double Matrix::get(int row, int col) const{
    if(inRange(row, col)){
        return IntensityMap[row * Width + col];
    }
}

bool Matrix::inRange(int row, int col) const{
    return row >= 0 && row < int(Height) && col >= 0 && col < int(Width);
}

void Matrix::initializeIntensityMap() {
    IntensityMap.release();
    IntensityMap = std::make_unique<double[]>(size_t(Height * Width));
}

}

