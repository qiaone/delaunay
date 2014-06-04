#ifndef FLIPRECORD_H
#define FLIPRECORD_H

#include "def.h"
#include <array>
#include <glm/glm.hpp>

class FlipRecord
{
public:
    FlipRecord();
    void save(VHandle vh, VHandle vh_oppo, HHandle hh, TriMesh& mesh);
    std::array<glm::vec3, 4> boundry_points;
};

#endif // FLIPRECORD_H
