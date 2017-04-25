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

/*void Image::save(QString file){
    auto H = getHeight(), W = getWidth();
    QImage pic(W, H, QImage::Format_RGB32);

    auto normalized = getNormalized();

    for(int y = 0; y < H; y++)
        for(int x = 0; x < W; x++){
            auto color = int(normalized.get(y, x) * 255);
            pic.setPixel(x, y, qRgb(color, color, color));
        }

    pic.save(file);
}*/

/*void Image::save(const std::string& file){
    auto H = getHeight(), W = getWidth();
    QImage pic(W, H, QImage::Format_RGB32);

    for(int y = 0; y < H; y++)
        for(int x = 0; x < W; x++){
            auto color = int(get(y, x) * 255);
            pic.setPixel(x, y, qRgb(color, color, color));
        }

    pic.save(file.c_str());
}

/*void Image::save(const std::string& file, const IPoints& points){
    auto H = getHeight();
    auto W = getWidth();
    auto size = points.size();
    QImage pic(W, H, QImage::Format_RGB32);

    for(int y = 0; y < H; y++)
        for(int x = 0; x < W; x++){
            auto color = int(get(y, x) * 255);
            pic.setPixel(x, y, qRgb(color, color, color));
        }

    QPainter painter(&pic);
    painter.setPen(Qt::red);

    for(int i = 0; i < size; i++){
        auto x = std::get<0>(points[i]);
        auto y = std::get<1>(points[i]);
        painter.drawPoint(x - 1, y);
        painter.drawPoint(x, y - 1);
        painter.drawPoint(x + 1, y);
        painter.drawPoint(x, y + 1);
    }

    pic.save(file.c_str());
}*/

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


