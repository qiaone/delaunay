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
	FunType funType;
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

	float calcZ(float x, float y, FunType fType)
	{
		float z = 0;
		switch (fType)
		{
		case CIRCLE:
			z = circle(x, y);
			break;
		case ELLIPSE:
			z = ellipse(x, y);
			break;
		case NORM2:
			z = norm2(x, y);
			break;
		default:
			break;
		}
		return z;
	}

	float calcZ(float x, float y)
	{
		return calcZ(x, y, funType);
	}

	Point calcN(float x, float y, FunType fType)
	{
		Point N(0, 0, 0);
		switch (fType)
		{
		case CIRCLE:
			N = nCircle(x, y);
			break;
		case ELLIPSE:
			N = nEllipse(x, y);
			break;
		case NORM2:
			N = nNorm2(x, y);
			break;
		default:
			break;
		}
		return N;
	}

	Point calcN(float x, float y)
	{
		return calcN(x, y, funType);
	}
	
	float circle(float x, float y)
	{
		return x*x + y*y;
	}

	Point nCircle(float x, float y)
	{
		return Point(2.0 * x, 2.0 * y, -1.0);
	}

	float ellipse(float x, float y)
	{
		return x*x + 0.1*y*y;
	}

	Point nEllipse(float x, float y)
	{
		return Point(2.0*x, 0.2*y, -1.0);
	}

	float norm2(float x, float y)
	{
		return sqrt(x*x + y*y);
	}

	Point nNorm2(float x, float y)
	{
		float t = sqrt(x*x + y*y);
		if (t < ESP) t = ESP;
		return Point(x / t, y / t, -1);
	}


};

#endif // DELAUNAYBASE_H
