#pragma once

#include "analyzer_intf.h"

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
class analyzer 
	: public analyzer_intf
	, protected logger_holder
{
public:
	analyzer(logger_ptr logger);

	void analyze_file(const wchar_t* file_name) override;
	void dbg_print_result(bool lines = true) override;
	
	void serialize_to_json(const wchar_t* file_name) override;
	void parse_from_json(const wchar_t* file_name) override;

	void reset_counters() override;

protected:
	analyze_data m_analyzed_data;
};

} // end of namespace logan