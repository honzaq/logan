#pragma once

namespace logan {

struct sum_data_item {
	uint32_t count = 0;
};

struct time_data_item {
	FILETIME    time = { 0 };
	std::string msg;
};

struct analyzed_data {
	std::string file_name;
	std::map<std::string, sum_data_item> sum_items;
	std::vector<time_data_item> time_items;
};

using sum_map = std::map<std::string, sum_data_item>;

struct global_data {
	sum_map gsum_items;
	std::map<std::string, std::vector<time_data_item>> gtime_items;
	
	void append(const analyzed_data& data) {
		gtime_items.emplace(data.file_name, data.time_items);
		for(const auto item : data.sum_items) {
			auto it = gsum_items.find(item.first);
			if(it != gsum_items.end()) {
				it->second.count += it->second.count;
			}
			else {
				gsum_items.emplace(item.first, item.second);
			}
		}
	}
};

class data_export : logger_holder
{
public:
	data_export(logger_ptr logger);
	bool serialize_to_json(const wchar_t* file_path, const global_data& data);
};

} // end of namespace logan