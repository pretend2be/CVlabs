#pragma once
#include "image.h"

namespace cv {

struct PyramidLevel{
    PyramidLevel(double _current_sigma, double _effective_sigma, Image&& _image)
        : current_sigma(_current_sigma)
        , effective_sigma(_effective_sigma)
        , image(std::move(_image))
    {}

    double current_sigma;
    double effective_sigma;
    Image image;
};

struct Octave{
    std::vector<PyramidLevel> levels;
    int index;
};

using Pyramid = std::vector<Octave>;

Pyramid getGaussPyramid(const Image& image, const double sigma_0, const double sigma_a,
                        const int number_of_levels, const int number_of_octaves, size_t levels_for_overlap = 3);
double getL(const Pyramid& pyramid, int y, int x, double sigma);
void savePyramid(const Pyramid& pyramid, const std::string& path);
Pyramid getDoG(const Pyramid& pyramid);

}
