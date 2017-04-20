#pragma once
#include "pyramidoctave.h"

namespace cv {

class Pyramid
{
public:
    Pyramid(const Image& image, const double sigma0, const double sigmaA, const int octaves, const int levels);
    void savePyramid(const std::string &file) const;
    void addOctave(PyramidOctave& octave);
    Image getImage(int octave, int level);

private:
    std::vector<PyramidOctave> octaves;

};

}
