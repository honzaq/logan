#include "stdafx.h"
#include "logger_holder.h"

namespace logan {

logger_ptr g_null_logger = spdlog::create<spdlog::sinks::null_sink_st>("null_logger");

} // end of namespace logan