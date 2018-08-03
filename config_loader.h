#pragma once

#include "parsers.h"

namespace logan {

class config_loader
{
public:
	bool load(const wchar_t* file_path);

	parsers get_parser(const wchar_t* file_name);

	bool only_new_time(parsers parser);

private:
	parsers parsers_from_name(const std::string& name);
	std::string parsers_to_name(parsers parser);

protected:
	json m_cfg;
};

} // end of namespace logan