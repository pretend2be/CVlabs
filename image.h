#pragma once

#include <iostream>
#include <QImage>
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
    Image(int height, int width, Border border = Border::Wrap);
    Image(const QImage& image, Border border = Border::Wrap);

    double get(int row, int col);

    Border getBorderE();
    void setBorderE(Border border);

    Image getNormalize();

private:
    std::pair<int, int> getCopyIdxs(int row, int col);
    std::pair<int, int> getMirrorIdxs(int row, int col);
    std::pair<int, int> getWrapIdxs(int row, int col);

    Border BorderE;
};

}


