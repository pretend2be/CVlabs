#pragma once
#include "image.h"

namespace cv {

class PyramidLvl
{
public:
    PyramidLvl(const Image& image, double current_sigma, double effective_sigma, int level);

    Image getImage() const;
    double getCurrentSigma();
    double getEffectiveSigma();
    int getLevelNumber();

private:
    double Effective_sigma;
    double Current_sigma;
    Image pimage;
    int Level;
};

}

