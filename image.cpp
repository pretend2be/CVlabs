#include "image.h"

namespace cv {

Image::Image(int height, int width, Border border)
    : Matrix(height, width)
    , BorderE(border)
{}

Image::Image(const QImage& image, Border border)
    : Image(image.height(), image.width(), border)
{
    for (int y = 0, h = getHeight(); y < h; y++){
        for (int x = 0, w = getWidth(); x < w; x++){
            auto color = image.pixel(x, y);
            auto intensity = ((double)qRed(color) * 0.299 + (double)qGreen(color) * 0.587 + (double)qBlue(color) * 0.114)/255;
            set(y , x, intensity);
        }
    }
}

Image::Image(const Image& image)
    : Matrix(image.getHeight(), image.getWidth())
    , BorderE(image.getBorderE())
{
    for(int i = 0; i < getHeight(); i++)
        for(int j = 0; j < getWidth(); j++)
            set(i, j, image.get(i, j));
}

Image operator-(const Image& left, const Image& right){
    if(left.getHeight() == right.getHeight() && left.getWidth() == right.getWidth()){
        auto H = left.getHeight(), W = left.getWidth();
        Image result(H, W);
        for(int i = 0; i < H; i++)
            for(int j = 0; j < W; j++)
                result.set(i, j, left.get(i, j) - right.get(i, j));
        return result;
    }
}

Image operator/(const Image& left, const double value) {
    Image result(left.getHeight(), left.getWidth());
    std::transform(left.begin(), left.end(), result.begin(), [value](const auto& intensity) {
        return intensity / value;
    });
    return result;
}

Border Image::getBorderE(){
    return BorderE;
}

Border Image::getBorderE() const{
    return BorderE;
}

void Image::setBorderE(Border border) {
    BorderE = border;
}

Image Image::getNormalized() const{
    auto minmax = std::minmax_element(begin(), end());
    auto min_int = *minmax.first;
    auto max_int = *minmax.second;
    Image result(getHeight(), getWidth());
    std::transform(begin(), end(), result.begin(),
            [&](const auto& intensity) {
                return (intensity - min_int) / double(max_int - min_int);
            });

    return result;
}

Image Image::getDownscale() const{
    auto halfHeight = getHeight()/2;
    auto halfWidth = getWidth()/2;
    Image result(halfHeight, halfWidth);

    for(int i = 0; i < halfHeight; i++)
        for(int j = 0; j < halfWidth; j++)
            result.set(i, j, get(i * 2, j * 2));

    return result;
}

double Image::get(int row, int col) const{
    if (Matrix::inRange(row, col)) {
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

std::pair<int, int> Image::getCopyIdxs(int row, int col) const{
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

    return std::make_pair(res_row, res_col);
}

std::pair<int, int> Image::getMirrorIdxs(int row, int col) const{
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

    return std::make_pair(res_row, res_col);
}

std::pair<int, int> Image::getWrapIdxs(int row, int col) const{
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

    return std::make_pair(res_row, res_col);
}

}


