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

static void drawAroundPoint(QPainter& painter, int x, int y){
    painter.drawPoint(x - 1, y);
    painter.drawPoint(x, y - 1);
    painter.drawPoint(x + 1, y);
    painter.drawPoint(x, y + 1);
}

QImage drawPoints(const Image& image, const IPoints& points){
    auto qimage = toQImage(image);
    auto size = points.size();

    QPainter painter(&qimage);
    painter.setPen(Qt::red);

    for(int i = 0; i < size; i++){
        auto y = std::get<0>(points[i]);
        auto x = std::get<1>(points[i]);
        drawAroundPoint(painter, x, y);
    }

    return qimage;
}

QImage drawMatches(const Image& first_image, const IPoints& first_points,
                   const Image& second_image, const IPoints& second_points, const Matches& matches){

    int H1 = first_image.getHeight(), W1 = first_image.getWidth();
    int H2 = second_image.getHeight(), W2 = second_image.getWidth();
    Image merged_image(std::max(H1, H2), W1 + W2);

    for(int i = 0; i < H1; i++)
        for(int j = 0; j < W1; j++)
            merged_image.set(i, j, first_image.get(i,j));

    for(int i = 0; i < H2; i++)
        for(int j = 0; j < W2; j++)
            merged_image.set(i, j + W1, second_image.get(i,j));

    auto qimage = toQImage(merged_image);

    QPainter painter(&qimage);

    auto size = matches.size();
    for(size_t i = 0; i < size; i++){
        auto x1 = std::get<1>(first_points[matches[i].first]);
        auto y1 = std::get<0>(first_points[matches[i].first]);
        auto x2 = std::get<1>(second_points[matches[i].second]);
        auto y2 = std::get<0>(second_points[matches[i].second]);

        int r = qrand() % 256, g = qrand() % 256, b = qrand() % 256;
        auto color = QColor(r, g, b);

        painter.setPen(color);
        painter.drawLine(x1, y1, x2 + W1, y2);

        painter.setPen(color);
        drawAroundPoint(painter, x1, y1);
        drawAroundPoint(painter, x2 + W1, y2);
    }

    return qimage;
}

QImage drawMatches(const Image& first_image, const Blobs& first_blobs,
                   const Image& second_image, const Blobs& second_blobs, const Matches& matches){

    int H1 = first_image.getHeight(), W1 = first_image.getWidth();
    int H2 = second_image.getHeight(), W2 = second_image.getWidth();
    Image merged_image(std::max(H1, H2), W1 + W2);

    for(int i = 0; i < H1; i++)
        for(int j = 0; j < W1; j++)
            merged_image.set(i, j, first_image.get(i,j));

    for(int i = 0; i < H2; i++)
        for(int j = 0; j < W2; j++)
            merged_image.set(i, j + W1, second_image.get(i,j));

    auto qimage = toQImage(merged_image);

    QPainter painter(&qimage);

    for(const auto& match : matches){
        auto x1 = first_blobs[match.first].y;
        auto y1 = first_blobs[match.first].x;
        auto x2 = second_blobs[match.second].y;
        auto y2 = second_blobs[match.second].x;

        int r = qrand() % 256, g = qrand() % 256, b = qrand() % 256;

        painter.setPen(QColor(r, g, b, 128));
        painter.drawLine(x1, y1, x2 + W1, y2);

        painter.setPen(QColor(r, g, b, 255));
        drawAroundPoint(painter, x1, y1);
        drawAroundPoint(painter, x2 + W1, y2);
    }

    return qimage;
}

}
