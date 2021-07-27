#include "stdafx.h"

namespace ModelIO
{
	PmxLoader::PmxLoader()
		:m_offset(NULL)
	{};
	
	PmxLoader::PmxLoader(const string& PmxFilePath)
	{
		LoadBinary(PmxFilePath);
	}

	PmxLoader::~PmxLoader()
	{

	}

	shared_ptr<vector<char>>& PmxLoader::LoadBinary(const string& PmxFilePath)
	{
		ifstream ifs(PmxFilePath, ios::in | ios::binary);
		if (ifs) {
			// ファイルのサイズを調べる
			ifs.seekg(0, ios::end);
			size_t size = ifs.tellg();
			ifs.seekg(0, ios::beg);

			// メモリ確保
			m_binData = make_shared<vector<char>>();
			m_binData->resize(size);

			// 読み込み
			ifs.read(&m_binData->front(), size);

			ifs.close();
		}

		return m_binData;
	}

	void PmxLoader::Test()
	{
		array<byte, 4>los;

		auto te = Decord<array<byte, 4>>(4);
	}

	// -- private --

	template<typename T>
	T PmxLoader::Decord(size_t size)
	{
		T result;

		memcpy(&result, &(*m_binData)[m_offset], size);
		m_offset += size;

		return result;
	}
}