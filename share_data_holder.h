#pragma once

#include <memory>
#include "share_data.h"

namespace logan {

/*! Logger class holder */
class share_data_holder
{
public:
	share_data_holder(share_data& data)
		: m_data(data)
	{}

	/*! Return current shared data */
	share_data& sh_data() {
		return m_data;
	}

private:
	share_data& m_data;
};

} // end of namespace logan
