// logan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "analyzer.h"
#include "time_measure.h"
#include "config_loader.h"
#include "utils.h"
#include "share_data.h"
#include "archive_zip.h"
#include "archive_gz.h"
#include "archive_lzma.h"
#include <filesystem>

namespace fs = std::experimental::filesystem;

using namespace logan;

/*
 * Used library:
 *  https://github.com/gabime/spdlog for logging
 *  http://rapidjson.org for JSON
 *  https://zlib.net for GZ unzip
 */

int main()
{
	// Console logger with color
	//auto logger = spdlog::stdout_color_mt("console");
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	auto logger = std::make_shared<spdlog::logger>("msvc_logger", sink);
	logger->set_pattern("%v");

	//////////////////////////////////////////////////////////////////////////
	// Load config
	measure::time cfg_load_measure(L"cfg load");
	
	share_data sdata;
	sdata.cfg = std::make_shared<config_loader>(logger);
	sdata.cfg->load(LR"(d:\github\logan\logan.cfg)");

	cfg_load_measure.end_measure([&](const wchar_t* id, double dDiff) {
		logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
	});

	//////////////////////////////////////////////////////////////////////////
	// Get cmdline param

	std::string file_path;

	int nArgs = 0;
	LPWSTR* szArglist = ::CommandLineToArgvW(::GetCommandLineW(), &nArgs);
	if(szArglist != nullptr) {
		if(nArgs >= 1) {
			file_path = to_utf8(szArglist[1]);
		}
		::LocalFree(szArglist);
	}

	if(file_path.empty()) {
		logger->error("Missing command line parameter for source file.");
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// parse .tar.gz
	if(1) {
		measure::time enum_measure(L"analyze whole .tar.gz");

		//file_path = R"(c:\Download\1533301033116-758eb8fcc47a4058bd2d8a3c7849a053.tar.gz)";

		archive_gz arch_gz(logger);
		arch_gz.open(file_path.c_str());

		global_data final_data;

		arch_gz.enumerate([logger, &sdata, &final_data](const std::string& name, const uint8_t* data, uint64_t data_size) -> bool {

			logger->info("Analyze {}", name.c_str());

			std::shared_ptr<analyzer> file_analyzer;
			file_analyzer = std::make_shared<analyzer>(sdata, logger);

			file_analyzer->analyze_data(data, data_size, name);
			const auto& result = file_analyzer->result();
			final_data.append(result);

			return true;
		});

		// get report file path
		auto file_name = fs::path(file_path).filename();
		while(file_name.has_extension()) {
			file_name = file_name.stem();
		}

		data_export exporter(logger);
		auto report_file_name = fs::path(file_path).replace_filename(file_name).replace_extension("report"); 
		exporter.serialize_to_json(report_file_name.c_str(), final_data);

		enum_measure.end_measure([&](const wchar_t* id, double dDiff) {
			logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
		});
	}

	//////////////////////////////////////////////////////////////////////////
// 	std::wstring log_dir = LR"(d:\github\logan\test_files)";
// 	std::wstring log_dir_enum = LR"(d:\github\logan\test_files\*.*)";
// 	std::vector<std::wstring> found_files;
// 
// 	WIN32_FIND_DATA files_data = { 0 };
// 	HANDLE file_handle = ::FindFirstFile(log_dir_enum.c_str(), &files_data);
// 	if(file_handle != INVALID_HANDLE_VALUE) {
// 
// 		do {
// 			if(!(files_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
// 				found_files.push_back(files_data.cFileName);
// 				logger->info("FoundFile : {}", to_utf8(files_data.cFileName).c_str());
// 			}
// 			
// 		} while(::FindNextFile(file_handle, &files_data));
// 
// 		// Close the handle after use or memory/resource leak
// 		::FindClose(file_handle);
// 	}
	//////////////////////////////////////////////////////////////////////////

// 	for(auto file : found_files) {
// 		auto parser = sdata.cfg->get_parser(file.c_str());
// 
// 		std::shared_ptr<analyzer_intf> file_analyzer;
// 		switch(parser) {
// 		case logan::parsers::asw:
// 			file_analyzer = std::make_shared<analyzer>(sdata, logger);
// 			break;
// 		case logan::parsers::asw_short_date:
// 			break;
// 		case logan::parsers::unknown:
// 		default:
// 			continue;
// 			break;
// 		}
// 
// 		if(!file_analyzer) { continue; }
// 
// 		measure::time parse_log_measure(std::wstring(L"parse log file " + file).c_str());
// 
// 		std::wstring analyze_file_path(log_dir + L"\\" + file);
// 		file_analyzer->parse_from_json(LR"(d:\github\logan\test_files\result.json)");
// 		file_analyzer->analyze_file(analyze_file_path.c_str());
// 		file_analyzer->serialize_to_json(LR"(d:\github\logan\test_files\result.json)");
// 
// 		parse_log_measure.end_measure([&](const wchar_t* id, double dDiff) {
// 			logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
// 		});
// 	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

//  	if(0) {
//  		analyzer al(sdata, logger);
//  		al.analyze_file(LR"(d:\github\logan\test_files\test1.log)");
//  		al.dbg_print_result(true);
//  	}
// 	if(0) {
// 		analyzer al(sdata, logger);
// 		al.analyze_file(LR"(d:\github\logan\test_files\test_multiline.log)");
// 		al.dbg_print_result(false);
// 		al.serialize_to_json(LR"(d:\github\logan\test_files\result.json)");
// 	}
// 	if(0) {
// 		analyzer al(sdata, logger);
// 		{
// 			al.parse_from_json(LR"(d:\github\logan\test_files\result.json)");
// 		}
// 		al.analyze_file(LR"(d:\github\logan\test_files\test_half_1.int.log)");
// 		al.dbg_print_result(false);
// 	}
// 	if(0) {
// 		analyzer al(sdata, logger);
// 		al.analyze_file(LR"(d:\github\logan\test_files\test_half_2.int.log)");
// 		al.dbg_print_result(false);
// 
// 		measure::time json_measure(L"json serialize");
// 		al.serialize_to_json(LR"(d:\github\logan\test_files\result.json)");
// 		{
// 			//al.reset_after_send();
// 			//al.serialize_to_json(LR"(d:\github\logan\test_files\result.json)");
// 		}
// 		json_measure.end_measure([&](const wchar_t* id, double dDiff) {
// 			logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
// 		});
// 	}

// 	if(0) {
// 		// big data
// 		analyzer al(sdata, logger);
// 		al.analyze_file(LR"(d:\github\logan\test_files\test_big.int.log)");
// 		al.dbg_print_result(false);
// 	}
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
