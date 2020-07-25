#pragma once

#include "system.h"
#include "StdAfx.h"

#include "RenderData.h"
#include "ReferenceManager.h"

class ENGINE_API CMaterial: public CReferenceCounter, CAllocable<CMaterial>
{
public:
    CMaterial(UINT slot, LPSTR texName);
    CMaterial(UINT slot, UINT w, UINT h);
    CMaterial(UINT slot, LPVOID data, UINT size = 1);
    CMaterial();
    ~CMaterial() { Release(); }

    VOID DefaultMaterial(VOID);
    VOID Release(VOID);

    VOID CreateTextureForSlot(UINT slot, LPSTR texName=NULL, UINT w=1, UINT h=1);
    VOID CreateEmbeddedTextureForSlot(UINT slot, LPVOID data, UINT size);
    
    inline VOID SetSamplerState(UINT state, UINT value) { mStats[state] = value; }
    inline UINT GetSamplerState(UINT state) const { return mStats[state]; }
    inline LPDIRECT3DTEXTURE9 GetTextureHandle(UINT slot=TEXTURESLOT_ALBEDO) { return mTextureHandle[slot]; }

    VOID SetUserTexture(UINT userSlot, LPDIRECT3DTEXTURE9 handle);
    VOID Bind(DWORD stage);
    VOID Unbind(DWORD stage);
    VOID* Lock(UINT slot=TEXTURESLOT_ALBEDO);
    VOID UploadARGB(UINT slot, VOID* data, UINT size);
    VOID Unlock(UINT slot = TEXTURESLOT_ALBEDO);

    VOID SetAmbient(D3DCOLORVALUE color);
    VOID SetDiffuse(D3DCOLORVALUE color);
    VOID SetSpecular(D3DCOLORVALUE color);
    VOID SetEmission(D3DCOLORVALUE color);
    VOID SetPower(FLOAT val);
    VOID SetOpacity(FLOAT val);
    VOID SetShaded(BOOL state);
    VOID SetAlphaIsTransparency(BOOL state);
    VOID SetEnableAlphaTest(BOOL state);
    VOID SetAlphaRef(DWORD refval);

    inline MATERIAL GetMaterialData() const { return mMaterialData; }
    inline BOOL IsTransparent() const { return mMaterialData.Opacity < 1.0f || mMaterialData.AlphaIsTransparency; }

private:
    LPDIRECT3DTEXTURE9 mTextureHandle[MAX_TEXTURE_SLOTS];
    MATERIAL mMaterialData;

    UINT mStats[MAX_SAMPLER_STATES];
};

