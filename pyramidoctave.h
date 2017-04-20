#pragma once
#include "pyramidlvl.h"

namespace cv {

class PyramidOctave
{
public:
    PyramidOctave(const int octave);
    int getOctaveNumber();
    void addLevel(PyramidLvl& lvl);
    int NumberOfLevels() const;
    Image getLevelImage(int level) const;
    PyramidLvl getLevel(int level) const;

private:
    std::vector<PyramidLvl> levels;
    int Octave;
};

}
