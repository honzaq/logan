// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>

// Stl
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <codecvt>
#include <fstream>
#include <memory>

// Logger
#include "spdlog/spdlog.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/null_sink.h"

// Json
#include "rapidjson/rapidjson.h"

// Our
#include "scope_guard.h"
#include "logger_holder.h"
#include "time_measure.h"
#include "utils.h"
