#include "stdafx.h"


shared_ptr<vector<char>> LoadBinary(const string& filepath)
{
	shared_ptr<vector<char>> result;
	ifstream ifs(filepath, ios::in | ios::binary);
	if (ifs) {
		// ファイルのサイズを調べる
		ifs.seekg(0, ios::end);
		size_t size = ifs.tellg();
		ifs.seekg(0, ios::beg);

		// メモリ確保
		result = make_shared<vector<char>>();
		result->resize(size);

		// 読み込み
		ifs.read(&result->front(), size);

		ifs.close();
	}

	return result;
}

int main()
{
	string meshpath = "mesh/";
	string modelname = "woman";
	meshpath += modelname + ".pmx";
	shared_ptr<vector<char>> binfile;

	ModelIO::PmxLoader loader(meshpath);

	loader.Test();

	binfile = LoadBinary(meshpath);

	//読込変数
	size_t offset = NULL;
	size_t size = NULL;

	// ヘッダー -------------------------------
	std::array<byte, 4> pmxHeader;
	constexpr array<byte, 4> HEADASCII{ 0x50,0x4d,0x58,0x20 };
	
	size = 4;
	memcpy(&pmxHeader, &(*binfile)[offset], size);
	offset += size;

	if (pmxHeader != HEADASCII)
	{
		cout << "非対応のバージョンです。" << endl;
		return 0;
	}

	//PMXバージョン
	float ver;

	size = sizeof(float);
	memcpy(&ver, &(*binfile)[offset], size);
	offset += size;

	//バイトサイズ（主に8）
	byte bytesize;

	size = sizeof(byte);
	memcpy(&bytesize, &(*binfile)[offset], size);
	offset += size;

	if (bytesize != 8)
	{
		cout << "Not 8 byte size" << endl;
		return 0;
	}

	array<byte, 8>byterow;

	size = sizeof(byte) * bytesize;
	memcpy(&byterow, &(*binfile)[offset], size);
	offset += size;

	if (byterow[ENCODING_FORMAT] != 0)
	{
		cout << "エンコード方式がUTF-16LEです。" << endl;
		return 0;
	}

	// -- モデル情報 --
	/*
		TexBuf形式
			int	バイト長　4バイト
			Text バイト長　可変値
	*/
	array<vector<byte>, 4> modelInfo;
	
	for (int i = 0; i < 4; i++) 
	{
		int buffersize = NULL;

		size = sizeof(buffersize);
		memcpy(&buffersize, &(*binfile)[offset], size);
		offset += size;

		
		// -- TexBufの読込 --
		//TODO　文字列読込未達成
		modelInfo[i].resize(buffersize);

		offset += buffersize;

		
	}

	//test
	OutData result;
	

	// -- 頂点データの読み込み --

	//頂点数
	int vertexcount = NULL;

	size = sizeof(int);
	memcpy(&vertexcount, &(*binfile)[offset], size);
	offset += size;

	result.vertexCount = static_cast<uint32_t>(vertexcount);
	result.vertices.resize(vertexcount);

	vector<Vertex> vertex(vertexcount);
	for (int i = 0; i < vertexcount; i++)
	{
		// -- 配置データ --
		size = sizeof(float3);
		memcpy(&vertex[i].position, &(*binfile)[offset], size);
		memcpy(&result.vertices[i].position, &(*binfile)[offset], size);
		offset += size;

		// -- 法線データ --
		size = sizeof(float3);
		memcpy(&vertex[i].normal, &(*binfile)[offset], size);
		memcpy(&result.vertices[i].normal, &(*binfile)[offset], size);
		offset += size;

		// -- UVデータ --
		size = sizeof(float2);
		memcpy(&vertex[i].uv, &(*binfile)[offset], size);
		memcpy(&result.vertices[i].uv, &(*binfile)[offset], size);
		offset += size;

		// -- 追加UV --
		// !- ある場合取得 -!
		if (byterow[HeaderDataIndex::NUMBER_OF_ADD_UV] != 0)
		{
			cout << "追加UVあり" << endl;
			return 0;
		}

		// -- ウェイト形式 --
		/*
			ウェイト変形方式	0:BDEF1 
								1:BDEF2 
								2:BDEF4 
								3:SDEF
		*/
		byte weightType;
		 
		size = sizeof(byte);
		memcpy(&weightType, &(*binfile)[offset], size);
		offset += size;

		vertex[i].weight.type = static_cast<WeightType>(weightType);

		switch (weightType)
		{
		case WeightType::BDEF1:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born1, &(*binfile)[offset], size);
			offset += size;
			break;
		case WeightType::BDEF2:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born1, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born2, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&vertex[i].weight.weight1, &(*binfile)[offset], size);
			offset += size;

			break;
		case WeightType::BDEF4:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born1, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born2, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born3, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born4, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&vertex[i].weight.weight1, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&vertex[i].weight.weight2, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&vertex[i].weight.weight3, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&vertex[i].weight.weight4, &(*binfile)[offset], size);
			offset += size;

			break;
		case WeightType::SDEF:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born1, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&vertex[i].weight.born2, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&vertex[i].weight.weight1, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float3);
			memcpy(&vertex[i].weight.c, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float3);
			memcpy(&vertex[i].weight.r0, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float3);
			memcpy(&vertex[i].weight.r1, &(*binfile)[offset], size);
			offset += size;
			break;
		default://-1 非参照
			break;
		}

		float edgesize = NULL;
		size = sizeof(float);
		memcpy(&vertex[i].edgeMagnif, &(*binfile)[offset], size);
		offset += size;

	}

	// -- 面数 --
	int surfacecount;
	size = sizeof(int);
	memcpy(&surfacecount, &(*binfile)[offset], size);
	offset += size;

	vector<int> surface(surfacecount);
	result.indexCount = surfacecount;
	result.indices.resize(surfacecount);
	for (int i = 0; i < surfacecount; i++)
	{
		size = byterow[HeaderDataIndex::VERTEX_INDEX_SIZE];
		memcpy(&surface[i], &(*binfile)[offset], size);
		memcpy(&result.indices[i], &(*binfile)[offset], size);
		offset += size;

		if (surface[i] == 2)
		{
			cout << "2" << endl;
		}
	}

	// -- テクスチャ --
	int texturecount = NULL;
	size = sizeof(int);
	memcpy(&texturecount, &(*binfile)[offset], size);
	offset += size;

	vector<vector<byte>> texturePath(texturecount);
	for (int i = 0; i < texturecount; i++)
	{
		//TexBuf
		int texbyte = NULL;
		size = sizeof(int);
		memcpy(&texbyte, &(*binfile)[offset], size);
		offset += size;

		texturePath[i].resize(texbyte);

		offset += texbyte;
	}

	// -- マテリアル --

	int materialcount = NULL;
	size = sizeof(int);
	memcpy(&materialcount, &(*binfile)[offset], size);
	offset += size;
	vector<Material> material(materialcount);
	for (int i = 0; i < materialcount; i++)
	{
		//texBuf
		int length;
		size = sizeof(int);
		memcpy(&length, &(*binfile)[offset], size);
		offset += size;

		//文字列文確保
		material[i].materialNameJa.resize(length);
		offset += length;

		//texBuf
		size = sizeof(int);
		memcpy(&length, &(*binfile)[offset], size);
		offset += size;

		//文字列文確保
		material[i].materialNameEn.resize(length);
		offset += length;

		size = sizeof(float4);
		memcpy(&material[i].Diffuse, &(*binfile)[offset], size);
		offset += size;
		size = sizeof(float3);
		memcpy(&material[i].Specular, &(*binfile)[offset], size);
		offset += size;
		size = sizeof(float);
		memcpy(&material[i].SpecularCoefficient, &(*binfile)[offset], size);
		offset += size;
		size = sizeof(float3);
		memcpy(&material[i].Ambient, &(*binfile)[offset], size);
		offset += size;

		size = sizeof(byte);
		memcpy(&material[i].bitFlag, &(*binfile)[offset], size);
		offset += size;

		size = sizeof(float4);
		memcpy(&material[i].edgeColor, &(*binfile)[offset], size);
		offset += size;
		
		size = sizeof(float);
		memcpy(&material[i].edgeSize, &(*binfile)[offset], size);
		offset += size;

		size = byterow[HeaderDataIndex::TEXTURE_INDEX_SIZE];
		memcpy(&material[i].texSize1, &(*binfile)[offset], size);
		offset += size;

		size = byterow[HeaderDataIndex::TEXTURE_INDEX_SIZE];
		memcpy(&material[i].texSize2, &(*binfile)[offset], size);
		offset += size;

		size = sizeof(byte);
		memcpy(&material[i].SMode, &(*binfile)[offset], size);
		offset += size;

		size = sizeof(byte);
		memcpy(&material[i].ToonFlag, &(*binfile)[offset], size);
		offset += size;
		if (material[i].ToonFlag)
		{
			size = byterow[HeaderDataIndex::TEXTURE_INDEX_SIZE];
			memcpy(&material[i].ToonTexIndex, &(*binfile)[offset], size);
			offset += size;
		}
		else
		{
			size = sizeof(byte);
			memcpy(&material[i].ShareToonTexNum, &(*binfile)[offset], size);
			offset += size;
		}

		//Tex
		int memosize = NULL;
		size = sizeof(int);
		memcpy(&memosize, &(*binfile)[offset], size);
		offset += size;

		size = memosize;
		material[i].memo.resize(memosize);

		size = sizeof(int);
		memcpy(&material[i].materialofsurfaceCount, &(*binfile)[offset], size);
		offset += size;
	}

	// -- ボーン --
	int BoneCount;
	size = sizeof(int);
	memcpy(&BoneCount, &(*binfile)[offset], size);
	offset += size;


	string savepath = "output/";
	savepath += modelname + ".mesh";
	std::ofstream savefile(savepath.c_str(), std::ios::binary | std::ios::out);

	if (!savefile)
	{
		std::cout << "ストリームエラー" << std::endl;
		return 0;
	}

	// -- ファイル書き出し --
	savefile.write((char*)&result.vertexCount, sizeof(uint32_t));					//<-頂点カウント
	savefile.write((char*)&result.vertices.front(), sizeof(Vertex2) * result.vertexCount);	//<-頂点データ（座標・法線・UV）

	savefile.write((char*)&result.indexCount, sizeof(uint32_t));					//<-頂点カウント
	savefile.write((char*)&result.indices.front(), sizeof(uint16_t)*result.indexCount);					//<-頂点カウント

	savefile.close();

	return 0;
}