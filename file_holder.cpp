#include "stdafx.h"
#include "file_holder.h"
#include <exception>

namespace logan {

file_holder::file_holder()
{}

file_holder::~file_holder()
{
	close();
}

void file_holder::open(const WCHAR* file_name)
{
	// Close previous file
	if(m_hFile) {
		close();
	}

	// Open new file
	m_hFile = ::CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE) {
		throw std::exception("Could not open file");
	}

	m_fileSize = ::GetFileSize(m_hFile, nullptr);

	//////////////////////////////////////////////////////////////////////////
	// Mapping Given file to Memory
	m_hFileMapping = ::CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if(m_hFileMapping == nullptr) {
		throw std::exception("Could not map file");
	}

	m_lpBaseAddress = reinterpret_cast<uint8_t*>(::MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0));
	if(m_lpBaseAddress == nullptr) {
		throw std::exception("Map view of file fail");
	}

	// Detect file BOM
	const uint8_t* data = reinterpret_cast<uint8_t*>(m_lpBaseAddress);
	m_encoding = detect_bom(data, m_fileSize);
}

void file_holder::close()
{
	if(m_lpBaseAddress != nullptr) {
		::UnmapViewOfFile(m_lpBaseAddress);
		m_lpBaseAddress = nullptr;
	}
	if(m_hFileMapping != nullptr) {
		::CloseHandle(m_hFileMapping);
		m_hFileMapping = nullptr;
	}
	if(m_hFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hFile);
		m_hFile = nullptr;
	}
	m_fileSize = 0;
	m_encoding = encoding::none;
}

encoding file_holder::get_bom() const
{
	return m_encoding;
}

const uint8_t* file_holder::data() const
{
	return m_lpBaseAddress;
}

const size_t file_holder::data_size() const
{
	return m_fileSize;
}

} // end of namespace logan