#include "pyramid.h"
#include "filters.h"
#include "qimagemaker.h"

namespace cv {

Pyramid getGaussPyramid(const Image &image, const double sigma_0, const double sigma_a,
                        const int number_of_levels, const int number_of_octaves, size_t levels_for_overlap){
    auto k = pow(2.0, 1.0 / number_of_levels);
    auto old_sigma = sigma_0;
    auto glob_sigma = sigma_0;
    auto current_sigma = sqrt(old_sigma * old_sigma - sigma_a * sigma_a);

    Pyramid gauss_pyramid(number_of_octaves);

    gauss_pyramid[0].levels.emplace_back(old_sigma, glob_sigma, GaussianFilterSeparable(image, current_sigma));

    for(auto octave_iterator = gauss_pyramid.begin(); octave_iterator != gauss_pyramid.end(); octave_iterator++){
        octave_iterator->index = int(octave_iterator - gauss_pyramid.begin());
        old_sigma = sigma_0;

        for(size_t j = 0; j < number_of_levels + levels_for_overlap; j++){
            const auto& level = octave_iterator->levels.back();
            auto new_sigma = old_sigma * k;
            glob_sigma = glob_sigma * k;
            current_sigma = sqrt(new_sigma * new_sigma - old_sigma * old_sigma);
            old_sigma = new_sigma;
            octave_iterator->levels.emplace_back(new_sigma, glob_sigma, GaussianFilterSeparable(level.image, current_sigma));
        }

        if(octave_iterator->index != (number_of_octaves - 1)){
            auto level = octave_iterator->levels.end() - 1 - long(levels_for_overlap);
            glob_sigma = level->effective_sigma;
            (octave_iterator + 1)->levels.emplace_back(sigma_0, glob_sigma, level->image.getDownscale());
        }
    }

    return gauss_pyramid;
}

double getL(const Pyramid& pyramid, int y, int x, double sigma){
    auto result_level = pyramid.front().levels.front();
    int octave_index = pyramid.front().index;

    for(const auto& octave : pyramid)
        for(const auto& level : octave.levels)
            if(level.effective_sigma >= sigma){
                result_level = level;
                octave_index = octave.index;
                goto stop;
            }
stop:
    auto row = y / pow(2.0, octave_index);
    auto col = x / pow(2.0, octave_index);

    return result_level.image.get(row, col);
}

void savePyramid(const Pyramid& pyramid, const std::string& file){
    for(const auto& octave : pyramid)
        for(const auto& level : octave.levels){
            auto name = file + std::to_string(octave.index) + "_" +
                    std::to_string(level.effective_sigma) + "_" +
                    std::to_string(level.current_sigma) + ".png";
            toQImage(level.image).save(name.c_str());
        }
}

Pyramid getDoG(const Pyramid& pyramid){
    Pyramid dog(pyramid.size());
    size_t octave_index = 0;
    for(const auto& octave : pyramid){
        auto& dog_octave = dog[octave_index++];
        dog_octave.index = octave.index;
        for(size_t i = 0; i < octave.levels.size() - 1; i++){
            const auto& first_level = octave.levels[i];
            const auto& second_level = octave.levels[i + 1];
            dog_octave.levels.emplace_back(first_level.current_sigma, first_level.effective_sigma,
                                           (second_level.image - first_level.image) / (exp2(1.0 / octave.levels.size()) - 1));
        }
    }

    return dog;
}

}

