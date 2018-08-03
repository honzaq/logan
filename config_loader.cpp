#include "stdafx.h"
#include "config_loader.h"
#include "utils.h"

namespace logan {

bool config_loader::load(const wchar_t* file_path)
{
	std::ifstream ifs(file_path);
	m_cfg = json::parse(ifs);
	if(m_cfg.is_object() && m_cfg.find("parsers") != m_cfg.end()) {
		return true;
	}
	return false;
}

parsers config_loader::parsers_from_name(const std::string& name)
{
	if(name.compare("asw") == 0) {
		return parsers::asw;
	}
	else if(name.compare("asw-short-date") == 0) {
		return parsers::asw_short_date;
	}
	return parsers::unknown;
}

std::string config_loader::parsers_to_name(parsers parser)
{
	switch(parser) {
	case logan::parsers::asw:
		return std::string("asw");
		break;
	case logan::parsers::asw_short_date:
		return std::string("asw-short-date");
		break;
	}

	return std::string();
}

parsers config_loader::get_parser(const wchar_t* file_name)
{
	parsers def_parser = parsers::unknown;
	auto jparsers = m_cfg["parsers"];
	for(auto it = jparsers.cbegin(); it != jparsers.cend(); ++it) {

		if(it.value().find("file-filter") == it.value().end()) {
			continue;
		}

		if(it.value().find("default") != it.value().end()) {
			def_parser = it.value().value("default", false) ? parsers_from_name(it.key()) : def_parser;
		}

		auto filters = it.value()["file-filter"];
		for(auto itFilter = filters.cbegin(); itFilter != filters.cend(); ++itFilter) {

			auto compare_no_case = [&](std::string& a, std::string& b) {
				return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) {
					return tolower(a) == tolower(b);
				});
			};

			std::string a = to_utf8(file_name);
			std::string b = itFilter.value().get<std::string>();

			if(wildchar_compare(b.c_str(), a.c_str())) {
				return parsers_from_name(it.key());
			}
		}
	}
	return def_parser;
}

bool config_loader::only_new_time(parsers parser)
{
	auto jparsers = m_cfg["parsers"];
	for(auto it = jparsers.cbegin(); it != jparsers.cend(); ++it) {

		if(parsers_to_name(parser).compare(it.key()) == 0) {
			if(it.value().find("only-new-time") != it.value().end()) {
				return it.value().value("only-new-time", false);
			}
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
/*
auto reports = report["reports"];
for(auto it = reports.cbegin(); it != reports.cend(); ++it) {

	data_item item;
	item.count = it.value()["count"].get<uint32_t>();
	item.skipped = 0;

	ULONGLONG ftLong = it.value()["last_time"].get<ULONGLONG>();
	item.last_time.dwLowDateTime = (DWORD)(ftLong & 0xFFFFFFFF);
	item.last_time.dwHighDateTime = (DWORD)(ftLong >> 32);

	m_analyzed_data.items.emplace(it.value()["text"].get<std::string>(), item);
}
*/
//////////////////////////////////////////////////////////////////////////

} // end of namespace logan