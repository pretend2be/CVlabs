#pragma once

#include <iostream>
#include <QImage>
#include <QPainter>
#include "matrix.h"

namespace cv {

enum class Border{
    Zero,
    Wrap,
    Copy,
    Mirror
};

class Image : public Matrix
{
public:
    Image(int height, int width, Border border = Border::Mirror);
    Image(const QImage& image, Border border = Border::Mirror);
    Image(const Image& image);

    Image& operator=(const Image& image){
        setHeight(image.getHeight());
        setWidth(image.getWidth());
        initializeIntensityMap();

        for(int i = 0; i < getHeight(); i++)
            for(int j = 0; j < getWidth(); j++)
                set(i, j, image.get(i, j));

        return *this;
    }

    friend Image operator-(const Image& left, const Image& right);
    friend Image operator/(const Image& left, const double value);

    double get(int row, int col) const;

    Border getBorderE();
    Border getBorderE() const;
    void setBorderE(Border border);

    Image getNormalized() const;
    Image getDownscale() const;

private:
    std::pair<int, int> getCopyIdxs(int row, int col) const;
    std::pair<int, int> getMirrorIdxs(int row, int col) const;
    std::pair<int, int> getWrapIdxs(int row, int col) const;

    Border BorderE;
};

}


