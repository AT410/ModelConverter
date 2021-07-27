/*!
* @breif ���f���f�[�^���o�͊֘A
* @file�@PmxIO.h
*/
#pragma once
#include "stdafx.h"

namespace ModelIO
{
	using byte = unsigned char;

	// -- ���_ --
	struct float4
	{
		float x, y, z, w;
	};

	struct float3
	{
		float x, y, z;
	};

	struct float2
	{
		float x, y;
	};

	struct TexBuf
	{
		int size;
		vector<byte>Text;
	};

	struct PMX
	{
		// -- ���f���f�[�^ --
		// -- PMX�w�b�_ --
		struct Headder
		{
			//"PMX " | ASCII�� 0x50,0x4d,0x58,0x20 ��ver1.0�ł́A"Pmx " 
			array<byte, 4> headname;

			//�o�[�W����
			float version;

			//�ݒ�f�[�^�T�C�Y
			byte settingsize;
			//�ݒ�f�[�^�� ver2.0�ł�8�Œ�
			array<byte, 8> pmxsetting;

			enum SettingType
			{
				ENCORD_TYPE,			//<-�G���R�[�h�����@0:UTF16 1:UTF8
				ADD_UV_COUNT,			//<-0�`4��
				VERTEX_INDEX_SIZE,		//<-1,2,4byte
				TEXTURE_INDEX_SIZE,		//<-1,2,4byte
				MATERIAL_INDEX_SIZE,	//<-1,2,4byte
				BONE_INDEX_SIZE,		//<-1,2,4byte
				MORPH_INDEX_SIZE,		//<-1,2,4byte
				BODY_INDEX_SIZE			//<-1,2,4byte
			};


		}Headder;

		// -- ���f����� --
		struct  Info
		{
			TexBuf JapaneseName;

			TexBuf EnglishName;

			TexBuf JapaneseComment;

			TexBuf EnglishComment;

		}info;

		// -- ���_�f�[�^ --
		struct Vertex
		{
			float3 position;

			float3 normal;

			float2 uv;

			vector<float4> add_uvs;	//<-PMX�w�b�_�̒ǉ�UV����

			byte weightType;		//<-�E�F�C�g�ό`���� 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF

			enum WeightType
			{
				BDEF,
				BDEF2,
				BDEF4,
				SDEF
			};

			/*
			BDEF1 ->
			  n : �{�[��Index�T�C�Y  | �E�F�C�g1.0�̒P��{�[��(�Q��Index)

			BDEF2 ->
			  n : �{�[��Index�T�C�Y  | �{�[��1�̎Q��Index
			  n : �{�[��Index�T�C�Y  | �{�[��2�̎Q��Index
			  4 : float              | �{�[��1�̃E�F�C�g�l(0�`1.0), �{�[��2�̃E�F�C�g�l�� 1.0-�{�[��1�E�F�C�g

			BDEF4 ->
			  n : �{�[��Index�T�C�Y  | �{�[��1�̎Q��Index
			  n : �{�[��Index�T�C�Y  | �{�[��2�̎Q��Index
			  n : �{�[��Index�T�C�Y  | �{�[��3�̎Q��Index
			  n : �{�[��Index�T�C�Y  | �{�[��4�̎Q��Index
			  4 : float              | �{�[��1�̃E�F�C�g�l
			  4 : float              | �{�[��2�̃E�F�C�g�l
			  4 : float              | �{�[��3�̃E�F�C�g�l
			  4 : float              | �{�[��4�̃E�F�C�g�l (�E�F�C�g�v1.0�̕ۏ�͂Ȃ�)

			SDEF ->
			  n : �{�[��Index�T�C�Y  | �{�[��1�̎Q��Index
			  n : �{�[��Index�T�C�Y  | �{�[��2�̎Q��Index
			  4 : float              | �{�[��1�̃E�F�C�g�l(0�`1.0), �{�[��2�̃E�F�C�g�l�� 1.0-�{�[��1�E�F�C�g
			 12 : float3             | SDEF-C�l(x,y,z)
			 12 : float3             | SDEF-R0�l(x,y,z)
			 12 : float3             | SDEF-R1�l(x,y,z) ���C���l��v�v�Z
			*/

			int bone1;
			int bone2;
			int bone3;
			int bone4;

			float weight1;
			float weight2;
			float weight3;
			float weight4;

			float3 c;
			float3 R0;
			float3 R1;

			float edgeMag;
		};

		int vecsize;
		vector<Vertex> vertices;

		// -- �� --
		int surfacesize;
		vector<int> surfaceIndex;

		// -- �e�N�X�`�� --
		int texcount;
		vector<TexBuf> textable;

		// -- �}�e���A�� --
		struct  Material
		{
			TexBuf JapaneseName;
			TexBuf EnglishName;

			float4 Diffuse;
			float3 Specular;
			float  SpecularKey;
			float3 Ambient;

			byte bitFlag;

			float4 edghcolor;
			float  edghsize;

			int normalTexIndex;
			int sphereTexIndex;

			byte sphereMode;

			//���LToon�t���O
			byte sharedToonFlag;

			//�t���O��0�̏ꍇ
			int ToontexIndex;
			//�t���O��1�̏ꍇ
			byte sharedTexIndex;

			TexBuf memo;

			int SurfaceCount;
		};

		int materialCount;
		vector<Material> materials;

		// -- �{�[�� --
		struct Bone
		{
			TexBuf JapaneseName;
			TexBuf EnglishName;

			float3 position;
			int ownerBoneIndex;
			int TransClass;

			unsigned short flag;
			/*
			���{�[���t���O
				0x0001  : �ڑ���(PMD�q�{�[���w��)�\�����@ -> 0:���W�I�t�Z�b�g�Ŏw�� 1:�{�[���Ŏw��

				0x0002  : ��]�\
				0x0004  : �ړ��\
				0x0008  : �\��
				0x0010  : �����

				0x0020  : IK

				0x0080  : ���[�J���t�^ | �t�^�Ώ� 0:���[�U�[�ό`�l�^IK�����N�^���d�t�^ 1:�e�̃��[�J���ό`��
				0x0100  : ��]�t�^
				0x0200  : �ړ��t�^

				0x0400  : ���Œ�
				0x0800  : ���[�J����

				0x1000  : ������ό`
				0x2000  : �O���e�ό`
			*/
			
			//0x***0�̏ꍇ
			float3 posOffset;
			//0x***1�̏ꍇ
			int ConectIndex;

			//0x*1**����0x*2**�̏ꍇ
			int parentIndex;
			float PasteRate;

			//0x*4**
			float3 armVec;

			//0x*8**
			float3 armVecX;
			float3 armVecZ;

			//0x2***
			int keyvalue;

			//0x**2*
			int IKTargetIndex;
			int IKloopCount;
			float Rad;

			struct IKLink
			{
				int LinkIndex;
				byte safeFlag;
				//true
				float3 MixToRad;
				float3 MaxToRad;
			};

			int IKlinkCount;
			vector<IKLink> IKLinks;
		};

		int BoneCount;
		vector<Bone> Bones;

		// -- ���[�t --
		struct Morp
		{
			TexBuf JapaneseName;
			TexBuf EnglishName;

			byte ControlType;
			byte MorpType;

			int offsetCount;
		};
	};
}