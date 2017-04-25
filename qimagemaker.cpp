#include "qimagemaker.h"

namespace cv {

QImage toQImage(const Image& image){
    auto H = image.getHeight();
    auto W = image.getWidth();
    QImage pic(W, H, QImage::Format_RGB32);

    for(int y = 0; y < H; y++)
        for(int x = 0; x < W; x++){
            auto color = int(image.get(y, x) * 255);
            pic.setPixel(x, y, qRgb(color, color, color));
        }

    return pic;
}

QImage drawPoints(const Image& image, const IPoints& points){
    auto qimage = toQImage(image);
    auto size = points.size();

    QPainter painter(&qimage);
    painter.setPen(Qt::red);

    for(int i = 0; i < size; i++){
        auto y = std::get<0>(points[i]);
        auto x = std::get<1>(points[i]);
        painter.drawPoint(x - 1, y);
        painter.drawPoint(x, y - 1);
        painter.drawPoint(x + 1, y);
        painter.drawPoint(x, y + 1);
    }

    return qimage;
}

}
