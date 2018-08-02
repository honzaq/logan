#pragma once

#include <memory>
#include "spdlog/spdlog.h"

namespace logan {

/*! Define logger pointer constant */
using logger_ptr = std::shared_ptr<spdlog::logger>;

/*! Define empty logger so logger()->log does not crash */
extern logger_ptr g_null_logger;

/*! Logger class holder */
class logger_holder
{
public:
	logger_holder(logger_ptr logger)
		: m_logger(logger ? logger : g_null_logger)
	{}
	
	/*! Return current logger */
	auto logger() const {
		return m_logger;
	}

private:
	logger_ptr m_logger;
};

} // end of namespace logan
