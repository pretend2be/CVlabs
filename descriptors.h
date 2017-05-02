#pragma once

#include <vector>
#include "image.h"
#include "poi.h"

namespace cv {

using Descriptor = std::vector<double>;
using Descriptors = std::vector<Descriptor>;
using Matches = std::vector<std::pair<int, int>>;
using Angles = std::vector<double>;

Descriptors getDescriptors(const Image& image, IPoints &points,
                           int descriptor_size, int block_size, int histogram_size);

Matches getMatches(const Descriptors& first, const Descriptors& second);

}
