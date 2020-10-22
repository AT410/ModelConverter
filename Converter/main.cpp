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
	string modelname = "pymodel.pmx";

	shared_ptr<vector<char>> binfile;

	binfile = LoadBinary(meshpath + modelname);

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

	// -- ���_�f�[�^�̓ǂݍ��� --

	//���_��
	int vertexcount = NULL;

	size = sizeof(int);
	memcpy(&vertexcount, &(*binfile)[offset], size);
	offset += size;

	vector<Vertex> vertex(vertexcount);
	for (int i = 0; i < vertexcount; i++)
	{
		// -- �z�u�f�[�^ --
		size = sizeof(float3);
		memcpy(&vertex[i].position, &(*binfile)[offset], size);
		offset += size;

		// -- �@���f�[�^ --
		size = sizeof(float3);
		memcpy(&vertex[i].normal, &(*binfile)[offset], size);
		offset += size;

		// -- UV�f�[�^ --
		size = sizeof(float2);
		memcpy(&vertex[i].uv, &(*binfile)[offset], size);
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
		default://-1 ��Q��
			break;
		}

		float edgesize = NULL;
		size = sizeof(float);
		memcpy(&edgesize, &(*binfile)[offset], size);
		offset++;

	}

	

	return 0;
}