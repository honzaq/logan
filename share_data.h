#pragma once

#include <memory>
#include "config_loader.h"

namespace logan {

struct share_data
{
	std::shared_ptr<config_loader> cfg;
};

} // end of namespace logan