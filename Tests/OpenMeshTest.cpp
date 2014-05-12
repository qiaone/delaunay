#include <QtTest/QtTest>
#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

class OpenMeshTest: public QObject
{
    Q_OBJECT

private slots:
    void CubeTest()
    {
        typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
        MyMesh mesh;

        // generate vertices
        MyMesh::VertexHandle vhandle[8];
        vhandle[0] = mesh.add_vertex(MyMesh::Point(-1, -1,  1));
        vhandle[1] = mesh.add_vertex(MyMesh::Point( 1, -1,  1));
        vhandle[2] = mesh.add_vertex(MyMesh::Point( 1,  1,  1));
        vhandle[3] = mesh.add_vertex(MyMesh::Point(-1,  1,  1));
        vhandle[4] = mesh.add_vertex(MyMesh::Point(-1, -1, -1));
        vhandle[5] = mesh.add_vertex(MyMesh::Point( 1, -1, -1));
        vhandle[6] = mesh.add_vertex(MyMesh::Point( 1,  1, -1));
        vhandle[7] = mesh.add_vertex(MyMesh::Point(-1,  1, -1));

        // generate (quadrilateral) faces
        std::vector<MyMesh::VertexHandle>  face_vhandles;
        face_vhandles.clear();
        face_vhandles.push_back(vhandle[0]);
        face_vhandles.push_back(vhandle[1]);
        face_vhandles.push_back(vhandle[2]);
        face_vhandles.push_back(vhandle[3]);
        mesh.add_face(face_vhandles);

        face_vhandles.clear();
        face_vhandles.push_back(vhandle[7]);
        face_vhandles.push_back(vhandle[6]);
        face_vhandles.push_back(vhandle[5]);
        face_vhandles.push_back(vhandle[4]);
        mesh.add_face(face_vhandles);
        face_vhandles.clear();
        face_vhandles.push_back(vhandle[1]);
        face_vhandles.push_back(vhandle[0]);
        face_vhandles.push_back(vhandle[4]);
        face_vhandles.push_back(vhandle[5]);
        mesh.add_face(face_vhandles);
        face_vhandles.clear();
        face_vhandles.push_back(vhandle[2]);
        face_vhandles.push_back(vhandle[1]);
        face_vhandles.push_back(vhandle[5]);
        face_vhandles.push_back(vhandle[6]);
        mesh.add_face(face_vhandles);
        face_vhandles.clear();
        face_vhandles.push_back(vhandle[3]);
        face_vhandles.push_back(vhandle[2]);
        face_vhandles.push_back(vhandle[6]);
        face_vhandles.push_back(vhandle[7]);
        mesh.add_face(face_vhandles);
        face_vhandles.clear();
        face_vhandles.push_back(vhandle[0]);
        face_vhandles.push_back(vhandle[3]);
        face_vhandles.push_back(vhandle[7]);
        face_vhandles.push_back(vhandle[4]);
        mesh.add_face(face_vhandles);

        QVERIFY(mesh.IsPolyMesh == 0);
    }
};

QTEST_MAIN(OpenMeshTest)
#include "OpenMeshTest.moc"
