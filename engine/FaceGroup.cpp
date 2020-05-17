#include "stdafx.h"

#include "FaceGroup.h"
#include "NeonEngine.h"

#include "Material.h"
#include "Frustum.h"

CFaceGroup::CFaceGroup(VOID)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
    mVerts.Release();
    mIndices.Release();
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
	mIsDirty = FALSE;
}

VOID CFaceGroup::Release(VOID)
{
	if (DelRef())
	{
        mVerts.Release();
        mIndices.Release();
        SAFE_RELEASE(mData.mesh);
        SAFE_RELEASE(mData.mat);
	}
}

VOID CFaceGroup::SetMaterial(DWORD stage, CMaterial* tex)
{
	mData.stage = stage;
	mData.mat = tex;
}

VOID CFaceGroup::AddVertex(const VERTEX& vertex)
{
	mIsDirty = TRUE;
    mVerts.Push(vertex);
}

VOID CFaceGroup::AddIndex(SHORT index)
{
	mIsDirty = TRUE;
	mIndices.Push(index);
}

VOID CFaceGroup::Draw(D3DXMATRIX* mat)
{
	if (!mData.mesh || mIsDirty)
		Build();

	if (mat)
	{
		mData.usesMatrix = TRUE;
		mData.matrix = *mat;
	}
	else mData.usesMatrix = FALSE;

    BOOL isGlobalShadingEnabled = RENDERER->GetLightingState();

    if (isGlobalShadingEnabled)
        RENDERER->EnableLighting(mData.mat->GetMaterialData().Shaded);

    RENDERER->SetDefaultRenderStates();

    mData.mat->Bind(mData.stage);

	RENDERER->DrawMesh(mData);

    RENDERER->EnableLighting(isGlobalShadingEnabled);

	mData.mat->Unbind(mData.stage);
}

VOID CFaceGroup::CalculateNormals()
{
    if (!mData.mesh)
    {
        MessageBoxA(NULL, "Mesh is not built yet, you can not calculate normals!", "Mesh error", MB_OK);
        ENGINE->Shutdown();
        return;
    }

	D3DXComputeNormals(mData.mesh, NULL);
}

VOID CFaceGroup::Build(VOID)
{
	LPDIRECT3DDEVICE9 dev = RENDERER->GetDevice();
	VOID *vidMem = NULL;
	SAFE_RELEASE(mData.mesh);

	if (mVerts.GetCount() == 0)
		return;

	DWORD numFaces = ((mIndices.GetCount() > 0) ? mIndices.GetCount() : mVerts.GetCount())/3;
	
	D3DXCreateMesh(numFaces,
		mVerts.GetCount(),
		D3DXMESH_MANAGED,
		meshVertexFormat,
		dev,
		&mData.mesh);
	
    mData.mesh->LockVertexBuffer(0, (VOID**)&vidMem);
    memcpy(vidMem, mVerts.GetData(), mVerts.GetCount() * sizeof(VERTEX));

    D3DXComputeBoundingSphere((D3DXVECTOR3*)mVerts.GetData(),
        mVerts.GetCount(),
        sizeof(VERTEX),
        &mData.meshOrigin,
        &mData.meshRadius);

	mData.mesh->UnlockVertexBuffer();

    if (mIndices.GetCount() > 0)
    {
        mData.mesh->LockIndexBuffer(0, (VOID**)&vidMem);
        memcpy(vidMem, mIndices.GetData(), mIndices.GetCount() * sizeof(SHORT));
		mData.mesh->UnlockIndexBuffer();
    }

	mIsDirty = FALSE;
}

VOID CFaceGroup::Clear(VOID)
{
	ZeroMemory(&mData, sizeof(RENDERDATA));
	mData.kind = PRIMITIVEKIND_TRIANGLELIST;
    mVerts.Release();
    mIndices.Release();
    SAFE_RELEASE(mData.mesh);
    SAFE_RELEASE(mData.mat);
	mIsDirty = FALSE;
}

CFaceGroup* CFaceGroup::Clone()
{
	CFaceGroup* clonedFG = new CFaceGroup();
	clonedFG->SetMaterial(mData.stage, mData.mat); // TODO: Clone material
	mData.mat->AddRef();

	for (auto v : mVerts)
		clonedFG->AddVertex(v);

	for (auto i : mIndices)
		clonedFG->AddIndex(i);

	return clonedFG;
}
