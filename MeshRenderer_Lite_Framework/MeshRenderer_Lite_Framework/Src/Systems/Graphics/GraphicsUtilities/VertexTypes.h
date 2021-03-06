#pragma once

#include <DirectXMath.h>

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

struct VertexWire
{
	XMFLOAT3 position;
	XMFLOAT4 color;

	VertexWire() :position(0, 0, 0), color(1, 1, 1, 1) {}
	VertexWire(const XMFLOAT3& pos, const XMFLOAT4& col) : position(pos), color(col) {}
};

struct VertexNormal
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;

	VertexNormal() :position(0, 0, 0), normal(0, 0, 0), color(1, 1, 1, 1) {}
	VertexNormal(const XMFLOAT3& pos, const XMFLOAT3& norm, const XMFLOAT4& col) :position(pos),
		normal(norm), color(col) {}
};

struct VertexTexture
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	XMFLOAT3 tangent;
	XMFLOAT3 biTangent;
	XMFLOAT4 color;

	VertexTexture() :position(0, 0, 0), normal(0, 0, 0), tangent(0, 0, 0), biTangent(0, 0, 0), uv(0, 0), color(1, 1, 1, 1) {}
	VertexTexture(const XMFLOAT3& pos, const XMFLOAT3& norm, const XMFLOAT2& uv, const XMFLOAT3& tangent, const XMFLOAT4& col) :position(pos),
		normal(norm), uv(uv), tangent(tangent), color(col) {}
	VertexTexture(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v,
		XMFLOAT4 col = XMFLOAT4(1, 1, 1, 1))
		: position(px, py, pz), normal(nx, ny, nz),
		uv(u, v),
		tangent(tx, ty, tz),
		color(col)
	{
	}
};

struct VertexAnimation
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 biTangent;
	XMFLOAT2 uv;
	XMFLOAT4 color;

	//Animation bone information
	unsigned int boneIDs[4] = { 0,0,0,0 };
	float boneWeights[4] = { 0,0,0,0 };


	VertexAnimation() :position(0, 0, 0), normal(0, 0, 0), tangent(0, 0, 0), biTangent(0, 0, 0), uv(0, 0), 
		color(1, 1, 1, 1) {}

	VertexAnimation(const XMFLOAT3& pos, const XMFLOAT3& norm, const XMFLOAT2& uv, const XMFLOAT3& tangent, const XMFLOAT4& col) :position(pos),
		normal(norm), uv(uv), tangent(tangent), color(col) {}
	VertexAnimation(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v,
		const XMFLOAT4& col = XMFLOAT4(1, 1, 1, 1))
		: position(px, py, pz), 
		normal(nx, ny, nz),
		uv(u, v),
		tangent(tx, ty, tz),
		color(col)
	{
	}
};