#pragma once

namespace cv {

const double kernelSobelX[] = {
    -1.0, 0.0, 1.0,
    -2.0, 0.0, 2.0,
    -1.0, 0.0, 1.0
};

const double kernelSobelY[] = {
    -1.0, -2.0, -1.0,
     0.0,  0.0,  0.0,
     1.0,  2.0,  1.0
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
