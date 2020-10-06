#include "pch.h"
#include "factory.h"

VOID bullet_body_create_generic(lua_State* L, btCollisionShape* shape, FLOAT mass, btTransform tr, btVector3 inertia = btVector3(0, 0, 0)) {
    btDefaultMotionState* motion = new btDefaultMotionState(tr);
    btRigidBody::btRigidBodyConstructionInfo ci(mass, motion, shape, inertia);
    btRigidBody* body = new btRigidBody(ci);
    int index = (int)bodies.size();
    shape->setUserIndex(index);
    body->setUserIndex(index);
    bodies.push_back(body);
    world->addRigidBody(body);
    lua_pushinteger(L, index);
}

INT bullet_body_create_plane(lua_State* L) {
    D3DXVECTOR4 origin = *(D3DXVECTOR4*)luaL_checkudata(L, 1, L_VECTOR);
    D3DXVECTOR4 plane = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);

    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(origin.x, origin.y, origin.z));
    btCollisionShape* shape = new btStaticPlaneShape(btVector3(plane.x, plane.y, plane.z), plane.w);
    bullet_body_create_generic(L, shape, 0.0f, tr);
    return 1;
}

INT bullet_body_create_static_cols(lua_State* L) {
    D3DXMATRIX mat = *(D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    CFaceGroup* fg = *(CFaceGroup**)luaL_checkudata(L, 2, L_FACEGROUP);

    btTriangleMesh* mesh = new btTriangleMesh(false, false);

    for (UINT i = 0; i < fg->GetNumVertices(); i += 3)
    {
        VERTEX v0 = *(fg->GetVertices() + i);
        VERTEX v1 = *(fg->GetVertices() + i + 1);
        VERTEX v2 = *(fg->GetVertices() + i + 2);
        btVector3 p0 = btVector3(v0.x, v0.y, v0.z);
        btVector3 p1 = btVector3(v1.x, v1.y, v1.z);
        btVector3 p2 = btVector3(v2.x, v2.y, v2.z);

        mesh->addTriangle(p0, p1, p2);
    }

    for (UINT i = 0; i < fg->GetNumIndices(); i += 3)
    {
        SHORT i0 = *(fg->GetIndices() + i);
        SHORT i1 = *(fg->GetIndices() + i + 1);
        SHORT i2 = *(fg->GetIndices() + i + 2);

        mesh->addTriangleIndices(i0, i1, i2);
    }

    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    btCollisionShape* shape = new btBvhTriangleMeshShape(mesh, true);
    bullet_body_create_generic(L, shape, 0.0f, tr);
    return 1;
}

INT bullet_body_create_static_cols_mesh(lua_State* L) {
    D3DXMATRIX mat = *(D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    CMesh* m = *(CMesh**)luaL_checkudata(L, 2, L_MESH);

    btTriangleMesh* mesh = new btTriangleMesh(false, false);

    SHORT indexOffset = 0;

    for (UINT i = 0; i < m->GetNumFGroups(); i++)
    {
        CFaceGroup* fg = m->GetFGroupData()[i];

        for (UINT i = 0; i < fg->GetNumVertices(); i += 3)
        {
            VERTEX v0 = *(fg->GetVertices() + i);
            VERTEX v1 = *(fg->GetVertices() + i + 1);
            VERTEX v2 = *(fg->GetVertices() + i + 2);
            btVector3 p0 = btVector3(v0.x, v0.y, v0.z);
            btVector3 p1 = btVector3(v1.x, v1.y, v1.z);
            btVector3 p2 = btVector3(v2.x, v2.y, v2.z);

            mesh->addTriangle(p0, p1, p2);
        }

        for (UINT i = 0; i < fg->GetNumIndices(); i += 3)
        {
            SHORT i0 = *(fg->GetIndices() + i);
            SHORT i1 = *(fg->GetIndices() + i + 1);
            SHORT i2 = *(fg->GetIndices() + i + 2);

            mesh->addTriangleIndices(i0, i1, i2);
        }

        indexOffset = mesh->getNumTriangles() / 3;
    }

    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    btCollisionShape* shape = new btBvhTriangleMeshShape(mesh, true);
    bullet_body_create_generic(L, shape, 0.0f, tr);
    return 1;
}

INT bullet_body_create_sphere(lua_State* L) {
    D3DXMATRIX mat = *(D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    FLOAT radius = (FLOAT)luaL_checknumber(L, 2);
    FLOAT mass = (FLOAT)luaL_checknumber(L, 3);

    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    btCollisionShape* shape = new btSphereShape(radius);
    btVector3 localInertia;
    shape->calculateLocalInertia(mass, localInertia);
    bullet_body_create_generic(L, shape, mass, tr, localInertia);
    return 1;
}

INT bullet_body_create_box(lua_State* L) {
    D3DXMATRIX mat = *(D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    D3DXVECTOR4 halfExtents = *(D3DXVECTOR4*)luaL_checkudata(L, 2, L_VECTOR);
    FLOAT mass = (FLOAT)luaL_checknumber(L, 3);

    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    btCollisionShape* shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
    btVector3 localInertia;
    shape->calculateLocalInertia(mass, localInertia);
    bullet_body_create_generic(L, shape, mass, tr, localInertia);
    return 1;
}

INT bullet_body_create_capsule(lua_State* L) {
    D3DXMATRIX mat = *(D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
    FLOAT radius = (FLOAT)luaL_checknumber(L, 2);
    FLOAT height = (FLOAT)luaL_checknumber(L, 3);
    FLOAT mass = (FLOAT)luaL_checknumber(L, 4);

    btTransform tr;
    tr.setIdentity();
    tr.setFromOpenGLMatrix(&mat[0]);
    btCollisionShape* shape = new btCapsuleShape(radius, height);
    btVector3 localInertia;
    shape->calculateLocalInertia(mass, localInertia);
    bullet_body_create_generic(L, shape, mass, tr, localInertia);
    return 1;
}
