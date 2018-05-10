#include "stdafx.h"
#include "file_holder.h"
#include <exception>

namespace logan {

constexpr unsigned char UTF16_BOM_LE[] = { unsigned char(0xFF), unsigned char(0xFE) };
constexpr unsigned char UTF16_BOM_BE[] = { unsigned char(0xFE), unsigned char(0xFF) };
constexpr unsigned char UTF8_BOM[] = { unsigned char(0xEF), unsigned char(0xBB), unsigned char(0xBF) };

file_holder::file_holder()
{}
file_holder::~file_holder()
{
	close();
}

void file_holder::open(const WCHAR* file_name)
{
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

	m_encoding = get_bom();
}

void file_holder::close()
{
	if(m_lpBaseAddress != nullptr) {
		::UnmapViewOfFile(m_lpBaseAddress);
	}
	if(m_hFileMapping != nullptr) {
		::CloseHandle(m_hFileMapping);
	}
	if(m_hFile != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hFile);
	}
}

file_holder::encoding file_holder::get_bom() const
{
	const uint8_t* data = reinterpret_cast<uint8_t*>(m_lpBaseAddress);
	if(m_fileSize >= 2 && data[0] == UTF16_BOM_LE[0] && data[1] == UTF16_BOM_LE[1]) {
		return encoding::utf16_le;
	}
	else if(m_fileSize >= 2 && data[0] == UTF16_BOM_BE[0] && data[1] == UTF16_BOM_BE[1]) {
		return encoding::utf16_be;
	}
	else if(m_fileSize >= 3 && data[0] == UTF8_BOM[0] && data[1] == UTF8_BOM[1] && data[2] == UTF8_BOM[2]) {
		return encoding::utf8;
	}
	else {
		return encoding::none;
	}
}

size_t file_holder::get_bom_length() const
{
	switch(m_encoding) {
	case encoding::utf8:
		return 3;
		break;
	case encoding::utf16_le:
	case encoding::utf16_be:
		return 2;
		break;
	}
	return 0;
}

const uint8_t* file_holder::data() const
{
	return m_lpBaseAddress + get_bom_length();
}

const size_t file_holder::data_length() const
{
	return m_fileSize - get_bom_length();
}

} // end of namespace logan