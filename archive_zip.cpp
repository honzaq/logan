#include "stdafx.h"
#include "archive_zip.h"

#include "zlib/zlib.h"
#include "zlib/unzip.h"

namespace logan {

enum class file_type
{
	unknown = 0,
	file,
	dir
};

struct zip_file_info 
{
	unzFile       file_handle = nullptr;
	unz_file_pos  file_pos;
	uLong         uncompress_size = 0;
	file_type     type = file_type::unknown;
};

bool archive_zip::open(const char* file_path)
{
	unzFile uf = unzOpen(file_path);
	if(uf == nullptr) {
		return false;
	}

	unz_global_info gi;
	if(unzGetGlobalInfo(uf, &gi) != UNZ_OK) {
		unzClose(uf);
		return false;
	}

	unz_file_info info;
	std::map<std::string, zip_file_info> files_data;

	int enumRes = unzGoToFirstFile(uf);
	while(enumRes == UNZ_OK) {
		
		// get file name
		char currentFileName[512];
		int fiRes = unzGetCurrentFileInfo(uf, &info, currentFileName, sizeof(currentFileName), 0, 0, 0, 0);
		if(fiRes != UNZ_OK) {
			assert(false && "cannot get current file info");
			enumRes = unzGoToNextFile(uf);
			continue;
		}
		std::string name(currentFileName);

		// analyze file
		zip_file_info zfi;
		zfi.file_handle = uf;
		if(unzGetFilePos(uf, &zfi.file_pos) != UNZ_OK) {
			assert(false && "cannot get file pos");
			enumRes = unzGoToNextFile(uf);
			continue;
		}
		zfi.uncompress_size = info.uncompressed_size;

		char lastsymbol = name[name.size() - 1];
		zfi.type = lastsymbol == '/' || lastsymbol == '\\' ? file_type::dir : file_type::file;

		files_data.emplace(name, zfi);

		// process next file
		enumRes = unzGoToNextFile(uf);
	}
	unzClose(uf);

	return true;
}

} // end of namespace logan