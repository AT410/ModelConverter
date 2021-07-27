/*!
* @breif モデルデータ入出力関連
* @file　PmxIO.h
*/
#pragma once
#include "stdafx.h"

namespace ModelIO
{
	using byte = unsigned char;

	// -- 頂点 --
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
		// -- モデルデータ --
		// -- PMXヘッダ --
		struct Headder
		{
			//"PMX " | ASCIIで 0x50,0x4d,0x58,0x20 ※ver1.0では、"Pmx " 
			array<byte, 4> headname;

			//バージョン
			float version;

			//設定データサイズ
			byte settingsize;
			//設定データ列 ver2.0では8固定
			array<byte, 8> pmxsetting;

			enum SettingType
			{
				ENCORD_TYPE,			//<-エンコード方式　0:UTF16 1:UTF8
				ADD_UV_COUNT,			//<-0個～4個
				VERTEX_INDEX_SIZE,		//<-1,2,4byte
				TEXTURE_INDEX_SIZE,		//<-1,2,4byte
				MATERIAL_INDEX_SIZE,	//<-1,2,4byte
				BONE_INDEX_SIZE,		//<-1,2,4byte
				MORPH_INDEX_SIZE,		//<-1,2,4byte
				BODY_INDEX_SIZE			//<-1,2,4byte
			};


		}Headder;

		// -- モデル情報 --
		struct  Info
		{
			TexBuf JapaneseName;

			TexBuf EnglishName;

			TexBuf JapaneseComment;

			TexBuf EnglishComment;

		}info;

		// -- 頂点データ --
		struct Vertex
		{
			float3 position;

			float3 normal;

			float2 uv;

			vector<float4> add_uvs;	//<-PMXヘッダの追加UV数分

			byte weightType;		//<-ウェイト変形方式 0:BDEF1 1:BDEF2 2:BDEF4 3:SDEF

			enum WeightType
			{
				BDEF,
				BDEF2,
				BDEF4,
				SDEF
			};

			/*
			BDEF1 ->
			  n : ボーンIndexサイズ  | ウェイト1.0の単一ボーン(参照Index)

			BDEF2 ->
			  n : ボーンIndexサイズ  | ボーン1の参照Index
			  n : ボーンIndexサイズ  | ボーン2の参照Index
			  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト

			BDEF4 ->
			  n : ボーンIndexサイズ  | ボーン1の参照Index
			  n : ボーンIndexサイズ  | ボーン2の参照Index
			  n : ボーンIndexサイズ  | ボーン3の参照Index
			  n : ボーンIndexサイズ  | ボーン4の参照Index
			  4 : float              | ボーン1のウェイト値
			  4 : float              | ボーン2のウェイト値
			  4 : float              | ボーン3のウェイト値
			  4 : float              | ボーン4のウェイト値 (ウェイト計1.0の保障はない)

			SDEF ->
			  n : ボーンIndexサイズ  | ボーン1の参照Index
			  n : ボーンIndexサイズ  | ボーン2の参照Index
			  4 : float              | ボーン1のウェイト値(0～1.0), ボーン2のウェイト値は 1.0-ボーン1ウェイト
			 12 : float3             | SDEF-C値(x,y,z)
			 12 : float3             | SDEF-R0値(x,y,z)
			 12 : float3             | SDEF-R1値(x,y,z) ※修正値を要計算
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

		// -- 面 --
		int surfacesize;
		vector<int> surfaceIndex;

		// -- テクスチャ --
		int texcount;
		vector<TexBuf> textable;

		// -- マテリアル --
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

			//共有Toonフラグ
			byte sharedToonFlag;

			//フラグが0の場合
			int ToontexIndex;
			//フラグが1の場合
			byte sharedTexIndex;

			TexBuf memo;

			int SurfaceCount;
		};

		int materialCount;
		vector<Material> materials;

		// -- ボーン --
		struct Bone
		{
			TexBuf JapaneseName;
			TexBuf EnglishName;

			float3 position;
			int ownerBoneIndex;
			int TransClass;

			unsigned short flag;
			/*
			○ボーンフラグ
				0x0001  : 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定

				0x0002  : 回転可能
				0x0004  : 移動可能
				0x0008  : 表示
				0x0010  : 操作可

				0x0020  : IK

				0x0080  : ローカル付与 | 付与対象 0:ユーザー変形値／IKリンク／多重付与 1:親のローカル変形量
				0x0100  : 回転付与
				0x0200  : 移動付与

				0x0400  : 軸固定
				0x0800  : ローカル軸

				0x1000  : 物理後変形
				0x2000  : 外部親変形
			*/
			
			//0x***0の場合
			float3 posOffset;
			//0x***1の場合
			int ConectIndex;

			//0x*1**又は0x*2**の場合
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

		// -- モーフ --
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