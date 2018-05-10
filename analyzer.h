#pragma once

namespace logan {

struct data_item {
	uint32_t count = 0;
	uint32_t skipped = 0;
	FILETIME last_time = { 0 };
};

struct analyze_data {
	std::map<std::string, data_item> items;
};

//////////////////////////////////////////////////////////////////////////
// Analyzer class
//////////////////////////////////////////////////////////////////////////
class analyzer : protected logger_holder
{
public:
	analyzer(logger_ptr logger);

	void analyze_file(const wchar_t* file_name);
	void dbg_print_result(bool lines = true);
	void serialize_to_json(const wchar_t* file_name);

protected:
	analyze_data m_analyzed_data;
};

} // end of namespace logan