#include "dviewer.h"
#include <math.h>
#include <QKeyEvent>
#include <QDebug>

using namespace qglviewer;
using namespace std;

//const int half_window_width = 300;
//const int half_window_height = 340;
const int scale_factor = 400;
const float lift_up_distance = 0.15f;

DViewer::DViewer(QWidget *parent)
    : QGLViewer(parent), isDrawResult(false), isShowParaboloid(true)
{

}

DViewer::DViewer(DelaunayBase* delaunay_, int mainwindow_width, int mainwindow_height)
    : delaunay(delaunay_), half_window_width(mainwindow_width / 4), half_window_height(mainwindow_height / 2) { }

void DViewer::toggleShowParaboloid()
{
    isShowParaboloid = !isShowParaboloid;
    update();
}

void DViewer::setParam(DelaunayBase* delaunay_, int mainwindow_width, int mainwindow_height)
{
    delaunay = delaunay_;
    half_window_width = mainwindow_width / 4;
    half_window_height = mainwindow_height / 2;
}

//void DViewer::resizeGL(int width, int height)
//{
//    half_window_width = width;
//    half_window_height = height;
//}

void DViewer::drawBeforeFlip()
{
	glColor3f(1, 0, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(1.0, 1.0);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glBegin(GL_TRIANGLES);
	glVertex3fv(&flip_space_points[0][0]);
	glVertex3fv(&flip_space_points[1][0]);
	glVertex3fv(&flip_space_points[3][0]);

	glVertex3fv(&flip_space_points[1][0]);
	glVertex3fv(&flip_space_points[2][0]);
	glVertex3fv(&flip_space_points[3][0]);
	glEnd();

	glDisable(GL_LIGHTING);
	glColor3f(0.3, 0.3, 0.3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.0f);
	glBegin(GL_TRIANGLES);
	glVertex3fv(&flip_space_points[0][0]);
	glVertex3fv(&flip_space_points[1][0]);
	glVertex3fv(&flip_space_points[3][0]);

	glVertex3fv(&flip_space_points[1][0]);
	glVertex3fv(&flip_space_points[2][0]);
	glVertex3fv(&flip_space_points[3][0]);
	glEnd();
	glEnable(GL_LIGHTING);
}

void DViewer::drawAfterFlip()
{
	glColor3f(1, 1, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(1.0, 1.0);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glBegin(GL_TRIANGLES);
	glVertex3fv(&flip_space_points[0][0]);
	glVertex3fv(&flip_space_points[1][0]);
	glVertex3fv(&flip_space_points[2][0]);

	glVertex3fv(&flip_space_points[0][0]);
	glVertex3fv(&flip_space_points[2][0]);
	glVertex3fv(&flip_space_points[3][0]);
	glEnd();
	glDisable(GL_POLYGON_OFFSET_FILL);

	glDisable(GL_LIGHTING);
	glColor3f(0.3, 0.3, 0.3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.0f);
	glBegin(GL_TRIANGLES);
	glVertex3fv(&flip_space_points[0][0]);
	glVertex3fv(&flip_space_points[1][0]);
	glVertex3fv(&flip_space_points[2][0]);

	glVertex3fv(&flip_space_points[0][0]);
	glVertex3fv(&flip_space_points[2][0]);
	glVertex3fv(&flip_space_points[3][0]);
	glEnd();
	glEnable(GL_LIGHTING);

}

void DViewer::showBeforeFlip3D()
{
    isDrawBeforeFlip = true;
    update();
}

void DViewer::showAfterFlip3D()
{
    isDrawAfterFlip = true;
    isDrawBeforeFlip = false;
    update();
}

void DViewer::clearAfterFlip3D()
{
    isDrawAfterFlip = false;
    update();
}

void DViewer::showResult3D()
{
    isDrawResult = true;
    update();
}

void DViewer::initFlipDemoParams(std::array<Point, 4>& flip)
{
    for(int i = 0; i < 4; i++)
    {
        float x = (flip[i][0] - half_window_width) / scale_factor;
        float y = (half_window_height - flip[i][1]) / scale_factor;
        flip_space_points[i] = Point(x, y, x * x + y * y + lift_up_distance);
    }
}

void DViewer::drawMesh()
{
    // points
    glPointSize(4.0);
    glColor3f(1, 0, 0);
    glBegin(GL_POINTS);
    for (auto& vh: delaunay->mesh.vertices())
    {
        auto point = delaunay->mesh.point(vh);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f((point[0] - half_window_width) / scale_factor, (half_window_height - point[1]) / scale_factor, 0);
    }
    glEnd();

    // planar triangles
    glColor3f(0, 0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for (auto& fh : delaunay->mesh.faces())
    {
        if (delaunay->hasInfinitePoint(fh))
        {
            continue;
        }

        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            auto point = delaunay->mesh.point(vh);
            float x = (point[0] - half_window_width) / scale_factor;
            float y = (half_window_height - point[1]) / scale_factor;
            glVertex3f(x, y, 0);
            glNormal3f(0, 0, 1);
        }
    }
    glEnd();

    // space triangles fill
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(0, 1, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (auto& fh : delaunay->mesh.faces())
    {
        if (delaunay->hasInfinitePoint(fh))
        {
            continue;
        }

        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            auto point = delaunay->mesh.point(vh);
            float x = (point[0] - half_window_width) / scale_factor;
            float y = (half_window_height - point[1]) / scale_factor;
            glVertex3f(x, y, x * x + y * y + lift_up_distance);
            glNormal3f(2 * x, 2 * y, -1);
        }
    }
    glEnd();

    // space triangles wireframe
    glColor3f(0, 0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.0f);
    glBegin(GL_TRIANGLES);
    for (auto& fh : delaunay->mesh.faces())
    {
        if (delaunay->hasInfinitePoint(fh))
        {
            continue;
        }

        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            auto point = delaunay->mesh.point(vh);
            float x = (point[0] - half_window_width) / scale_factor;
            float y = (half_window_height - point[1]) / scale_factor;
            glVertex3f(x, y, x * x + y * y + lift_up_distance + 0.001);
            glNormal3f(0, 0, 1); // ???
        }
    }
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DViewer::init()
{
#ifdef GL_RESCALE_NORMAL  // OpenGL 1.2 Only...
    glEnable(GL_RESCALE_NORMAL);
#endif

    //setBackgroundColor(QColor(172, 193, 236));
    setBackgroundColor(QColor(240, 240, 240));

    setMouseBinding(Qt::NoModifier, Qt::LeftButton, CAMERA, TRANSLATE);
    setMouseBinding(Qt::NoModifier, Qt::RightButton, CAMERA, ROTATE);

    // Make sure the manipulatedFrame is not easily clipped by the zNear and zFar planes
    setSceneRadius(2);
    camera()->fitSphere(Vec(0, 0, 0), 1);

    //help();
    //restoreStateFromFile();

    // auto-normalize
    glEnable(GL_NORMALIZE);

    glEnable(GL_BLEND);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

//    // 全局光照
//    GLfloat globel_ambient[] = { 0.0 , 0.0 , 0.0 , 1.0 };
//    glLightModelfv(GL_LIGHT_MODEL_AMBIENT , globel_ambient);

//    // 镜面反射
//    GLfloat planet_specular[] = { 0.5 , 0.5 , 0.5 , 0.5 };
//    glMaterialfv(GL_FRONT , GL_SPECULAR ,planet_specular);


    //glDisable(GL_DEPTH_TEST);

    // create display list
    paraboloidListId = glGenLists(1);
    glNewList(paraboloidListId, GL_COMPILE);
    drawParaboloid(Point(0.0f, 0.0f, lift_up_distance), 50.0f, 50.0f);
    glEndList();


}

void DViewer::draw()
{
    // Here we are in the world coordinate system. Draw unit size axis.
    // drawAxis();


    //// Save the current model view matrix (not needed here in fact)
    //glPushMatrix();
    //// Multiply matrix to get in the frame coordinate system.
    //glMultMatrixd(manipulatedFrame()->matrix());
    // Scale down the drawings
    //glScalef(5.0f, 5.0f, 5.0f);
    // Draw an axis using the QGLViewer static function
    //drawAxis();

    //drawPoints();

    if (isDrawBeforeFlip)
        drawBeforeFlip();

    if (isDrawAfterFlip)
        drawAfterFlip();


    if (isDrawResult)
    {
        glDepthMask(GL_FALSE);
        drawMesh();
        glDepthMask(GL_TRUE);
    }

    if (isShowParaboloid)
    {
        // make depth buffer read-only
        glDepthMask(GL_FALSE);
        // draw paraboloid
        glCallList(paraboloidListId);
        glDepthMask(GL_TRUE);
    }

    // Restore the original (world) coordinate system
    //glPopMatrix();
}

inline float getx(float u, float v)
{
    return u * cos(v);
}

inline float gety(float u, float v)
{
    return u * sin(v);
}

// bottom_point: the lowest point of the paraboloid
// slice , stack: control the number of triangles 
void DViewer::drawParaboloid(Point bottom_point, float slice, float stack)
{
	const float step_v = (float)(M_PI / slice);
	const float step_u = 1.0 / stack;

	glEnable(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA /*GL_ONE*/);

	glColor4f(0.0f, 0.6f, 1.0f, 0.5f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLE_STRIP);
	for (float u = 0; u < 1; u += step_u)
	{
		for (float v = 0; v < 2 * M_PI + step_v; v += step_v)
		{
			float x = getx(u, v);
			float y = gety(u, v);
			Point N = Point(x, y, -0.5);
			glNormal3f(N[0], N[1], N[2]);
			glVertex3f(x + bottom_point[0], y + bottom_point[1],
				u * u + bottom_point[2]);

			x = getx(u + step_u, v);
			y = gety(u + step_u, v);
			N = Point(x, y, -0.5);
			glNormal3f(N[0], N[1], N[2]);
			glVertex3f(x + bottom_point[0], y + bottom_point[1],
				(u + step_u) * (u + step_u) + bottom_point[2]);

		}
	}
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_POLYGON_SMOOTH);
}
