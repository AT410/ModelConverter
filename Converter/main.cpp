#include "stdafx.h"


shared_ptr<vector<char>> LoadBinary(const string& filepath)
{
	shared_ptr<vector<char>> result;
	ifstream ifs(filepath, ios::in | ios::binary);
	if (ifs) {
		// �t�@�C���̃T�C�Y�𒲂ׂ�
		ifs.seekg(0, ios::end);
		size_t size = ifs.tellg();
		ifs.seekg(0, ios::beg);

		// �������m��
		result = make_shared<vector<char>>();
		result->resize(size);

		// �ǂݍ���
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

	//�Ǎ��ϐ�
	size_t offset = NULL;
	size_t size = NULL;

	// �w�b�_�[ -------------------------------
	std::array<byte, 4> pmxHeader;
	constexpr array<byte, 4> HEADASCII{ 0x50,0x4d,0x58,0x20 };
	
	size = 4;
	memcpy(&pmxHeader, &(*binfile)[offset], size);
	offset += size;

	if (pmxHeader != HEADASCII)
	{
		cout << "��Ή��̃o�[�W�����ł��B" << endl;
		return 0;
	}

	//PMX�o�[�W����
	float ver;

	size = sizeof(float);
	memcpy(&ver, &(*binfile)[offset], size);
	offset += size;

	//�o�C�g�T�C�Y�i���8�j
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
		cout << "�G���R�[�h������UTF-16LE�ł��B" << endl;
		return 0;
	}

	// -- ���f����� --
	/*
		TexBuf�`��
			int	�o�C�g���@4�o�C�g
			Text �o�C�g���@�ϒl
	*/
	array<vector<byte>, 4> modelInfo;
	
	for (int i = 0; i < 4; i++) 
	{
		int buffersize = NULL;

		size = sizeof(buffersize);
		memcpy(&buffersize, &(*binfile)[offset], size);
		offset += size;

		
		// -- TexBuf�̓Ǎ� --
		//TODO�@������Ǎ����B��
		modelInfo[i].resize(buffersize);

		offset += buffersize;

		
	}

	//test
	OutData result;
	

	// -- ���_�f�[�^�̓ǂݍ��� --

	//���_��
	int vertexcount = NULL;

	size = sizeof(int);
	memcpy(&vertexcount, &(*binfile)[offset], size);
	offset += size;

	result.vertexCount = static_cast<uint32_t>(vertexcount);
	result.vertices.resize(vertexcount);

	vector<Vertex> vertex(vertexcount);
	for (int i = 0; i < vertexcount; i++)
	{
		// -- �z�u�f�[�^ --
		size = sizeof(float3);
		memcpy(&vertex[i].position, &(*binfile)[offset], size);
		memcpy(&result.vertices[i].position, &(*binfile)[offset], size);
		offset += size;

		// -- �@���f�[�^ --
		size = sizeof(float3);
		memcpy(&vertex[i].normal, &(*binfile)[offset], size);
		memcpy(&result.vertices[i].normal, &(*binfile)[offset], size);
		offset += size;

		// -- UV�f�[�^ --
		size = sizeof(float2);
		memcpy(&vertex[i].uv, &(*binfile)[offset], size);
		memcpy(&result.vertices[i].uv, &(*binfile)[offset], size);
		offset += size;

		// -- �ǉ�UV --
		// !- ����ꍇ�擾 -!
		if (byterow[HeaderDataIndex::NUMBER_OF_ADD_UV] != 0)
		{
			cout << "�ǉ�UV����" << endl;
			return 0;
		}

		// -- �E�F�C�g�`�� --
		/*
			�E�F�C�g�ό`����	0:BDEF1 
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
		default://-1 ��Q��
			break;
		}

		float edgesize = NULL;
		size = sizeof(float);
		memcpy(&vertex[i].edgeMagnif, &(*binfile)[offset], size);
		offset += size;

	}

	// -- �ʐ� --
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

	// -- �e�N�X�`�� --
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

	// -- �}�e���A�� --

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

		//�����񕶊m��
		material[i].materialNameJa.resize(length);
		offset += length;

		//texBuf
		size = sizeof(int);
		memcpy(&length, &(*binfile)[offset], size);
		offset += size;

		//�����񕶊m��
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

	// -- �{�[�� --
	int BoneCount;
	size = sizeof(int);
	memcpy(&BoneCount, &(*binfile)[offset], size);
	offset += size;


	string savepath = "output/";
	savepath += modelname + ".mesh";
	std::ofstream savefile(savepath.c_str(), std::ios::binary | std::ios::out);

	if (!savefile)
	{
		std::cout << "�X�g���[���G���[" << std::endl;
		return 0;
	}

	// -- �t�@�C�������o�� --
	savefile.write((char*)&result.vertexCount, sizeof(uint32_t));					//<-���_�J�E���g
	savefile.write((char*)&result.vertices.front(), sizeof(Vertex2) * result.vertexCount);	//<-���_�f�[�^�i���W�E�@���EUV�j

	savefile.write((char*)&result.indexCount, sizeof(uint32_t));					//<-���_�J�E���g
	savefile.write((char*)&result.indices.front(), sizeof(uint16_t)*result.indexCount);					//<-���_�J�E���g

	savefile.close();

	return 0;
}