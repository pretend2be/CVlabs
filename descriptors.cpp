#define _USE_MATH_DEFINES
#include "descriptors.h"
#include "filters.h"

namespace cv {

static double getVectorSize(const Descriptor& descriptor){
    double sum = 0;
    auto size = descriptor.size();
    for(size_t i = 0; i < size; i++)
        sum += descriptor[i] * descriptor[i];
    return sqrt(sum);

}

Descriptors getDescriptors(const Image& image, IPoints& points,
                           int blocks_per_grid_side, int block_size_px, int histogram_size){
    auto sobel_dx = SobelDx(image);
    auto sobel_dy = SobelDy(image);
    auto sobel_grad = SobelGradient(sobel_dx, sobel_dy);
    auto grad_directions = GradientDirections(sobel_dx, sobel_dy);

    int grid_size = blocks_per_grid_side * block_size_px;
    int grid_half = grid_size / 2;

    int wide_histogram_size = 36;
    const auto wide_bin_size = M_PI * 2 / wide_histogram_size;
    Angles angles;
    Angles spare_angles;
    IPoints spare_points;

    for(const auto& point : points){
        std::vector<double> wide_histogram(wide_histogram_size, 0.0);

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++){
                auto u = std::get<0>(point) - grid_half + i;
                auto v = std::get<1>(point) - grid_half + j;

                auto gradient_value = sobel_grad.get(u,v);
                auto gradient_direction = grad_directions.get(u, v);

                auto first_bin_idx = int(gradient_direction / wide_bin_size);
                auto distance_to_bin_center = gradient_direction - first_bin_idx * wide_bin_size - wide_bin_size / 2;
                auto second_bin_idx = distance_to_bin_center > 0 ? first_bin_idx + 1 : first_bin_idx - 1;

                first_bin_idx %= wide_histogram_size;
                second_bin_idx = (second_bin_idx + wide_histogram_size) % wide_histogram_size;

                auto second_percent = fabs(distance_to_bin_center) / wide_bin_size;
                auto first_percent = 1 - second_percent;

                wide_histogram[size_t(first_bin_idx)] += first_percent * gradient_value;
                wide_histogram[size_t(second_bin_idx)] += second_percent * gradient_value;
            }

        size_t first_max_idx = 0;
        size_t second_max_idx = 1;
        if(wide_histogram[first_max_idx] < wide_histogram[second_max_idx])
            std::swap(first_max_idx, second_max_idx);

        for(size_t i = 2; i < wide_histogram_size; i++){
            if(wide_histogram[i] > wide_histogram[first_max_idx]){
                second_max_idx = first_max_idx;
                first_max_idx = i;
            } else if(wide_histogram[i] > wide_histogram[second_max_idx])
                second_max_idx = i;
        }

        auto first_angle = first_max_idx * wide_bin_size;
        auto second_angle = second_max_idx * wide_bin_size;

        angles.emplace_back(first_angle);

        if(first_angle * 0.8 <= second_angle){
            spare_angles.emplace_back(second_angle);
            spare_points.emplace_back(point);
        }
    }

    points.insert(points.end(), spare_points.begin(), spare_points.end());
    angles.insert(angles.end(), spare_angles.begin(), spare_angles.end());


    int histogram_number = blocks_per_grid_side * blocks_per_grid_side;
    int descriptor_value_number = histogram_number * histogram_size;
    const auto bin_size = M_PI * 2 / histogram_size;
    auto points_size = points.size();

    Descriptors descriptors;
    descriptors.reserve(points_size);
    size_t angle_index = 0;

    for(const auto& point : points){

        descriptors.emplace_back(descriptor_value_number, 0);
        auto& descriptor = descriptors.back();

        auto angle = angles[angle_index++];

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++){
                auto u = std::get<0>(point) - grid_half + i;
                auto v = std::get<1>(point) - grid_half + j;

                auto gradient_value = sobel_grad.get(u,v);
                auto gradient_direction = grad_directions.get(u, v) - angle;
                if(gradient_direction < 0) gradient_direction += M_PI * 2;

                auto first_bin_idx = int(gradient_direction / bin_size);
                auto distance_to_bin_center = gradient_direction - first_bin_idx * bin_size - bin_size / 2;
                auto second_bin_idx = distance_to_bin_center > 0 ? first_bin_idx + 1 : first_bin_idx - 1;

                first_bin_idx %= histogram_size;
                second_bin_idx = (second_bin_idx + histogram_size) % histogram_size;

                auto new_i = int((i - grid_half) * cos(angle) - (j - grid_half) * sin(angle));
                auto new_j = int((i - grid_half) * cos(angle) + (j - grid_half) * sin(angle));

                new_i += grid_half;
                new_j += grid_half;

                if(new_i >= 0 && new_i < grid_size && new_j >= 0 && new_j < grid_size){

                auto histogram_start_idx = (new_i / grid_size + new_j / block_size_px) * histogram_size;

                auto second_percent = fabs(distance_to_bin_center) / bin_size;
                auto first_percent = 1 - second_percent;

                descriptor[size_t(histogram_start_idx + first_bin_idx)] += first_percent * gradient_value;
                descriptor[size_t(histogram_start_idx + second_bin_idx)] += second_percent * gradient_value;
                }
            }
    }

    for(auto& descriptor : descriptors){
        auto vector_size = getVectorSize(descriptor);
        for(size_t i = 0; i < descriptor_value_number; i++){
            descriptor[i] /= vector_size;

            if(descriptor[i] > 0.2)
                descriptor[i] = 0.2;
        }

        vector_size = getVectorSize(descriptor);
        for(size_t i = 0; i < descriptor_value_number; i++){
            descriptor[i] /= vector_size;
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

static size_t getNearestIndex(const Descriptor& descriptor, const Descriptors& descriptors){
    size_t min_index = 0;
    auto min_distance = getDistance(descriptor, descriptors[min_index]);

    for(size_t i = 1, desc_size = descriptors.size(); i < desc_size; i++){
        auto distance = getDistance(descriptor, descriptors[i]);
        if(distance < min_distance){
            min_index = i;
            min_distance = distance;
        }
    }

    return min_index;
}

static std::pair<std::pair<size_t, size_t>, std::pair<double, double>>
twoNearest(const Descriptor& descriptor, const Descriptors& descriptors){

    auto min_indexes = std::make_pair(size_t(0), size_t(1));
    auto min_distances = std::make_pair(getDistance(descriptor, descriptors[min_indexes.first]),
            getDistance(descriptor, descriptors[min_indexes.second]));

    if(min_distances.first > min_distances.second){
        std::swap(min_indexes.first, min_indexes.second);
        std::swap(min_distances.first, min_distances.second);
    }

    for(size_t i = 2, desc_size = descriptors.size(); i < desc_size; i++){
        auto distance = getDistance(descriptor, descriptors[i]);
        if(distance < min_distances.first){
            min_distances.second = min_distances.first;
            min_distances.first = distance;
            min_indexes.second = min_indexes.first;
            min_indexes.first = i;
        } else if(distance < min_distances.second){
            min_distances.second = distance;
            min_indexes.second = i;
        }
    }

    auto k = std::make_pair(min_indexes, min_distances);

    return k;
}

Matches getMatches(const Descriptors& first, const Descriptors& second){

    Matches matches;

    for(size_t i = 0, size = first.size(); i < size; i++){
        auto two_Nearest = twoNearest(first[i], second);
        auto nearest_indexes = two_Nearest.first;
        auto nearest_distances = two_Nearest.second;

        auto first_nearest_index = nearest_indexes.first;
        auto second_nearest_index = getNearestIndex(second[first_nearest_index], first);

        if(second_nearest_index == i && nearest_distances.first / nearest_distances.second < 0.8)
            matches.emplace_back(i, first_nearest_index);
    }

    return matches;
}

}
