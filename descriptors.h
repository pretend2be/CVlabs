#pragma once

#include <vector>
#include "image.h"
#include "poi.h"
#include "pyramid.h"

namespace cv {

using Descriptor = std::vector<double>;
using Descriptors = std::vector<Descriptor>;
using Matches = std::vector<std::pair<int, int>>;
using Angles = std::vector<double>;

struct Blob{
    Blob(int _x, int _y, double _sigma, const PyramidLevel* _level, int _scale)
        : x(_x)
        , y(_y)
        , sigma(_sigma)
        , level(_level)
        , scale(_scale)
    {}

    int x;
    int y;
    double sigma;
    const PyramidLevel* level;
    int scale;
};

using Blobs = std::vector<Blob>;
std::tuple<Descriptors, Blobs> getDescriptors(const Image& _image, int _grid_size, int histogram_size);

Descriptors getDescriptors(const Image& image, IPoints &points,
                           int blocks_per_grid_side, int block_size_px, int histogram_size);

Matches getMatches(const Descriptors& first, const Descriptors& second);
Blobs getBlobs(const Pyramid& pyramid);
}
