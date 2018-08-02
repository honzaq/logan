#include "stdafx.h"
#include "config_loader.h"

namespace logan {

bool config_loader::load(const wchar_t* file_name)
{
	std::ifstream ifs(file_name);
	m_cfg = json::parse(ifs);
	if(m_cfg.is_object() && m_cfg.find("parsers") != m_cfg.end()) {
		return true;
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