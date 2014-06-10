#ifndef DELAUNAYBASE_H
#define DELAUNAYBASE_H

#include <QObject>
#include <array>
#include "def.h"

class DelaunayBase : public QObject
{
    Q_OBJECT
public:
    explicit DelaunayBase(QObject *parent = 0);
    TriMesh mesh;
    bool hasInfinitePoint(const FHandle &fh)
    {
        Point a(-INF, -INF, 0);
        Point b(INF, -INF, 0);
        Point c(0, INF, 0);

        for(auto& vh : mesh.fv_range(fh))
        {
            Point p = mesh.point(vh);
            if (p == a || p == b || p == c)
            {
                return true;
            }
        }
        return false;
    }

    bool hasInfinitePoint(const std::array<Point, 4>& points)
    {
        Point a(-INF, -INF, 0);
        Point b(INF, -INF, 0);
        Point c(0, INF, 0);

        for(auto& p : points)
        {
            if (p == a || p == b || p == c)
            {
                return true;
            }
        }
        return false;
    }
};

#endif // DELAUNAYBASE_H
