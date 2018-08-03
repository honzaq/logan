// logan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "analyzer.h"
#include "time_measure.h"
#include "config_loader.h"
#include "utils.h"
#include "share_data.h"
#include <filesystem>

namespace fs = std::experimental::filesystem;

using namespace logan;

/*
 * Library use:
 *  https://github.com/gabime/spdlog for logging
 *  https://github.com/nlohmann/json for JSON
 * 
 */

int main()
{
	// Console logger with color
	//auto logger = spdlog::stdout_color_mt("console");
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	auto logger = std::make_shared<spdlog::logger>("msvc_logger", sink);
	logger->set_pattern("%v");

	share_data data;
	data.cfg = std::make_shared<config_loader>();
	data.cfg->load(LR"(d:\github\logan\logan.cfg)");

	//////////////////////////////////////////////////////////////////////////
	std::wstring log_dir = LR"(d:\github\logan\test_files)";
	std::wstring log_dir_enum = LR"(d:\github\logan\test_files\*.*)";
	std::vector<std::wstring> found_files;

	WIN32_FIND_DATA files_data = { 0 };
	HANDLE file_handle = ::FindFirstFile(log_dir_enum.c_str(), &files_data);
	if(file_handle != INVALID_HANDLE_VALUE) {

		do {
			if(!(files_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				found_files.push_back(files_data.cFileName);
				logger->info("FoundFile : {}", to_utf8(files_data.cFileName).c_str());
			}
			
		} while(::FindNextFile(file_handle, &files_data));

		// Close the handle after use or memory/resource leak
		::FindClose(file_handle);
	}
	//////////////////////////////////////////////////////////////////////////

	for(auto file : found_files) {
		auto parser = data.cfg->get_parser(file.c_str());

		std::shared_ptr<analyzer_intf> file_analyzer;
		switch(parser) {
		case logan::parsers::asw:
			file_analyzer = std::make_shared<analyzer>(data, logger);
			break;
		case logan::parsers::asw_short_date:
			break;
		case logan::parsers::unknown:
		default:
			continue;
			break;
		}

		if(!file_analyzer) { continue; }

		measure::time parse_log_measure(std::wstring(L"parse log file " + file).c_str());

		std::wstring analyze_file_path(log_dir + L"\\" + file);
		file_analyzer->parse_from_json(LR"(d:\github\logan\test_files\result.json)");
		file_analyzer->analyze_file(analyze_file_path.c_str());
		file_analyzer->serialize_to_json(LR"(d:\github\logan\test_files\result.json)");

		parse_log_measure.end_measure([&](const wchar_t* id, double dDiff) {
			logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
		});
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	if(0) {
		analyzer al(data, logger);
		al.analyze_file(LR"(d:\github\logan\test_files\test2.int.log)");
		al.dbg_print_result(false);
	}
	if(0) {
		analyzer al(data, logger);
		al.analyze_file(LR"(d:\github\logan\test_files\test_multiline.log)");
		al.dbg_print_result(false);
		al.serialize_to_json(LR"(d:\github\logan\test_files\result.json)");
	}
	if(0) {
		analyzer al(data, logger);
		{
			al.parse_from_json(LR"(d:\github\logan\test_files\result.json)");
		}
		al.analyze_file(LR"(d:\github\logan\test_files\test_half_1.int.log)");
		al.dbg_print_result(false);
	}
	if(0) {
		analyzer al(data, logger);
		al.analyze_file(LR"(d:\github\logan\test_files\test_half_2.int.log)");
		al.dbg_print_result(false);

		measure::time json_measure(L"json serialize");
		al.serialize_to_json(LR"(d:\github\logan\test_files\result.json)");
		{
			//al.reset_after_send();
			//al.serialize_to_json(LR"(d:\github\logan\test_files\result.json)");
		}
		json_measure.end_measure([&](const wchar_t* id, double dDiff) {
			logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
		});
	}

	if(0) {
		// big data
		analyzer al(data, logger);
		al.analyze_file(LR"(d:\github\logan\test_files\test_big.int.log)");
		al.dbg_print_result(false);
	}
	//al.analyze_file(LR"(d:\github\logan\test_files\test1.log)");
	//al.analyze_file(LR"(d:\github\logan\test_files\test1.int.log)");
	//al.analyze_file(LR"(d:\github\logan\test_files\test2.int.log)");
	

	//parse_log_measure.end_measure([&](const wchar_t* id, double dDiff) {
	//	logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
	//});

// 	auto time = ll.get_time();
// 	auto line = ll.get_line();

	return 0;
}
