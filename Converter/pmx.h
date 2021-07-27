#pragma once
#include "stdafx.h"

enum HeaderDataIndex
{
	ENCODING_FORMAT,
	NUMBER_OF_ADD_UV,
	VERTEX_INDEX_SIZE,
	TEXTURE_INDEX_SIZE,
	MATERIAL_INDEX_SIZE,
	BONE_INDEX_SIZE,
	RIGID_BODY_INDEX_SIZE
};

enum WeightType
{
	BDEF1,
	BDEF2,
	BDEF4,
	SDEF
};


// -- Pmxヘッダ --
struct PmxHead
{
	float ver;


};

//struct Vertex
//{
//	float3 position;//12byte
//	float3 normal;	//12byte
//	float2 uv;		//8byte
//};

// ボーンウェイト
struct Weight
{
	WeightType type;
	int born1;
	int	born2;
	int	born3;
	int	born4;
	float weight1;
	float weight2;
	float weight3;
	float weight4;
	float3 c;
	float3 r0;
	float3 r1;
};


struct Vertex
{
	// 頂点座標
	float3 position;//12byte
	// 法線
	float3 normal;	//12byte
	// UV座標
	float2 uv;		//8byte
	// 追加UV座標
	std::vector<float4> additionalUV;

	Weight weight;

	// エッジ倍率
	float edgeMagnif;
};

struct Vertex2
{
	// 頂点座標
	float3 position;//12byte
	// 法線
	float3 normal;	//12byte
	// UV座標
	float2 uv;		//8byte
};

struct Material
{
	vector<byte> materialNameJa;
	vector<byte> materialNameEn;

	float4 Diffuse;
	float3 Specular;
	float SpecularCoefficient;//Specular係数
	float3 Ambient;

	/*
		描画フラグ(8bit）－各bit 0：OFF　1:ON
			0x01:両面描画
			0x02:地面影
			0x04:セルフマップへの描画
			0x08:セルフシャドウの描画
			0x10:エッジ描画
	*/
	byte bitFlag;

	float4 edgeColor;
	float edgeSize;
	int texSize1;
	int texSize2;
	byte SMode;

	byte ToonFlag;
	int ToonTexIndex;
	byte ShareToonTexNum;

	vector<byte> memo;
	int materialofsurfaceCount;
};

struct PmxData
{

};

struct OutData
{
	uint32_t vertexCount;			//<-頂点数
	std::vector<Vertex2> vertices;	//<-頂点データ

	uint32_t indexCount;
	std::vector<uint16_t> indices;
};