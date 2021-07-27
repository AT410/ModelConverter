/*!
* @breif PMX�`���̃o�C�i���t�@�C���Ǎ�
* @file PmxLoader.h
* @copyright Abe Tatsuya GitHub:at410
*/
#pragma once
#include "stdafx.h"

namespace ModelIO 
{
	class PmxLoader
	{
	public:
		PmxLoader(const string& PmxFilePath);
		PmxLoader();
		~PmxLoader();

		shared_ptr<vector<char>>& LoadBinary(const string& PmxFilePath);

		void Test();

		// -- Getter --
		shared_ptr<vector<char>>& GetBinData() { return m_binData; }
	private:
		// -- �������֐� --
		template<typename T>
		T Decord(size_t size);

		size_t m_offset;
		shared_ptr<vector<char>> m_binData;

		// -- ���f���f�[�^ --
	};
}
