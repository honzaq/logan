#pragma once

#include "logger_holder.h"

namespace logan {

class archive_gz
	: logger_holder
{
public:
	archive_gz(logger_ptr logger);
	bool open(const char* file_path);
	void enumerate(std::function<bool(const std::string& name, const uint8_t* data, uint64_t data_size)> enumerate_fn);

protected:
	unsigned int get_gzip_file_size(const char* file_path);

private:
	std::unique_ptr<uint8_t[]> m_data_buffer;

	enum class file_type {
		unknown = 0,
		file,
		dir
	};
	struct file_info {
		uint64_t  data_start_pos = 0;
		uint64_t  uncompress_size = 0;
		file_type type = file_type::unknown;
	};
	std::map<std::string, file_info> m_files_data;
};

} // end of namespace logan