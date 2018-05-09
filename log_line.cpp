#include "stdafx.h"
#include "log_line.h"

namespace logan {

log_line::log_line(const uint8_t* data, size_t data_length, size_t pos)
	: m_data(data)
	, m_data_length(data_length)
{
	get_full_line(pos);
	parse();
}

void log_line::get_full_line(size_t pos)
{
	if(pos > m_data_length) {
		throw std::exception("Position is after data length");
	}

	//////////////////////////////////////////////////////////////////////////
	// Search for log 'line' begin. Line begin with '\r\n + [' or BOF + [
	auto search_pos = pos;
	m_line_pos.begin = 0;
	byte found_chars = 0;
	while(search_pos > 0) {
		if(found_chars == 0 && m_data[search_pos] == '[') {
			found_chars = 1;
		}
		else if(found_chars == 1 && m_data[search_pos] == '\n') {
			found_chars = 2;
		}
		else if(found_chars == 2 && m_data[search_pos] == '\r') {
			m_line_pos.begin = search_pos + found_chars; // + 2 characters \r\n (point to [)
			break;
		}
		else {
			found_chars = 0;
		}
		search_pos--;
	}

	//////////////////////////////////////////////////////////////////////////
	// Search for log 'line' end (can be multi line text). Line end with (new line) '\r\n[' or OEF
	search_pos = pos;
	m_line_pos.end = m_data_length;
	found_chars = 0;
	while(search_pos < m_data_length) {
		if(found_chars == 0 && m_data[search_pos] == '\r') {
			found_chars = 1;
		}
		else if(found_chars == 1 && m_data[search_pos] == '\n') {
			found_chars = 2;
		}
		else if(found_chars == 2 && m_data[search_pos] == '[') {
			m_line_pos.end = search_pos - found_chars; // - 2 characters \r\n
			break;
		}
		else {
			found_chars = 0;
		}
		search_pos++;
	}
}

std::string log_line::get_line()
{
	return std::string(reinterpret_cast<const char*>(m_data + m_line_pos.begin), m_line_pos.end - m_line_pos.begin);
}

// digits only
size_t to_int(const char* str, size_t len) {

	int n = 0;
	while(len--) {
		n = n * 10 + *str++ - '0';
	}
	return n;
}

void log_line::parse()
{
	// Log line description
	//  sample: [2018-05-02 10:06:47.900] [error  ] [modul1     ] [  308: 4616] Somewhere: File could not be opened. Code 0.
	//          [date max 23chars       ]
	//                                    [severity max 7 chars]
	//                                              [logger max 11chars]
	//                                                            [PID:THID max 5chars each]
	//                                                                          Log text

	//////////////////////////////////////////////////////////////////////////
	// Date
	text_pos time_pos;
	if(m_line_pos.end > m_line_pos.begin + 24 && m_data[m_line_pos.begin] == '[' && m_data[m_line_pos.begin + 24] == ']') {
		time_pos.begin = m_line_pos.begin + 1;
		time_pos.end = time_pos.begin + 23;
	}
	else {
		throw std::exception("unsupported log format, date");
	}
	
	//std::string time(reinterpret_cast<const char*>(m_data + time_pos.begin), time_pos.end - time_pos.begin);
	{
		// to SystemTime
		SYSTEMTIME sys_time = { 0 };
		// 2018-05-02 10:06:47.900
		sys_time.wYear = static_cast<WORD>(to_int(reinterpret_cast<const char*>(m_data + time_pos.begin), 4));
		sys_time.wMonth = static_cast<WORD>(to_int(reinterpret_cast<const char*>(m_data + time_pos.begin + 5), 2));
		sys_time.wDay = static_cast<WORD>(to_int(reinterpret_cast<const char*>(m_data + time_pos.begin + 8), 2));

		sys_time.wHour = static_cast<WORD>(to_int(reinterpret_cast<const char*>(m_data + time_pos.begin + 11), 2));
		sys_time.wMinute = static_cast<WORD>(to_int(reinterpret_cast<const char*>(m_data + time_pos.begin + 14), 2));
		sys_time.wSecond = static_cast<WORD>(to_int(reinterpret_cast<const char*>(m_data + time_pos.begin + 17), 2));
		sys_time.wMilliseconds = static_cast<WORD>(to_int(reinterpret_cast<const char*>(m_data + time_pos.begin + 20), 3));
	}
	
	
	
	
}

} // end of namespace logan