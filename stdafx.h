// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <codecvt>
#include <fstream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/null_sink.h"

#include "logger_holder.h"

#pragma warning ( push )
#pragma warning ( disable : 4127 )
#include "nlohmann/json.hpp"
#pragma warning ( pop )

using json = nlohmann::json;
