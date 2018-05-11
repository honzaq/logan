// logan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "analyzer.h"
#include "time_measure.h"

using namespace logan;

/*
 * Library use:
 *  https://github.com/gabime/spdlog for logging
 *  https://github.com/nlohmann/json for JSON
 * 
 */

static inline std::string to_utf8(const wchar_t* str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(str);
}

int main()
{
	// Console logger with color
	//auto logger = spdlog::stdout_color_mt("console");
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	auto logger = std::make_shared<spdlog::logger>("msvc_logger", sink);
	logger->set_pattern("%v");

	measure::time parse_log_measure(L"parse log file");

	if(0) {
		analyzer al(logger);
		al.analyze_file(LR"(d:\github\logan\test_files\test2.int.log)");
		al.dbg_print_result(false);
	}
	if(1) {
		analyzer al(logger);
		{
			al.parse_from_json(LR"(d:\github\logan\test_files\result.json)");
		}
		al.analyze_file(LR"(d:\github\logan\test_files\test_half_1.int.log)");
		al.dbg_print_result(false);
	}
	if(1) {
		analyzer al(logger);
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
		analyzer al(logger);
		al.analyze_file(LR"(d:\github\logan\test_files\test_big.int.log)");
		al.dbg_print_result(false);
	}
	//al.analyze_file(LR"(d:\github\logan\test_files\test1.log)");
	//al.analyze_file(LR"(d:\github\logan\test_files\test1.int.log)");
	//al.analyze_file(LR"(d:\github\logan\test_files\test2.int.log)");
	

	parse_log_measure.end_measure([&](const wchar_t* id, double dDiff) {
		logger->info("Measure {} time = {:5.5f}", to_utf8(id).c_str(), dDiff);
	});

// 	auto time = ll.get_time();
// 	auto line = ll.get_line();

	return 0;
}
