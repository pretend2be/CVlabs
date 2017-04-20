#pragma once

namespace cv {

const double kernelSobelX[] = {
    -1, 0, 1,
    -2, 0, 2,
    -1, 0, 1
};

const double kernelSobelY[] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1
};

const double kernelBlur[] = {
    1.0/9.0, 1.0/9.0, 1.0/9.0,
    1.0/9.0, 1.0/9.0, 1.0/9.0,
    1.0/9.0, 1.0/9.0, 1.0/9.0
};

const double kernelSharpness[] = {
    -1.0/9.0, -1.0/9.0, -1.0/9.0,
    -1.0/9.0, 17.0/9.0, -1.0/9.0,
    -1.0/9.0, -1.0/9.0, -1.0/9.0
};

}
