#pragma once

namespace logan {

struct text_pos {
	size_t begin = 0;
	size_t end = 0;

	inline size_t len() const {
		return end - begin;
	}
};

class log_line
{
public:
	log_line();

	/* Parse line from data and found possition of searched string */
	void parse(const uint8_t* data, size_t data_length, size_t pos);

	/* Will trim new line and followed white spaces from msg */
	void trim_msg_whitespaces(bool val);

	/* Return full raw line without change */
	std::string get_raw_line();
	/* Return log time */
	FILETIME get_time();
	/* Return log message */
	std::string get_msg();

protected:
	/* from position found entire log line */
	void full_line_from_pos(size_t pos);
	/* parse found line */
	void parse_found_line();

	/* skip all spaces at the string begin (move begin pointer)*/
	void trim_begin_space(text_pos& val_pos);
	/* skip all spaces at the string end (move end pointer)*/
	void trim_end_space(text_pos& val_pos);

protected:
	const uint8_t* m_data;
	size_t         m_data_length;
	text_pos       m_line_pos;
	bool           m_trim_msg_whitespaces = false;
};

} // end of namespace logan
