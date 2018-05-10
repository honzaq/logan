#pragma once

namespace logan {

class file_holder
{
public:
	file_holder();
	~file_holder();

	enum class encoding {
		none = 0,
		utf8,
		utf16_le,
		utf16_be
	};

	void open(const WCHAR* file_name);
	void close();

	encoding get_bom() const;
	size_t get_bom_length() const;

	/* BOM already removed */
	const uint8_t* data() const;
	/* BOM already removed */
	const size_t data_length() const;

protected:
	HANDLE m_hFile = nullptr;
	HANDLE m_hFileMapping = nullptr;
	uint8_t* m_lpBaseAddress = nullptr;
	DWORD  m_fileSize = 0;
	encoding m_encoding = encoding::none;

};

} // end of namespace logan