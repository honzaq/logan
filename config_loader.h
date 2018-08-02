#pragma once

namespace logan {

class config_loader
{
public:
	bool load(const wchar_t* file_name);

protected:
	json m_cfg;
};

} // end of namespace logan