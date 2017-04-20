#include "pyramidlvl.h"

namespace cv {

PyramidLvl::PyramidLvl(const Image& image, double current_sigma, double effective_sigma, int level)
    : pimage(image)
    , Current_sigma(current_sigma)
    , Effective_sigma(effective_sigma)
    , Level(level)
{}

Image PyramidLvl::getImage() const{
    return pimage;
}

double PyramidLvl::getCurrentSigma(){
    return Current_sigma;
}

double PyramidLvl::getEffectiveSigma(){
    return Effective_sigma;
}

int PyramidLvl::getLevelNumber(){
    return Level;
}

}


