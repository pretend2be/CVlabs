#pragma once

#include <QImage>
#include "image.h"
#include "poi.h"

namespace cv {

QImage toQImage(const Image& image);
QImage drawPoints(const Image& image, const IPoints& points);

}
