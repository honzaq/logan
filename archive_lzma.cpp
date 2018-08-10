#include "stdafx.h"
#include "archive_lzma.h"
#include "7zip/7zTypes.h"
#include "7zip/LzmaDec.h"
#include "7zip/LzmaLib.h"
#include "7zip/Alloc.h"

namespace logan {

//////////////////////////////////////////////////////////////////////////
static void *_lzmaAlloc(ISzAllocPtr, size_t size) {
	return new uint8_t[size];
}
static void _lzmaFree(ISzAllocPtr, void *addr) {
	if(!addr) {
		return;
	}
	delete[] reinterpret_cast<uint8_t *>(addr);
}
static ISzAlloc _allocFuncs = {
	_lzmaAlloc, _lzmaFree
};
//////////////////////////////////////////////////////////////////////////

archive_lzma::archive_lzma(logger_ptr logger)
	: logger_holder(logger)
{}

bool archive_lzma::open(const char* file_path)
{
	measure::time gz_load_time(L"gz_read");

	FILE* file = nullptr;
	fopen_s(&file, file_path, "rb");
	size_t size, decSize;
	Byte *data, *dec = nullptr;

	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	data = (unsigned char*)malloc(size);
	fread(data, 1, size, file);
	fclose(file);

	//const Byte * props = reinterpret_cast<const Byte *>(&data);

	ISzAlloc allocator;
	allocator = g_Alloc;
	CLzmaDec decoder;
	LzmaDec_Construct(&decoder);
	SRes result = LzmaDec_Allocate(&decoder, data, LZMA_PROPS_SIZE, &allocator);
	LzmaDec_Init(&decoder);

	const Byte*  pos = data + LZMA_PROPS_SIZE;
	size_t fileSize = 0;
	for(int i = 0; i < 8; ++i) {
		if(pos >= data + size) {
			assert(false);
		}

		fileSize |= ((UInt64)*pos++) << (8 * i);
	}

	const std::uint64_t * uncompressed_size = reinterpret_cast<const std::uint64_t *>(data + LZMA_PROPS_SIZE);
	std::uint64_t uncompressed_size2 = *(reinterpret_cast<std::uint64_t*>(data + LZMA_PROPS_SIZE));

	const Byte * position = data + (LZMA_PROPS_SIZE + sizeof(std::uint64_t));

	auto total_compressed_bytes = static_cast<SizeT>(size - (position - data));

	SizeT decompressDataSize = fileSize;

	auto dec_buf = std::make_unique<std::uint8_t[]>(fileSize);

	ELzmaStatus status = LZMA_STATUS_NOT_SPECIFIED;
	result = LzmaDec_DecodeToBuf(&decoder, dec_buf.get(), &decompressDataSize, position, &total_compressed_bytes, LZMA_FINISH_ANY, &status);
	if(result != SZ_OK) {
		assert(false);
	}

// 	file = fopen("Module_DEC.dll", "w");
// 	fwrite(dec, 1, decSize, file);
// 	fclose(file);

	gz_load_time.end_measure([&](const wchar_t* id, double dDiff) {
		logger()->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
	});

	return true;
}

} // end of namespace logan