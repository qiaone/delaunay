#include "triangulation.h"

namespace Delaunay
{

Triangulation::Triangulation(VecPt & all_points)
{
    for(auto & point : all_points)
        mesh.add_vertex(point);
}

void Triangulation::perform()
{

}

}
