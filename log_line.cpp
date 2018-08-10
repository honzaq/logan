#include "stdafx.h"
#include "log_line.h"

namespace logan {

log_line::log_line()
{
}

void log_line::parse(const uint8_t* data, size_t data_length, size_t pos)
{
	m_data = data;
	m_data_length = data_length;
	full_line_from_pos(pos);
}

void log_line::trim_msg_whitespaces(bool val)
{
	m_trim_msg_whitespaces = val;
}

void log_line::full_line_from_pos(size_t pos)
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

std::string log_line::get_raw_line()
{
	return std::string(reinterpret_cast<const char*>(m_data + m_line_pos.begin), m_line_pos.len());
}

// digits only
size_t to_int(const unsigned char* str, size_t len)
{
	size_t n = 0;
	while(len--) {
		if(*str == ' ') {
			++str;
			continue;
		}
		n = n * 10 + *str++ - '0';
	}
	return n;
}

void log_line::trim_begin_space(text_pos& val_pos)
{
	while(val_pos.begin <= val_pos.end && m_data[val_pos.begin] == ' ') {
		++val_pos.begin;
	}
}

void log_line::trim_end_space(text_pos& val_pos)
{
	while(val_pos.end > val_pos.begin && m_data[--val_pos.end] == ' ') {};

	++val_pos.end;
}

void log_line::parse_found_line()
{
	// Log line description
	//  sample: [2018-05-02 10:06:47.900] [error  ] [modul1     ] [  308: 4616] Somewhere: File could not be opened. Code 0.
	//          [date max 23chars       ]
	//                                    [severity max 7 chars]
	//                                              [logger max 11chars]
	//                                                            [PID:THID max 5chars each]
	//                                                                          Log text

	if(m_line_pos.len() < 64) {
		throw std::exception("unsupported log format");
	}

	//////////////////////////////////////////////////////////////////////////
	// Date
	text_pos time_pos;
	time_pos.begin = m_line_pos.begin + 1;
	time_pos.end = time_pos.begin + 23;
	{
		//std::string time(reinterpret_cast<const char*>(m_data + time_pos.begin), time_pos.end - time_pos.begin);
		// to SystemTime
// 		SYSTEMTIME sys_time = { 0 };
// 		// 2018-05-02 10:06:47.900
// 		sys_time.wYear = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin), 4));
// 		sys_time.wMonth = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 5), 2));
// 		sys_time.wDay = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 8), 2));
// 
// 		sys_time.wHour = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 11), 2));
// 		sys_time.wMinute = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 14), 2));
// 		sys_time.wSecond = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 17), 2));
// 		sys_time.wMilliseconds = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 20), 3));
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Severity
	text_pos severity_pos;
	severity_pos.begin = m_line_pos.begin + 27;
	severity_pos.end = severity_pos.begin + 7;
	trim_end_space(severity_pos);
	{
		//std::string severity(reinterpret_cast<const char*>(m_data + severity_pos.begin), severity_pos.len());
	}

	//////////////////////////////////////////////////////////////////////////
	// Logger
	text_pos logger_pos;
	logger_pos.begin = m_line_pos.begin + 37;
	logger_pos.end = logger_pos.begin + 11;
	trim_end_space(logger_pos);
	{
		//std::string logger(reinterpret_cast<const char*>(m_data + logger_pos.begin), logger_pos.len());
	}

	//////////////////////////////////////////////////////////////////////////
	// PID
	text_pos pid_pos;
	pid_pos.begin = m_line_pos.begin + 51;
	pid_pos.end = pid_pos.begin + 5;
	trim_begin_space(pid_pos);
	std::string spid(reinterpret_cast<const char*>(m_data + pid_pos.begin), pid_pos.len());

	{
		//uint32_t pid = static_cast<uint32_t>(to_int(reinterpret_cast<const unsigned char*>(m_data + pid_pos.begin), pid_pos.len()));
	}

	//////////////////////////////////////////////////////////////////////////
	// THID
	text_pos thid_pos;
	thid_pos.begin = m_line_pos.begin + 57;
	thid_pos.end = thid_pos.begin + 5;
	trim_begin_space(thid_pos);
	std::string sthid(reinterpret_cast<const char*>(m_data + thid_pos.begin), thid_pos.len());

	{
		//uint32_t thid = static_cast<uint32_t>(to_int(reinterpret_cast<const unsigned char*>(m_data + thid_pos.begin), thid_pos.len()));
	}

	text_pos msg_pos;
	msg_pos.begin = m_line_pos.begin + 64;
	msg_pos.end = m_line_pos.end;
	{
		//std::string msg(reinterpret_cast<const char*>(m_data + msg_pos.begin), msg_pos.len());
	}
}

FILETIME log_line::get_time()
{
	if(m_line_pos.len() < 64) {
		throw std::exception("unsupported log format");
	}

	text_pos time_pos;
	time_pos.begin = m_line_pos.begin + 1;
	time_pos.end = time_pos.begin + 23;

	// to SystemTime
	SYSTEMTIME sys_time = { 0 };
	sys_time.wYear = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin), 4));
	sys_time.wMonth = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 5), 2));
	sys_time.wDay = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 8), 2));

	sys_time.wHour = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 11), 2));
	sys_time.wMinute = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 14), 2));
	sys_time.wSecond = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 17), 2));
	sys_time.wMilliseconds = static_cast<WORD>(to_int(reinterpret_cast<const unsigned char*>(m_data + time_pos.begin + 20), 3));

	FILETIME ft;
	::SystemTimeToFileTime(&sys_time, &ft);

	return ft;
}

inline bool is_white_char(char ch) {
	return ch == 32 || ch == 160 || ch == '\n' || ch == '\r' || ch == '\t';
}

std::string log_line::get_msg()
{
	text_pos msg_pos;
	msg_pos.begin = m_line_pos.begin + 64;
	msg_pos.end = m_line_pos.end;

	if(m_trim_msg_whitespaces) {

		std::string msg;
		msg.reserve(msg_pos.len() + 20); // + some extra reserve for ' ¶ '

		//////////////////////////////////////////////////////////////////////////
		// trim from start
		text_pos trim_pos = msg_pos;

		do {
			while(trim_pos.begin < msg_pos.end && is_white_char(m_data[trim_pos.begin])) {
				++trim_pos.begin;
			}

			//////////////////////////////////////////////////////////////////////////
			// search for new line (can be in middle of text)
			text_pos new_line_pos;
			trim_pos.end = trim_pos.begin;
			while(trim_pos.end < msg_pos.end) {
				if(m_data[trim_pos.end] == '\r') {
					new_line_pos.begin = new_line_pos.end = trim_pos.end;
				}
				else if(m_data[trim_pos.end] == '\n' && new_line_pos.begin > 0) {
					new_line_pos.end = trim_pos.end;
					break;
				}
				else if(new_line_pos.begin > 0) { // reset found '\r'
					new_line_pos.begin = new_line_pos.end = 0;
				}
				++trim_pos.end;
			}

			// we found new line
			if(new_line_pos.begin != new_line_pos.end) {
				trim_pos.end = new_line_pos.begin;
				// Append space as new line separator replacement 
				if(!msg.empty()) msg += " ¶ ";
				// Append line to msg
				msg += std::string(reinterpret_cast<const char*>(m_data + trim_pos.begin), trim_pos.len());

				// move after new line
				trim_pos.begin = trim_pos.end = new_line_pos.end + 1;
			}
			else {
				if(!msg.empty()) msg += " ¶ ";
				msg += std::string(reinterpret_cast<const char*>(m_data + trim_pos.begin), trim_pos.len());
			}
		} while(trim_pos.end < msg_pos.end);

		return msg;
	}

	// return raw msg
	return std::string(reinterpret_cast<const char*>(m_data + msg_pos.begin), msg_pos.len());
}

} // end of namespace logan