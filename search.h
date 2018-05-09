#pragma once

// search used from: https://github.com/FooBarWidget/boyer-moore-horspool

namespace logan {
namespace search {

using occtable_type = std::vector<size_t>;

/* This function creates an occ table to be used by the search algorithms. */
/* It only needs to be created once per a text to search. */
const occtable_type create_occ_table(const uint8_t* text, size_t text_length);

// Returns a pointer to the first byte of needle inside haystack, 
size_t search_in_horspool(const uint8_t* data, size_t data_length, const occtable_type& occ, const uint8_t* text, size_t text_length);

} // end of namespace search
} // end of namespace logan