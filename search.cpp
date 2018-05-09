#include "stdafx.h"
#include "search.h"

namespace logan {
namespace search {

const occtable_type create_occ_table(const uint8_t* text, size_t text_length)
{
	occtable_type occ(UCHAR_MAX + 1, text_length); // initialize a table of UCHAR_MAX+1 elements to value text_length
													 /* Populate it with the analysis of the needle */
													 /* But ignoring the last letter */
	if(text_length >= 1) {
		const size_t text_length_minus_1 = text_length - 1;
		for(size_t a = 0; a < text_length_minus_1; ++a) {
			occ[text[a]] = text_length_minus_1 - a;
		}
	}
	return occ;
}

// Returns a pointer to the first byte of text inside data, 
size_t search_in_horspool(const uint8_t* data, size_t data_length, const occtable_type& occ, const uint8_t* text, size_t text_length)
{
	if(text_length > data_length) return data_length;
	if(text_length == 1) {
		const uint8_t* result = (const uint8_t*)::memchr(data, *text, data_length);
		return result ? size_t(result - data) : data_length;
	}

	const size_t text_length_minus_1 = text_length - 1;

	const wchar_t last_needle_char = text[text_length_minus_1];

	size_t data_position = 0;
	while(data_position <= data_length - text_length) {
		const wchar_t occ_char = data[data_position + text_length_minus_1];

		// The author modified this part. Original algorithm matches text right-to-left.
		// This code calls memcmp() (usually matches left-to-right) after matching the last
		// character, thereby incorporating some ideas from
		// "Tuning the Boyer-Moore-Horspool String Searching Algorithm"
		// by Timo Raita, 1992.
		if(last_needle_char == occ_char && ::memcmp(text, data + data_position, text_length_minus_1) == 0) {
			return data_position;
		}

		data_position += occ[occ_char];
	}
	return data_length;
}

} // end of namespace search
} // end of namespace logan