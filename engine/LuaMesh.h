#pragma once

#include "system.h"

#include <lua/lua.hpp>

#include "MeshBuilder.h"

static INT mesh_new(lua_State* L)
{
	*(MeshBuilder*)lua_newuserdata(L, sizeof(MeshBuilder)) = MeshBuilder();

	luaL_setmetatable(L, L_MESH);
	return 1;
}

static INT mesh_addvertex(lua_State* L)
{
	MeshBuilder* mesh = (MeshBuilder*)luaL_checkudata(L, 1, L_MESH);
	VERTEX* vert = (VERTEX*)luaL_checkudata(L, 2, L_VERTEX);
	mesh->AddVertex(*vert);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_addindex(lua_State* L)
{
	MeshBuilder* mesh = (MeshBuilder*)luaL_checkudata(L, 1, L_MESH);
	SHORT index = (SHORT)luaL_checknumber(L, 2);
	mesh->AddIndex(index);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_addtriangle(lua_State* L)
{
	MeshBuilder* mesh = (MeshBuilder*)luaL_checkudata(L, 1, L_MESH);
	SHORT i1 = (SHORT)luaL_checknumber(L, 2);
	SHORT i2 = (SHORT)luaL_checknumber(L, 3);
	SHORT i3 = (SHORT)luaL_checknumber(L, 4);
	
	mesh->AddIndex(i1);
	mesh->AddIndex(i2);
	mesh->AddIndex(i3);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_settexture(lua_State* L)
{
	MeshBuilder* mesh = (MeshBuilder*)luaL_checkudata(L, 1, L_MESH);
	DWORD stage = (DWORD)luaL_checknumber(L, 2);
	LPDIRECT3DTEXTURE9* tex = NULL;

	if (lua_gettop(L) == 3)
		tex = (LPDIRECT3DTEXTURE9*)luaL_checkudata(L, 3, L_TEXTURE);

	mesh->SetTexture(stage, tex ? *tex : NULL);

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_draw(lua_State* L)
{
	MeshBuilder* mesh = (MeshBuilder*)luaL_checkudata(L, 1, L_MESH);
	
	mesh->Draw();

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_build(lua_State* L)
{
	MeshBuilder* mesh = (MeshBuilder*)luaL_checkudata(L, 1, L_MESH);

	mesh->Build();

	lua_pushvalue(L, 1);
	return 1;
}

static INT mesh_delete(lua_State* L)
{
	MeshBuilder* mesh = (MeshBuilder*)luaL_checkudata(L, 1, L_MESH);

	mesh->Release();

	lua_pushvalue(L, 1);
	return 1;
}

static VOID LuaMesh_register(lua_State* L)
{
	lua_register(L, L_MESH, mesh_new);
	luaL_newmetatable(L, L_MESH);
	lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, mesh_addvertex); lua_setfield(L, -2, "addVertex");
	lua_pushcfunction(L, mesh_addindex); lua_setfield(L, -2, "addIndex");
	lua_pushcfunction(L, mesh_addtriangle); lua_setfield(L, -2, "addTriangle");
	lua_pushcfunction(L, mesh_settexture); lua_setfield(L, -2, "setTexture");
	lua_pushcfunction(L, mesh_draw); lua_setfield(L, -2, "draw");
	lua_pushcfunction(L, mesh_build); lua_setfield(L, -2, "build");
	lua_pushcfunction(L, mesh_delete) ; lua_setfield(L, -2, "__gc");

	lua_pop(L, 1);
}
