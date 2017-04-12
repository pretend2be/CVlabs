#include "image.h"

namespace cv {

Image::Image(int height, int width, Border border = Border::Wrap)
    : Matrix(height, width)
    , BorderE(border)
{}

Image::Image(const QImage& image, Border border = Border::Wrap)
    : Image(image.height(), image.width(), border)
{
    for (int i = 0, h = getHeight(); i < h; i++){
        for (int j = 0, w = getWidth(); j < w; j++){
            auto color = image.pixel(i,j);
            auto intensity = ((double)qRed(color) * 0.299 + (double)qGreen(color) * 0.587 + (double)qBlue(color) * 0.114)/255;
            set(i, j, intensity);
        }
    }
}

Border Image::getBorderE(){
    return BorderE;
}

void Image::setBorderE(Border border) {
    BorderE = border;
}

Image Image::getNormalize(){
    auto minmax = std::minmax_element(begin(), end());
    auto min_int = minmax.first;
    auto max_int = minmax.second;
    Image result(getHeight(), getWidth());
    std::transform(begin(), end(), result.begin(),
            [&](const auto& intensity) {
                return (intensity - min_int) / double(max_int - min_int);
            });
}

double Image::get(int row, int col){
    if (inRange(row, col)) {
        return Matrix::get(row, col);
    }

    if (BorderE == Border::Copy) {
        auto indexes = getCopyIdxs(row, col);
        return Matrix::get(indexes.first, indexes.second);
    } else if (BorderE == Border::Mirror) {
        auto indexes = getMirrorIdxs(row, col);
        return Matrix::get(indexes.first, indexes.second);
    } else if (BorderE == Border::Wrap) {
        auto indexes = getWrapIdxs(row, col);
        return Matrix::get(indexes.first, indexes.second);
    }

    return 0;
}

std::pair<int, int> Image::getCopyIdxs(int row, int col){
    int res_row;
    int res_col;

    if (row < 0) {
        res_row = 0;
    } else if (row >= getHeight()) {
        res_row = getHeight() - 1;
    } else {
        res_row = row;
    }

    if (col < 0) {
        res_col = 0;
    } else if (col >= getWidth()) {
        res_col = getWidth() - 1;
    } else {
        res_col = col;
    }

    return std::makepair(res_row, res_col);
}

std::pair<int, int> Image::getMirrorIdxs(int row, int col){
    int res_row;
    int res_col;

    if (row < 0) {
        res_row = -row - 1;
    } else if (row >= getHeight()) {
        res_row = 2 * getHeight() - row - 1;
    } else {
        res_row = row;
    }

    if (col < 0) {
        res_col = -col - 1;
    } else if (col >= getWidth()) {
        res_col = 2 * getWidth() - col - 1;
    } else {
        res_col = col;
    }

    return std::makepair(res_row, res_col);
}

std::pair<int, int> Image::getWrapIdxs(int row, int col){
    int res_row;
    int res_col;

    if (row < 0) {
        res_row = getHeight() + row;
    } else if (row >= getHeight()) {
        res_row = row - getHeight();
    } else {
        res_row = row;
    }

    if (col < 0) {
        res_col = getWidth() + col;
    } else if (col >= getWidth()) {
        res_col = col - getWidth();
    } else {
        res_col = col;
    }

    return std::makepair(res_row, res_col);
}

}


