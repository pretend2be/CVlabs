#pragma once

#include <QImage>
#include "image.h"
#include "poi.h"
#include "descriptors.h"

namespace cv {

QImage toQImage(const Image& image);
QImage drawPoints(const Image& image, const IPoints& points);
QImage drawMatches(const Image& first_image, const IPoints& first_points,
                   const Image& second_image, const IPoints& second_points, const Matches& matches);
QImage drawMatches(const Image& first_image, const Blobs& first_blobs,
                   const Image& second_image, const Blobs& second_blobs, const Matches& matches);

}
