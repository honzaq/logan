#include "stdafx.h"
#include "analyzer.h"

#include "file_holder.h"
#include "search.h"
#include "log_line.h"

namespace logan {

analyzer::analyzer(share_data& data, logger_ptr logger)
	: share_data_holder(data)
	, logger_holder(logger)
{
}

void analyzer::analyze_file(const wchar_t* file_name)
{
	// Define search text
	std::string search_text = "[error";

	file_holder fh;
	fh.open(file_name);

	file_holder::encoding bom = fh.get_bom();
	if(bom != file_holder::encoding::utf8) {
		// Currently support only UTF-8
		logger()->error("Currently support only UTF-8.");
		return;
	}

	bool only_new_time = data().cfg->only_new_time(parsers::asw);

	//////////////////////////////////////////////////////////////////////////
	// build table for fast search
	const search::occtable_type occ = search::create_occ_table((const uint8_t*)search_text.data(), search_text.size());

	// get initial pos and length
	const uint8_t* data_cur_pos = fh.data();
	size_t data_remain_length = fh.data_length();

	// search in remaining data
	size_t pos = search::search_in_horspool(data_cur_pos, data_remain_length, occ, (const uint8_t*)search_text.data(), search_text.size());
	while(pos < data_remain_length) {

		// prepare found line
		log_line ll(data_cur_pos, data_remain_length, pos);

		// get line msg and count it
		auto msg = ll.get_msg();
		auto time = ll.get_time();

		auto it = m_analyzed_data.items.find(msg);
		if(it != m_analyzed_data.items.end()) {
			if(only_new_time) {
				if(::CompareFileTime(&it->second.last_time, &time) < 0) {
					it->second.count++;
					it->second.last_time = time;
				}
				else {
					it->second.skipped++;
				}
			}
			else {
				it->second.count++;
				if(::CompareFileTime(&it->second.last_time, &time) < 0) {
					it->second.last_time = time;
				}
			}
		}
		else {
			data_item item = { 1, 0, time };
			m_analyzed_data.items.emplace(msg, item);
		}

		// move after last found
		data_cur_pos += pos + search_text.size();
		data_remain_length -= pos + search_text.size();

		pos = search::search_in_horspool(data_cur_pos, data_remain_length, occ, (const uint8_t*)search_text.data(), search_text.size());
	}
}

void analyzer::dbg_print_result(bool lines /*= true*/)
{
	uint32_t total = 0;
	uint32_t counter = 1;
	for(auto item : m_analyzed_data.items) {
		if(lines) {
			logger()->info("{:4d} {:2d}:{:2d} {}", counter++, item.second.count, item.second.skipped, item.first);
		}
		total += item.second.count;
	}
	logger()->info("Total found: {}", total);
}

void analyzer::serialize_to_json(const wchar_t* file_name)
{
	json report = json::object();
	json reports = json::array();
	for(auto item : m_analyzed_data.items) {
		json new_obj = json::object();
		new_obj["text"] = item.first;
		new_obj["count"] = item.second.count;

		ULONGLONG qwResult;
		qwResult = (((ULONGLONG)item.second.last_time.dwHighDateTime) << 32) + item.second.last_time.dwLowDateTime;
		new_obj["last_time"] = qwResult;

		reports.emplace_back(new_obj);
	}
	report["reports"] = reports;
	
	std::ofstream o(file_name);
	o << std::setw(4) << report << std::endl;
}

void analyzer::parse_from_json(const wchar_t* file_name)
{
	// clear previous data
	m_analyzed_data.items.clear();

	// parse json
	std::ifstream ifs(file_name);
	if(ifs) {
		json report = json::parse(ifs);
		if(report.is_object() && report.find("reports") != report.end()) {
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
		}
	}
}

void analyzer::reset_counters()
{
	for(auto& item : m_analyzed_data.items) {
		item.second.count = 0;
	}
}

} // end of namespace logan