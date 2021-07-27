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


// -- Pmx�w�b�_ --
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

// �{�[���E�F�C�g
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
	// ���_���W
	float3 position;//12byte
	// �@��
	float3 normal;	//12byte
	// UV���W
	float2 uv;		//8byte
	// �ǉ�UV���W
	std::vector<float4> additionalUV;

	Weight weight;

	// �G�b�W�{��
	float edgeMagnif;
};

struct Vertex2
{
	// ���_���W
	float3 position;//12byte
	// �@��
	float3 normal;	//12byte
	// UV���W
	float2 uv;		//8byte
};

struct Material
{
	vector<byte> materialNameJa;
	vector<byte> materialNameEn;

	float4 Diffuse;
	float3 Specular;
	float SpecularCoefficient;//Specular�W��
	float3 Ambient;

	/*
		�`��t���O(8bit�j�|�ebit 0�FOFF�@1:ON
			0x01:���ʕ`��
			0x02:�n�ʉe
			0x04:�Z���t�}�b�v�ւ̕`��
			0x08:�Z���t�V���h�E�̕`��
			0x10:�G�b�W�`��
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
	uint32_t vertexCount;			//<-���_��
	std::vector<Vertex2> vertices;	//<-���_�f�[�^

	uint32_t indexCount;
	std::vector<uint16_t> indices;
};