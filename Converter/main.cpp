#include "stdfx.h"

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
	string modelname = "pymodel.pmx";

	shared_ptr<vector<char>> binfile;

	binfile = LoadBinary(meshpath + modelname);

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

	// -- 頂点データの読み込み --

	//頂点数
	int vertexcount = NULL;

	size = sizeof(int);
	memcpy(&vertexcount, &(*binfile)[offset], size);
	offset += size;

	vector<Vertex> vertex(vertexcount);
	for (int i = 0; i < vertexcount; i++)
	{
		// -- 配置データ --
		size = sizeof(float3);
		memcpy(&vertex[i].position, &(*binfile)[offset], size);
		offset += size;

		// -- 法線データ --
		size = sizeof(float3);
		memcpy(&vertex[i].normal, &(*binfile)[offset], size);
		offset += size;

		// -- UVデータ --
		size = sizeof(float2);
		memcpy(&vertex[i].uv, &(*binfile)[offset], size);
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

		int weight = NULL;
		float weightVal = NULL;
		float3 SDEFVal;

		switch (weightType)
		{
		case WeightType::BDEF1:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;
			break;
		case WeightType::BDEF2:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&weightVal, &(*binfile)[offset], size);
			offset += size;

			break;
		case WeightType::BDEF4:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&weightVal, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&weightVal, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&weightVal, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&weightVal, &(*binfile)[offset], size);
			offset += size;

			break;
		case WeightType::SDEF:
			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = byterow[HeaderDataIndex::BONE_INDEX_SIZE];
			memcpy(&weight, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float);
			memcpy(&weightVal, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float3);
			memcpy(&SDEFVal, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float3);
			memcpy(&SDEFVal, &(*binfile)[offset], size);
			offset += size;

			size = sizeof(float3);
			memcpy(&SDEFVal, &(*binfile)[offset], size);
			offset += size;
			break;
		default://-1 非参照
			break;
		}

		float edgesize = NULL;
		size = sizeof(float);
		memcpy(&edgesize, &(*binfile)[offset], size);
		offset++;

	}

	

	return 0;
}