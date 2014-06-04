#include "fliprecord.h"

FlipRecord::FlipRecord()
{
}

void FlipRecord::save(VHandle vh, VHandle vh_oppo, HHandle hh, TriMesh& mesh)
{
    auto p = mesh.point(vh);
    auto p_oppo = mesh.point(vh_oppo);
    auto s = mesh.point(mesh.from_vertex_handle(hh));
    auto t = mesh.point(mesh.to_vertex_handle(hh));
    boundry_points[0] = glm::vec3(p[0], p[1], p[2]);
    boundry_points[1] = glm::vec3(s[0], s[1], s[2]);
    boundry_points[2] = glm::vec3(p_oppo[0], p_oppo[1], p_oppo[2]);
    boundry_points[3] = glm::vec3(t[0], t[1], t[2]);
}