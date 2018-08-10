#include "stdafx.h"
#include "data.h"

#include "rapidjson/document.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace logan {

data_export::data_export(logger_ptr logger)
	: logger_holder(logger)
{}

inline __int64 to_int64(FILETIME& ft)
{
	return static_cast<__int64>(ft.dwHighDateTime) << 32 | ft.dwLowDateTime;
}

bool data_export::serialize_to_json(const wchar_t* file_path, const global_data& data)
{
	Document doc(rapidjson::kObjectType);
	auto& allocator = doc.GetAllocator();

	//////////////////////////////////////////////////////////////////////////
	// order sum reports
	std::vector<std::reference_wrapper<const sum_map::value_type>> v(data.gsum_items.begin(), data.gsum_items.end());
	std::sort(v.begin(), v.end(), [](const sum_map::value_type& a, const sum_map::value_type& b) {
		return a.second.count > b.second.count;
	});

	//////////////////////////////////////////////////////////////////////////
	// sum reports
	Value sreports(rapidjson::kArrayType);
	for(const sum_map::value_type& item : v) {
		Value new_obj(rapidjson::kObjectType);
		new_obj.AddMember("text", Value(item.first.c_str(), allocator).Move(), allocator);
		new_obj.AddMember("count", item.second.count, allocator);
		sreports.PushBack(new_obj, allocator);
	}

	doc.AddMember("sum-reports", sreports, allocator);

	//////////////////////////////////////////////////////////////////////////
	// file reports
	Value freports(rapidjson::kObjectType);
	for(auto fitem : data.gtime_items) {
		
		Value file_arr(rapidjson::kArrayType);
		for(auto time_item : fitem.second) {
			Value new_obj(rapidjson::kObjectType);
			new_obj.AddMember("time", to_int64(time_item.time), allocator);
			new_obj.AddMember("msg", Value(time_item.msg.c_str(), allocator), allocator);
			file_arr.PushBack(new_obj, allocator);
		}

		freports.AddMember(Value(fitem.first.c_str(), allocator), file_arr, allocator);
	}
	doc.AddMember("file-reports", freports, allocator);


	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, file_path, L"wb");
	if(err != 0) {
		logger()->error("Cannot write to file '{}':{}", to_utf8(file_path), err);
		return false;
	}
	scope_guard fg = [&]() {
		fclose(file);
	};

	char write_buffer[65536];
	FileWriteStream os(file, write_buffer, sizeof(write_buffer));
	PrettyWriter<FileWriteStream> writer(os);
	doc.Accept(writer);

	return true;
}

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
// 	if(!doc.IsObject() || !doc.HasMember("sum-reports") || !doc["sum-reports"].IsArray()) {
// 		return false;
// 	}
// 
// 	const auto& reports = doc["sum-reports"];
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

} // end of namespace logan