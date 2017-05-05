#define _USE_MATH_DEFINES
#include "descriptors.h"
#include "filters.h"

namespace cv {

static double getVectorLength(const Descriptor& descriptor){
    double sum = 0;
    auto size = descriptor.size();
    for(size_t i = 0; i < size; i++)
        sum += descriptor[i] * descriptor[i];
    return sqrt(sum);
}

static int mod(int x, int mod){
    return (x % mod + mod) % mod;
}

static Descriptors Normalize(Descriptors& descriptors){
    size_t size = descriptors[0].size();
    for(auto& descriptor : descriptors){
        auto vector_length = getVectorLength(descriptor);
        for(size_t i = 0; i < size; i++){
            descriptor[i] /= vector_length;

            if(descriptor[i] > 0.2)
                descriptor[i] = 0.2;
        }

        vector_length = getVectorLength(descriptor);
        for(size_t i = 0; i < size; i++){
            descriptor[i] /= vector_length;
        }
    }

    return descriptors;
}

static double specifyAngle(double x0, double y0, double x1, double y1, double x2, double y2) {
    auto a = (y2 - y0) / ((x2 - x0) * (x2 - x1)) - (y1 - y0) / ((x1 - x0) * (x2 - x1));
    auto b = (y1 - y0) / (x1 - x0) - a * (x1 + x0);
    return -b / (2 * a);
}

std::tuple<Descriptors, Blobs> getDescriptors(const Image& _image, int _grid_size, int histogram_size){
    int min_size = 16;
    size_t number_of_levels = 8;
    double sigma_a = 0.5;
    double sigma_0 = 1.6;
    auto number_of_octaves = size_t(std::min(std::log2(double(_image.getHeight()) / min_size),
                                             std::log2(double(_image.getWidth()) / min_size))) +1;

    auto gauss_pyramid = getGaussPyramid(_image, sigma_0, sigma_a, number_of_levels, number_of_octaves);
    auto blobs = getBlobs(gauss_pyramid);
    std::cout << blobs.size() << std::endl;

    int wide_histogram_size = 36;
    std::vector<double> wide_histogram(wide_histogram_size);
    const auto wide_bin_size = M_PI * 2 / wide_histogram_size;

    auto get_final_angle = [&](int max_index) {
        auto angle_center = max_index * wide_bin_size + wide_bin_size / 2;
        auto angle_left = angle_center - wide_bin_size;
        auto angle_right = angle_center + wide_bin_size;
        auto y_center = wide_histogram[size_t(max_index)];
        auto y_left = wide_histogram[size_t(mod(max_index - 1, wide_histogram_size))];
        auto y_right = wide_histogram[size_t(mod(max_index + 1, wide_histogram_size))];
        if (y_center >= y_left && y_center >= y_right) {
            return specifyAngle(angle_center, y_center, angle_left, y_left, angle_right, y_right);
        }
        return angle_center;
    };

    Angles angles;
    Angles spare_angles;
    Blobs spare_blobs;

    for(const auto& blob : blobs){
        std::fill(wide_histogram.begin(), wide_histogram.end(), 0.0);

        const auto& image = blob.level->image;
        auto grid_size = int(_grid_size * blob.level->current_sigma / sigma_0);
        auto grid_half = grid_size / 2;

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++){
                auto u = blob.x / blob.scale - grid_half + i;
                auto v = blob.y / blob.scale - grid_half + j;

                auto dx = getDx(image, u, v);
                auto dy = getDy(image, u, v);

                auto gradient_value = getGradientValue(dx, dy);
                auto gradient_direction = getGradientDirection(dx, dy);

                auto first_bin_idx = int(gradient_direction / wide_bin_size);
                auto distance_to_bin_center = gradient_direction - first_bin_idx * wide_bin_size - wide_bin_size / 2;
                auto second_bin_idx = distance_to_bin_center > 0 ? first_bin_idx + 1 : first_bin_idx - 1;

                first_bin_idx = mod(first_bin_idx, wide_histogram_size);
                second_bin_idx = mod(second_bin_idx, wide_histogram_size);

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

        auto first_angle = get_final_angle(first_max_idx);
        auto second_angle = get_final_angle(second_max_idx);

        angles.emplace_back(first_angle);

        if(wide_histogram[first_max_idx] * 0.8 <= wide_histogram[second_max_idx] &&
                fabs(first_angle - second_angle) > wide_bin_size){
            spare_angles.emplace_back(second_angle);
            spare_blobs.emplace_back(blob);
        }
    }

    blobs.insert(blobs.end(), spare_blobs.begin(), spare_blobs.end());
    angles.insert(angles.end(), spare_angles.begin(), spare_angles.end());
    std::cout << blobs.size() << std::endl;

    int histogram_number = 16;
    int descriptor_value_number = histogram_number * histogram_size;
    const auto bin_size = M_PI * 2 / histogram_size;
    size_t angle_index = 0;

    Descriptors descriptors;
    descriptors.reserve(blobs.size());

    for(const auto& blob : blobs){
        const auto& image = blob.level->image;
        auto grid_size = int(_grid_size * blob.level->current_sigma / sigma_0);
        auto grid_half = grid_size / 2;
        auto block_size = grid_size / 4.0;

        descriptors.emplace_back(descriptor_value_number, 0);
        auto& descriptor = descriptors.back();
        const auto angle = angles[angle_index++];

        for(int i = 0; i < grid_size; i++)
            for(int j = 0; j < grid_size; j++){
                auto u = blob.x / blob.scale - grid_half + i;
                auto v = blob.y / blob.scale - grid_half + j;

                auto dx = getDx(image, u, v);
                auto dy = getDy(image, u, v);
                auto gradient_value = getGradientValue(dx, dy);
                auto gradient_direction = getGradientDirection(dx, dy) - angle;

                auto new_i = int((i - grid_half) * cos(angle) - (j - grid_half) * sin(angle));
                auto new_j = int((i - grid_half) * sin(angle) + (j - grid_half) * cos(angle));

                new_i += grid_half;
                new_j += grid_half;

                if(new_i >= 0 && new_i < grid_size && new_j >= 0 && new_j < grid_size){
                    auto histogram_x = std::min(int(round(new_i / block_size)), 3);
                    auto histogram_y = std::min(int(round(new_j / block_size)), 3);
                    auto histogram_angle = int(round(gradient_direction / bin_size));

                    for(int x = histogram_x - 1; x <= histogram_x; x++){
                        if(x >= 0){
                            auto distance_to_cetner_x = fabs(new_i - (block_size * x + block_size / 2));
                            auto percent_x = 1 - distance_to_cetner_x / block_size;
                            for(int y = histogram_y - 1; y <= histogram_y; y++){
                                if(y >= 0){
                                    auto distance_to_cetner_y = fabs(new_j - (block_size * y + block_size / 2));
                                    auto percent_y = 1 - distance_to_cetner_y / block_size;
                                    for(int a = histogram_angle - 1; a <= histogram_angle; a++){
                                        auto bin_index = mod(a, histogram_size);
                                        auto distance_to_center_bin = fabs(gradient_direction -
                                                                           ( bin_size * a + bin_size / 2));
                                        auto percent_angle = 1 - distance_to_center_bin / bin_size;
                                        auto index = (4 * x + y) * histogram_size + bin_index;
                                        descriptor[size_t(index)] += gradient_value * percent_x * percent_y * percent_angle;
                                    }
                                }
                            }
                        }
                    }
                }
            }
    }

    descriptors = Normalize(descriptors);

    return std::make_tuple(descriptors, blobs);
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
                second_bin_idx = mod(second_bin_idx, wide_histogram_size);

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

                auto first_bin_idx = int(gradient_direction / bin_size);
                auto distance_to_bin_center = gradient_direction - first_bin_idx * bin_size - bin_size / 2;
                auto second_bin_idx = distance_to_bin_center > 0 ? first_bin_idx + 1 : first_bin_idx - 1;

                first_bin_idx = mod(first_bin_idx, histogram_size);
                second_bin_idx = mod(second_bin_idx, histogram_size);

                auto new_i = int((i - grid_half) * cos(angle) - (j - grid_half) * sin(angle));
                auto new_j = int((i - grid_half) * sin(angle) + (j - grid_half) * cos(angle));

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

    descriptors = Normalize(descriptors);

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

const bool isExtremum(const Octave& octave, size_t _k, int _i, int _j){
    auto center = octave.levels[_k].image.get(_i, _j);
    if (fabs(center) < 0.035) {
        return false;
    }

    std::vector<double> intensities(26);
    size_t index = 0;

    for(size_t k = _k -1; k <= _k + 1; k++)
        for(int i = _i -1; i <= _i + 1; i++)
            for(int j = _j -1; j <= _j + 1; j++)
                if(k != _k || i != _i || j != _j)
                    intensities[index++] = octave.levels[k].image.get(i, j);

    auto minmax = std::minmax_element(intensities.begin(), intensities.end());
    return center < *minmax.first || center > *minmax.second;
}

Blobs getBlobs(const Pyramid &pyramid){
    auto dog = getDoG(pyramid);
    double R = 10.0;

    Blobs blobs;
    int scale = 1;
    size_t octave_index = 0;
    for(const auto& octave : dog){
        for(size_t k = 1, levels_size = octave.levels.size() -1; k < levels_size; k++){
            const auto& level = octave.levels[k];
            for(int i = 0, H = level.image.getHeight(); i < H; i++)
                for(int j = 0, W = level.image.getWidth(); j < W; j++)
                    if(isExtremum(octave, k, i, j)) {
                        auto a = getDx2(level.image, i, j);
                        auto b = getDxDy(level.image, i, j);
                        auto c = getDy2(level.image, i, j);
                        auto det = a * c - b * b;
                        auto trace = a + c;
                        if( (trace * trace) / det <= (R + 1) * (R + 1) / R){
                            const auto& gauss_level = pyramid[octave_index].levels[k];
                            blobs.emplace_back(round((i + 0.5) * scale - 0.5), round((j + 0.5) * scale - 0.5),
                                               level.effective_sigma, &gauss_level, scale);
                        }
                    }
        }
        scale *= 2;
        octave_index++;
    }

    return blobs;
}

}
