#pragma once

#include "system.h"

#include <lua/lua.hpp>

static INT matrix_new(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)lua_newuserdata(L, sizeof(D3DXMATRIX));
	D3DXMatrixIdentity(mat);

	luaL_setmetatable(L, L_MATRIX);
	return 1;
}

static INT matrix_translate(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	FLOAT x = (FLOAT)lua_tonumber(L, 2);
	FLOAT y = (FLOAT)lua_tonumber(L, 3);
	FLOAT z = (FLOAT)lua_tonumber(L, 4);
	D3DXMatrixTranslation(mat, x, y, z);

	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_rotate(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	FLOAT x = (FLOAT)lua_tonumber(L, 2);
	FLOAT y = (FLOAT)lua_tonumber(L, 3);
	FLOAT z = (FLOAT)lua_tonumber(L, 4);
	if (x) D3DXMatrixRotationX(mat, D3DXToRadian(x));
	if (y) D3DXMatrixRotationY(mat, D3DXToRadian(y));
	if (z) D3DXMatrixRotationZ(mat, D3DXToRadian(z));
	
	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_scale(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	FLOAT x = (FLOAT)lua_tonumber(L, 2);
	FLOAT y = (FLOAT)lua_tonumber(L, 3);
	FLOAT z = (FLOAT)lua_tonumber(L, 4);
	D3DXMatrixScaling(mat, x, y, z);

	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_mul(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXMATRIX* matRHS = (D3DXMATRIX*)luaL_checkudata(L, 2, L_MATRIX);

	D3DXMATRIX* out = (D3DXMATRIX*)lua_newuserdata(L, sizeof(D3DXMATRIX));
	*out = *mat * *matRHS;

	luaL_setmetatable(L, L_MATRIX);
	return 1;
}

static INT matrix_bind(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	FLOAT kind = (FLOAT)luaL_checknumber(L, 2);
	RENDERER->PushMatrix((UINT)kind, *mat);

	lua_pushvalue(L, 1);
	return 1;
}

static INT matrix_lookat(lua_State* L)
{
	D3DXMATRIX* mat = (D3DXMATRIX*)luaL_checkudata(L, 1, L_MATRIX);
	D3DXVECTOR3* eye = (D3DXVECTOR3*)luaL_checkudata(L, 2, L_VECTOR3);
	D3DXVECTOR3* at = (D3DXVECTOR3*)luaL_checkudata(L, 3, L_VECTOR3);
	D3DXVECTOR3* up = (D3DXVECTOR3*)luaL_checkudata(L, 4, L_VECTOR3);

	D3DXMatrixLookAtLH(mat, eye, at, up);
	lua_pushvalue(L, 1);
	return 1;
}

static VOID LuaMatrix_register(lua_State* L)
{
	lua_register(L, L_MATRIX, matrix_new);
	luaL_newmetatable(L, L_MATRIX);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, matrix_translate); lua_setfield(L, -2, "translate");
	lua_pushcfunction(L, matrix_rotate); lua_setfield(L, -2, "rotate");
	lua_pushcfunction(L, matrix_scale); lua_setfield(L, -2, "scale");
	lua_pushcfunction(L, matrix_bind); lua_setfield(L, -2, "bind");
	lua_pushcfunction(L, matrix_lookat); lua_setfield(L, -2, "lookAt");

	lua_pushcfunction(L, matrix_mul); lua_setfield(L, -2, "__mul");
	lua_pop(L, 1);
}
