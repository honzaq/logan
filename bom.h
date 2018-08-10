#pragma once
#include <stdint.h>

namespace logan {

enum class encoding {
	none = 0,
	utf8,
	utf16_le,
	utf16_be
};

constexpr encoding detect_bom(const uint8_t* data, const uint64_t data_size) {

	constexpr uint8_t UTF16_BOM_LE[] = { uint8_t(0xFF), uint8_t(0xFE) };
	constexpr uint8_t UTF16_BOM_BE[] = { uint8_t(0xFE), uint8_t(0xFF) };
	constexpr uint8_t UTF8_BOM[] = { uint8_t(0xEF), uint8_t(0xBB), uint8_t(0xBF) };

	encoding bom = encoding::none;
	if(data_size >= 2 && data[0] == UTF16_BOM_LE[0] && data[1] == UTF16_BOM_LE[1]) {
		bom = encoding::utf16_le;
	}
	else if(data_size >= 2 && data[0] == UTF16_BOM_BE[0] && data[1] == UTF16_BOM_BE[1]) {
		bom = encoding::utf16_be;
	}
	else if(data_size >= 3 && data[0] == UTF8_BOM[0] && data[1] == UTF8_BOM[1] && data[2] == UTF8_BOM[2]) {
		bom = encoding::utf8;
	}
	else {
		bom = encoding::none;
	}
	return bom;
}

constexpr uint8_t bom_length(encoding enc)
{
	switch(enc) {
	case encoding::utf8:
		return 3;
		//break;
	case encoding::utf16_le:
	case encoding::utf16_be:
		return 2;
		//break;
	}
	return 0;
}

} // end of namespace logan