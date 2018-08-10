#include "stdafx.h"
#include "archive_gz.h"
#include "zlib/zlib.h"

namespace logan {

archive_gz::archive_gz(logger_ptr logger)
	: logger_holder(logger)
{}

unsigned int archive_gz::get_gzip_file_size(const char* file_path)
{
	FILE* file= nullptr;

	errno_t err = fopen_s(&file, file_path, "rb");
	if(err != 0) {
		return 0;
	}

	if(fseek(file, -4, SEEK_END) != 0) {
		fclose(file);
		return 0;
	}

	unsigned char buf_size[4] = { 0 };
	unsigned int len = fread(&buf_size[0], sizeof(unsigned char), 4, file);
	if(len != 4) {
		fclose(file);
		return 0;
	}

	auto file_size = (unsigned int)((buf_size[3] << 24) | (buf_size[2] << 16) | (buf_size[1] << 8) | buf_size[0]);

	fclose(file);

	return file_size;
}

typedef struct {
	char name[100];
	char mode[8];
	char owner[8];
	char group[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char type;
	char linkname[100];
	char _padding[255];
} tar_header;

static uint64_t round_up(uint64_t n, unsigned incr) {
	return n + (incr - n % incr) % incr;
}

bool archive_gz::open(const char* file_path)
{
	measure::time gz_load_time(L"gz_read");

	m_files_data.clear();

	auto uncompress_size = get_gzip_file_size(file_path);

	gzFile file = gzopen(file_path, "r");
	if(!file) {
		assert(false);
		return false;
	}

	m_data_buffer = std::make_unique<uint8_t[]>((size_t)uncompress_size);
	int bytes_read = gzread(file, m_data_buffer.get(), uncompress_size);
	if((unsigned int)bytes_read < uncompress_size) {
		const char * error_string;
		int err;
		error_string = gzerror(file, &err);
		if(err) {
			assert(false);
		}
	}
	gzclose(file);

	//////////////////////////////////////////////////////////////////////////
	// Parse tar file (inside .gz)

	uint64_t pos = 0;

	while(pos < uncompress_size) {
		tar_header header;
		memcpy_s(&header, sizeof(header), m_data_buffer.get() + pos, sizeof(header));
		pos += sizeof(header);

		file_info fi;
		if(header.type == '0') {
			fi.type = file_type::file;
		}
		else if(header.type == '5') {
			fi.type = file_type::dir;
		}
		else if(header.type == '\0') {
			// and of tar
			break;
		}
		else {
			assert(false && "unsupported type");
		}
		fi.data_start_pos = pos;
		sscanf_s(header.size, "%llo", &fi.uncompress_size);

		std::string name(header.name);
		m_files_data.emplace(name, fi);

		pos += round_up(fi.uncompress_size, 512);
	}

	gz_load_time.end_measure([&](const wchar_t* id, double dDiff) {
		logger()->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
	});

	return true;
}

void archive_gz::enumerate(std::function<bool(const std::string& name, const uint8_t* data, uint64_t data_size)> enumerate_fn)
{
	for(const auto data_file : m_files_data) {
		if(!enumerate_fn(data_file.first, reinterpret_cast<const uint8_t*>(m_data_buffer.get() + data_file.second.data_start_pos), data_file.second.uncompress_size)) {
			break;
		}
	}
}


} // end of namespace logan