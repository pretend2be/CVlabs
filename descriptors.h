#pragma once

#include <vector>
#include "image.h"
#include "poi.h"

namespace cv {

using Descriptor = std::vector<double>;
using Descriptors = std::vector<Descriptor>;
using Matches = std::vector<std::pair<int, int>>;

Descriptors getDescriptors(const Image& image, const IPoints& points,
                           int descriptor_size, int block_size, int histogram_value_number);

Matches getMatches(const Descriptors& first, const Descriptors& second);

}
