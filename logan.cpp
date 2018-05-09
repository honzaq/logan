// logan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "file_holder.h"
#include "search.h"
#include "log_line.h"

using namespace logan;

int main()
{
	file_holder fh;
	fh.open(LR"(d:\github\logan\test_files\test1.log)");
	file_holder::encoding bom = fh.get_bom();
	if(bom != file_holder::encoding::utf8) {
		// Currently support only UTF-8
		return 0;
	}

	// Define search text
	std::string needle = "[error";

	const search::occtable_type occ = search::create_occ_table((const uint8_t*)needle.data(), needle.size());
	size_t pos = search::search_in_horspool(fh.data(), fh.data_length(), occ, (const uint8_t*)needle.data(), needle.size());
	if(pos == fh.data_length()) {
		// Nothing found
		return 0;
	}

	log_line ll(fh.data(), fh.data_length(), pos);
	
	auto line = ll.get_line();

	return 0;
}

