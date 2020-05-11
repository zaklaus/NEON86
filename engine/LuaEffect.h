#pragma once

#include "system.h"
#include "Effect.h"

#include <lua/lua.hpp>

static INT effect_new(lua_State* L)
{
    LPCSTR effectPath = luaL_checkstring(L, 1);
    
    *(CEffect*)lua_newuserdata(L, sizeof(CEffect)) = CEffect(effectPath);

    luaL_setmetatable(L, L_EFFECT);
    return 1;
}

static INT effect_begin(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR technique = luaL_checkstring(L, 2);
    
    lua_pushinteger(L, fx->Begin(technique));
    return 1;
}

static INT effect_end(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);

    lua_pushinteger(L, fx->End());
    return 1;
}

static INT effect_beginpass(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    UINT pass = -1;

    if (lua_isstring(L, 2))
    {
        LPCSTR passName = luaL_checkstring(L, 2);
        pass = fx->FindPass(passName);
    }
    else pass = (UINT)luaL_checkinteger(L, 2) - 1;

    lua_pushinteger(L, fx->BeginPass(pass));
    return 1;
}

static INT effect_endpass(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);

    lua_pushinteger(L, fx->EndPass());
    return 1;
}

static INT effect_commit(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);

    lua_pushinteger(L, fx->CommitChanges());
    return 1;
}

static INT effect_setbool(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    FLOAT value = (FLOAT)lua_toboolean(L, 3);

    fx->SetBool(name, value == 1.0f);
    return 0;
}

static INT effect_setfloat(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    FLOAT value = (FLOAT)luaL_checknumber(L, 3);

    fx->SetFloat(name, value);
    return 0;
}

static INT effect_setmatrix(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    D3DXMATRIX* value = (D3DXMATRIX*)luaL_checkudata(L, 3, L_MATRIX);

    fx->SetMatrix(name, *value);
    return 0;
}

static INT effect_setvector3(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    D3DXVECTOR3* value = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR);

    fx->SetVector3(name, *value);
    return 0;
}

static INT effect_setinteger(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    DWORD value = (DWORD)luaL_checkinteger(L, 3);

    fx->SetInteger(name, value);
    return 0;
}

static INT effect_setlight(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    CLight* value = NULL;

    if (lua_gettop(L) == 3 && luaL_testudata(L, 3, L_LIGHT)) {
        value = (CLight*)lua_touserdata(L, 3);
    }

    fx->SetLight(name, value);
    return 0;
}

static INT effect_setvector4(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    
    if (lua_gettop(L) == 4)
    {
        D3DXVECTOR3* value = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR);
        FLOAT value2 = (FLOAT)lua_tonumber(L, 4);
        fx->SetVector4(name, D3DXVECTOR4(*value, value2));
    }
    else
    {
        D3DXVECTOR4* value = (D3DXVECTOR4*)luaL_checkudata(L, 3, L_VECTOR);
        fx->SetVector4(name, *value);
    }
        

    return 0;
}

static INT effect_settexture(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    LPCSTR name = luaL_checkstring(L, 2);
    
    if (luaL_testudata(L, 3, L_RENDERTARGET))
    {
        CRenderTarget* rtt = (CRenderTarget*)lua_touserdata(L, 3);
        fx->SetTexture(name, rtt->GetTextureHandle());
    }
    else if (luaL_testudata(L, 3, L_MATERIAL))
    {
        UINT slot = TEXTURESLOT_ALBEDO;
        CMaterial* mat = (CMaterial*)lua_touserdata(L, 3);

        if (lua_gettop(L) == 4)
            slot = (UINT)luaL_checkinteger(L, 4) - 1;

        fx->SetTexture(name, mat->GetTextureHandle(slot));
    }
    else {
        LPDIRECT3DTEXTURE9 handle = (LPDIRECT3DTEXTURE9)lua_touserdata(L, 3);
        fx->SetTexture(name, handle);
    }
    
    return 0;
}

static INT effect_delete(lua_State* L)
{
    CEffect* fx = (CEffect*)luaL_checkudata(L, 1, L_EFFECT);
    
    fx->Release();
    return 0;
}

static VOID LuaEffect_register(lua_State* L)
{
    lua_register(L, L_EFFECT, effect_new);
    luaL_newmetatable(L, L_EFFECT);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    luaN_setid(L);

    REGC("start", effect_begin);
    REGC("finish", effect_end);

    REGC("begin", effect_begin);
    REGC("flush", effect_end);
    REGC("done", effect_end);

    REGC("beginPass", effect_beginpass);
    REGC("endPass", effect_endpass);
    REGC("commit", effect_commit);

    REGC("setBool", effect_setbool);
    REGC("setFloat", effect_setfloat);
    REGC("setInt", effect_setinteger);
    REGC("setMatrix", effect_setmatrix);
    REGC("setVector3", effect_setvector3);
    REGC("setVector4", effect_setvector4);
    REGC("setTexture", effect_settexture);
    REGC("setLight", effect_setlight);

    REGC("__gc", effect_delete);

    lua_pop(L, 1);
}
