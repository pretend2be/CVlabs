#include "pyramid.h"
#include "filters.h"

namespace cv {

Pyramid::Pyramid(const Image& image, const double sigma0, const double sigmaA, const int octaves, const int levels)
{
    Image result(image.getHeight(), image.getWidth());

    auto k = pow(2.0, 1.0 / levels);

    auto old_sigma = sigma0;
    auto glob_sigma = sigma0;
    auto current_sigma = sqrt(old_sigma * old_sigma - sigmaA * sigmaA);

    result = GaussianFilterSeparable(image, current_sigma);

    for(int i = 0; i < octaves; i++){

        PyramidOctave octave(i);
        old_sigma = sigma0;

        for(int j = 0; j < levels; j++){
            auto new_sigma = old_sigma * k;
            current_sigma = sqrt(new_sigma * new_sigma - old_sigma * old_sigma);
            if(i == 0 && j == 0){
                PyramidLvl level(result, old_sigma, glob_sigma, j);
                octave.addLevel(level);
            }
            else{
            result = GaussianFilterSeparable(result, current_sigma);
            PyramidLvl level(result, old_sigma, glob_sigma, j);
            octave.addLevel(level);
            }

            old_sigma = new_sigma;
            glob_sigma = glob_sigma * k;
        }

        addOctave(octave);
        result = result.getDownscale();
    }
}

void Pyramid::savePyramid(const std::string& file) const{
    auto oct = octaves.size();
    for(int i = 0; i < oct; i++){
        auto NoL = octaves[i].NumberOfLevels();
        for(int j = 0; j < NoL; j++){
            auto level = octaves[i].getLevel(j);
            auto name = file + "_" + std::to_string(level.getEffectiveSigma()) +
                    "_" + std::to_string(level.getCurrentSigma()) + ".png";
            level.getImage().save(name);
        }
    }
}

void Pyramid::addOctave(PyramidOctave& octave){
    octaves.push_back(octave);
}

Image Pyramid::getImage(int octave, int level){
    return octaves[octave].getLevelImage(level);
}

}

