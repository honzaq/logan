#pragma once

#include "parsers.h"
#include "rapidjson/document.h"

namespace logan {

class config_loader : logger_holder
{
public:
	config_loader(logger_ptr logger);

	bool load(const wchar_t* file_path);

	parsers get_parser(const wchar_t* file_name);
	bool trim_msg_whitespaces(parsers parser);

private:
	parsers parsers_from_name(const char* str);

protected:
	rapidjson::Document m_cfg;
};

} // end of namespace logan