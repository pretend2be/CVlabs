#define _USE_MATH_DEFINES
#include "descriptors.h"
#include "filters.h"

namespace cv {

Descriptors getDescriptors(const Image& image, const IPoints& points,
                           int descriptor_size, int block_size, int histogram_value_number){
    auto sobel_dx = SobelDx(image);
    auto sobel_dy = SobelDy(image);
    auto sobel_grad = SobelGradient(sobel_dx, sobel_dy);
    auto grad_directions = GradientDirections(sobel_dx, sobel_dy);

    int grid_size = descriptor_size * block_size;
    int grid_half = grid_size / 2;
    int histogram_number = descriptor_size * descriptor_size;
    int descriptor_value_number = histogram_number * histogram_value_number;

    Descriptors descriptors;

    const auto z = M_PI * 2 / histogram_value_number;
    auto size = points.size();

    for(int k = 0; k < size; k++){

        descriptors.emplace_back(descriptor_value_number, 0);
        auto& descriptor = descriptors.back();

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++){
                auto u = std::get<0>(points[k]) - grid_half + i;
                auto v = std::get<1>(points[k]) - grid_half + j;

                auto gradient_value = sobel_grad.get(u,v);
                auto gradient_direction = grad_directions.get(u, v);

                auto first_bin_idx = int(gradient_direction / z);
                auto distance_to_bin_center = gradient_direction - first_bin_idx * z + z / 2;
                auto second_bin_idx = distance_to_bin_center > 0 ? first_bin_idx + 1 : first_bin_idx - 1;

                first_bin_idx %= histogram_value_number;
                second_bin_idx %= histogram_value_number;

                auto histogram_start_idx = (i / grid_size * j / block_size) * histogram_value_number;

                auto second_percent = fabs(distance_to_bin_center) / z;
                auto first_percent = 1 - second_percent;

                descriptor[size_t(histogram_start_idx + first_bin_idx)] += first_percent * gradient_value;
                descriptor[size_t(histogram_start_idx + second_bin_idx)] += second_percent * gradient_value;
            }
    }

    return descriptors;
}

static double getDistance(const Descriptor& first, const Descriptor& second){
    double sum = 0;
    auto size = first.size();

    for(size_t i = 0; i < size; i++){
        sum += pow(first[i] - second[i], 2);
    }
    return sqrt(sum);
}

Matches getMatches(const Descriptors& first, const Descriptors& second){

    Matches matches;
    auto first_size = first.size();
    auto second_size = second.size();

    for(size_t i = 0; i < first_size; i++){
        size_t min_index = 0;
        auto min_distance = getDistance(first[i], second[min_index]);
        for(size_t j = 1; j < second_size; j++){
            auto distance = getDistance(first[i], second[j]);
            if(distance < min_distance){
                min_distance = distance;
                min_index = j;
            }
        }
        if( min_distance < 0.4)
            matches.emplace_back(i, min_index);
    }

    return matches;
}

}
