#pragma once

#include "bom.h"

namespace logan {

class file_holder
{
public:
	file_holder();
	~file_holder();

	/* Open file and set mapping on it */
	void open(const WCHAR* file_name);
	/* Close current file */
	void close();

	/* Get file BOM */
 	encoding get_bom() const;

	/* Return file data pointer */
	const uint8_t* data() const;
	/* Return file data size */
	const size_t data_size() const;

protected:
	HANDLE   m_hFile = nullptr;
	HANDLE   m_hFileMapping = nullptr;
	uint8_t* m_lpBaseAddress = nullptr;
	DWORD    m_fileSize = 0;
	encoding m_encoding = encoding::none;
};

} // end of namespace logan