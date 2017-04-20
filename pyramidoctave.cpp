#include "pyramidoctave.h"

namespace cv {

PyramidOctave::PyramidOctave(int octave)
    : Octave(octave)
{}

int PyramidOctave::getOctaveNumber(){
    return Octave;
}

void PyramidOctave::addLevel(PyramidLvl& lvl){
    levels.push_back(lvl);
}

int PyramidOctave::NumberOfLevels() const{
    return levels.size();
}

Image PyramidOctave::getLevelImage(int level) const{
    return levels[level].getImage();
}

PyramidLvl PyramidOctave::getLevel(int level) const{
    return levels[level];
}

}


