#pragma once

//#include <map>
//#include <string>

namespace logan {

template <typename T>
class enum_parser
{
public:
	enum_parser() {};
	T toEnum(const std::string& value) {
 		const auto it = enum_map.find(value);
 		if(it == enum_map.end()) {
			throw std::runtime_error("unknown enum type");
 		}
 		return it->second;
	};
	std::string fromEnum(T value) {
		auto it = std::find_if(enum_map.begin(), enum_map.end(), [value](auto&& item) -> bool { return item.second == value; });
		if(it == enum_map.end()) {
			throw std::runtime_error("unknown enum type");
		}
		return it->first;
	}
private:
	std::map<std::string, T> enum_map;
};

enum class parsers {
	unknown = 0,
	asw,
	asw_short_date
};
enum_parser<parsers>::enum_parser()
{
	enum_map["unknown"]        = parsers::unknown;
	enum_map["asw"]            = parsers::asw;
	enum_map["asw_short_date"] = parsers::asw_short_date;
}

} // end of namespace logan
