#include "StdAfx.h"
#include "Model.h"

#include "Mesh.h"
#include "MeshLoader.h"
#include "NeonEngine.h"
#include "FileSystem.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CModel::CModel(LPSTR modelPath)
{
    mCapacity = 4;
    mCount = 0;
    mMeshes = (CMesh**)malloc(mCapacity * sizeof(CMesh*));
    D3DXMatrixIdentity(&identityMat);

    if (modelPath)
        LoadModel(modelPath);
}

void CModel::Release()
{
    SAFE_FREE(mMeshes);
    mCount = 0;
    mCapacity = 0;
}

#define MESHIMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded |\
    aiProcess_Triangulate |\
    aiProcess_CalcTangentSpace |\
    aiProcess_FlipUVs |\
    aiProcess_JoinIdenticalVertices |\
    aiProcess_PreTransformVertices

void CModel::LoadModel(LPCSTR modelPath, BOOL loadMaterials)
{
    Assimp::Importer imp;

    const aiScene* model = imp.ReadFile(FILESYSTEM->ResourcePath(RESOURCEKIND_USER, modelPath), MESHIMPORT_FLAGS);

    if (!model)
    {
        MessageBoxA(NULL, "Mesh import failed!", "Mesh error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

    CMesh* meshGroup = NULL;
    aiString meshName;

    for (UINT i = 0; i < model->mNumMeshes; i++)
    {
        aiMesh* m = model->mMeshes[i];
        if (!meshGroup || strcmp(meshName.C_Str(), m->mName.C_Str()))
        {
            if (meshGroup)
                AddMeshGroup(meshGroup);

            meshGroup = new CMesh();
            CReferenceManager::TrackRef(meshGroup);
            
            meshName = m->mName;
            meshGroup->SetName(meshName);
        }

        CFaceGroup* mesh = CMeshLoader::LoadNode(model, m, loadMaterials);
        meshGroup->AddMesh(mesh, identityMat);
    }

    AddMeshGroup(meshGroup);
}

void CModel::Draw(const D3DXMATRIX& wmat)
{
    if (!mMeshes)
        return;

    for (UINT i=0; i<mCount;i++)
    {
        mMeshes[i]->Draw(wmat);
    }
}

CMesh* CModel::FindMeshGroup(LPCSTR name)
{
    for (UINT i = 0; i < mCount; i++)
    {
        if (!strcmp(name, mMeshes[i]->GetName().C_Str()))
            return mMeshes[i];
    }

    return NULL;
}

void CModel::AddMeshGroup(CMesh* mg)
{
    if (!mg)
        return;

    if (mCount >= mCapacity)
    {
        mCapacity += 4;

        mMeshes = (CMesh**)realloc(mMeshes, mCapacity * sizeof(CMesh*));
        
        if (!mMeshes )
        {
            MessageBoxA(NULL, "Can't add mesh group to model!", "Out of memory error", MB_OK);
            ENGINE->Shutdown();
            return;
        }
    }

    mMeshes[mCount++] = mg;
}

D3DXMATRIX CModel::identityMat;