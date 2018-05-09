#pragma once

namespace logan {

class log_line
{
public:
	log_line(const uint8_t* data, size_t data_length, size_t pos);

	std::string get_line();

protected:
	void get_full_line(size_t pos);
	void parse();

protected:
	struct text_pos {
		size_t begin = 0;
		size_t end = 0;
	};

	const uint8_t* m_data;
	size_t         m_data_length;
	text_pos       m_line_pos;
};

} // end of namespace logan
