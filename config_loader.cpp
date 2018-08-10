#include "stdafx.h"
#include "config_loader.h"
#include "utils.h"
#include "rapidjson/filereadstream.h"

using namespace rapidjson;

namespace logan {

config_loader::config_loader(logger_ptr logger)
	: logger_holder(logger)
{
}

bool config_loader::load(const wchar_t* file_path)
{
	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, file_path, L"rb");
	if(err != 0) {
		logger()->error("Cannot open file '{}':{}", to_utf8(file_path), err);
		return false;
	}
	scope_guard fg = [&]() {
		fclose(file);
	};

	char read_buffer[65536];
	FileReadStream is(file, read_buffer, sizeof(read_buffer));
	
	if(m_cfg.ParseStream(is).HasParseError()) {
		logger()->error("Parse json file '{}' failed, {}:{}.", to_utf8(file_path), (int32_t)m_cfg.GetParseError(), m_cfg.GetErrorOffset());
		return false;
	}

	if(!m_cfg.IsObject() || !m_cfg.HasMember("parsers")) {
		return false;
	}

	return true;
}

parsers config_loader::parsers_from_name(const char* str)
{
	if(strcmp("asw", str) == 0) {
		return parsers::asw;
	}
	else if(strcmp("asw-short-date", str) == 0) {
		return parsers::asw_short_date;
	}
	return parsers::unknown;
}

// std::string config_loader::parsers_to_name(parsers parser)
// {
// 	switch(parser) {
// 	case logan::parsers::asw:
// 		return std::string("asw");
// 		break;
// 	case logan::parsers::asw_short_date:
// 		return std::string("asw-short-date");
// 		break;
// 	}
// 
// 	return std::string();
// }

parsers config_loader::get_parser(const wchar_t* file_name)
{
	std::string fine_name_a = to_utf8(file_name);

	parsers def_parser = parsers::unknown;
	const auto& jparsers = m_cfg["parsers"];
	for(const auto& it: jparsers.GetObject()) {

		const auto& it_val = it.value;

		if(!it_val.HasMember("file-filter")) {
			continue;
		}
		enum_parser<parsers> ep;
		if(it_val.HasMember("default") && it_val["default"].IsBool()) {
			def_parser = it_val["default"].GetBool() ? ep.toEnum(it.name.GetString()) : def_parser;
		}

		const auto& filters = it_val["file-filter"];
		for(const auto& it_filter: filters.GetArray()) {

			if(wildchar_compare(it_filter.GetString(), fine_name_a.c_str())) {
				return ep.toEnum(it.name.GetString());
			}
		}
	}
	return def_parser;
}

bool config_loader::trim_msg_whitespaces(parsers parser)
{
	//trim-msg-whitespaces
	enum_parser<parsers> ep;
	auto sparser = ep.fromEnum(parser);
	const auto& jparsers = m_cfg["parsers"];
	if(jparsers.HasMember(sparser.c_str())) {
		const auto& jpar = jparsers[sparser.c_str()];
		if(jpar.HasMember("trim-msg-whitespaces") && jpar["trim-msg-whitespaces"].IsBool()) {
			return jpar["trim-msg-whitespaces"].GetBool();
		}
		return false;
	}
	return false;
}

} // end of namespace logan