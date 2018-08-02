#pragma once

namespace logan {

class analyzer_intf
{
public:
	virtual void analyze_file(const wchar_t* file_name) = 0;
	virtual void dbg_print_result(bool lines = true) = 0;

	virtual void serialize_to_json(const wchar_t* file_name) = 0;
	virtual void parse_from_json(const wchar_t* file_name) = 0;

	virtual void reset_counters() = 0;
};

} // end of namespace logan