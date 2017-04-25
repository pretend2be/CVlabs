#pragma once

#include "image.h"
#include <vector>

namespace cv {

using IPoint = std::tuple<int, int, double>;
using IPoints = std::vector<IPoint>;

double getDistance(int x1, int y1, int x2, int y2);

Image Moravec(const Image& image, int window_size);
Image Harris(const Image& image, int window_size, double k = 0.05);

IPoints getPoi(const Image& image, double treshold, int p_size);
IPoints filterPoints(const IPoints &points, int target_quantity);

}
