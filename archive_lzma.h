#pragma once

#include "logger_holder.h"

namespace logan {

class archive_lzma
	: logger_holder
{
public:
	archive_lzma(logger_ptr logger);
	bool open(const char* file_path);
};

} // end of namespace logan