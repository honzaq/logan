#include "stdafx.h"
#include "analyzer.h"

#include "file_holder.h"
#include "search.h"
#include "log_line.h"
#include "data.h"

namespace logan {

analyzer::analyzer(share_data& data, logger_ptr logger)
	: share_data_holder(data)
	, logger_holder(logger)
{
}

void analyzer::analyze_file(const wchar_t* file_path)
{
	file_holder fh;
	fh.open(file_path);

	analyze_data(fh.data(), fh.data_size(), to_utf8(file_path));
}

void analyzer::analyze_data(const uint8_t* data, uint64_t data_size, const std::string& name)
{
	m_analyzed_data.file_name = name;

	// Define search text
	std::string search_text = "[error";

	encoding bom = detect_bom(data, data_size);
	if(bom != encoding::utf8) {
		// Currently support only UTF-8
		logger()->error("Currently support only UTF-8.");
		return;
	}
	auto bom_len = bom_length(bom);

	//////////////////////////////////////////////////////////////////////////
	// build table for fast search
	const search::occtable_type occ = search::create_occ_table((const uint8_t*)search_text.data(), search_text.size());

	// get initial pos and length
	const uint8_t* data_cur_pos = reinterpret_cast<const uint8_t*>(data + bom_len);
	size_t data_remain_length = (size_t)(data_size - bom_len);

	log_line ll;
	ll.trim_msg_whitespaces(sh_data().cfg->trim_msg_whitespaces(parsers::asw));

	// search in remaining data
	size_t pos = search::search_in_horspool(data_cur_pos, data_remain_length, occ, (const uint8_t*)search_text.data(), search_text.size());
	while(pos < data_remain_length) {

		// prepare found line
		ll.parse(data_cur_pos, data_remain_length, pos);

		// get line msg and count it
		auto msg = ll.get_msg();
		auto time = ll.get_time();

		//////////////////////////////////////////////////////////////////////////
		// By sum count
		auto it = m_analyzed_data.sum_items.find(msg);
		if(it != m_analyzed_data.sum_items.end()) {
			it->second.count++;
		}
		else {
			sum_data_item item = { 1 };
			m_analyzed_data.sum_items.emplace(msg, item);
		}

		//////////////////////////////////////////////////////////////////////////
		// By time order
		time_data_item time_item;
		time_item.time = time;
		time_item.msg = msg;
		m_analyzed_data.time_items.emplace_back(time_item);

		//////////////////////////////////////////////////////////////////////////
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
	for(auto item : m_analyzed_data.sum_items) {
		if(lines) {
			logger()->info("{:4d} {:2d} {}", counter++, item.second.count, item.first);
		}
		total += item.second.count;
	}
	logger()->info("Total found: {}", total);
}

// bool analyzer::serialize_to_json(const wchar_t* file_path)
// {
// 	Document doc(rapidjson::kObjectType);
// 	auto& allocator = doc.GetAllocator();
// 
// 	Value reports(rapidjson::kArrayType);
// 	for(auto item : m_analyzed_data.sum_items) {
// 		Value new_obj(rapidjson::kObjectType);
// 		new_obj.AddMember("text", Value(item.first.c_str(), allocator).Move(), allocator);
// 		new_obj.AddMember("count", item.second.count, allocator);
// 		reports.PushBack(new_obj, allocator);
// 	}
// 
// 	doc.AddMember("reports", reports, allocator);
// 
// 	FILE* file = nullptr;
// 	errno_t err = _wfopen_s(&file, file_path, L"wb");
// 	if(err != 0) {
// 		logger()->error("Cannot write to file '{}':{}", to_utf8(file_path), err);
// 		return false;
// 	}
// 	scope_guard fg = [&]() {
// 		fclose(file);
// 	};
// 
// 	char write_buffer[65536];
// 	FileWriteStream os(file, write_buffer, sizeof(write_buffer));
// 	PrettyWriter<FileWriteStream> writer(os);
// 	doc.Accept(writer);
// 
// 	return true;
// }
// 
// bool analyzer::parse_from_json(const wchar_t* file_path)
// {
// 	// clear previous data
// 	m_analyzed_data.sum_items.clear();
// 
// 	FILE* file = nullptr;
// 	errno_t err = _wfopen_s(&file, file_path, L"rb");
// 	if(err != 0) {
// 		if(err != 2) {
// 			logger()->error("Cannot open file '{}':{}", to_utf8(file_path), err);
// 		}
// 		return false;
// 	}
// 	scope_guard fg = [&]() {
// 		fclose(file);
// 	};
// 
// 	char read_buffer[65536];
// 	FileReadStream is(file, read_buffer, sizeof(read_buffer));
// 
// 	Document doc;
// 	if(doc.ParseStream(is).HasParseError()) {
// 		logger()->error("Parse json file '{}' failed, {}:{}.", to_utf8(file_path), (int32_t)doc.GetParseError(), doc.GetErrorOffset());
// 		return false;
// 	}
// 
// 	if(!doc.IsObject() || !doc.HasMember("reports") || !doc["reports"].IsArray()) {
// 		return false;
// 	}
// 
// 	const auto& reports = doc["reports"];
// 	for(const auto& it: reports.GetArray()) {
// 
// 		if(!it.IsObject()) {
// 			continue;
// 		}
// 
// 		sum_data_item item;
// 		item.count = it["count"].GetUint();
// 
// 		m_analyzed_data.sum_items.emplace(it["text"].GetString(), item);
// 	}
// 
// 	return true;
// }

void analyzer::reset_counters()
{
	for(auto& item : m_analyzed_data.sum_items) {
		item.second.count = 0;
	}
}

const analyzed_data& analyzer::result() const
{
	return m_analyzed_data;
}

} // end of namespace logan