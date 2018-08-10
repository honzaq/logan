#pragma once

#include "share_data_holder.h"
#include "data.h"

namespace logan {

//////////////////////////////////////////////////////////////////////////
// Analyzer class
//////////////////////////////////////////////////////////////////////////
class analyzer 
	: protected share_data_holder
	, protected logger_holder
{
public:
	analyzer(share_data& data, logger_ptr logger);

	void analyze_file(const wchar_t* file_path);
	void analyze_data(const uint8_t* data, uint64_t data_size, const std::string& name);
	void dbg_print_result(bool lines = true);

	const analyzed_data& result() const;
	
// 	bool serialize_to_json(const wchar_t* file_path);
// 	bool parse_from_json(const wchar_t* file_path);

	void reset_counters();

protected:
	analyzed_data m_analyzed_data;
};

} // end of namespace logan