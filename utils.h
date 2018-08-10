#pragma once

#include <string>
#include <codecvt>

namespace logan {

static inline std::string to_utf8(const wchar_t* str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(str);
}

static inline std::wstring to_utf16(const char* str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	return converter.from_bytes(str);
}

static inline bool wildchar_compare(const char* wildstring, const char* matchstring)
{
	// https://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing

	char stopstring[1] = { 0 };
	//const char *wildstringNew = wildstring;

	while(*matchstring != '\0') {

		if(*wildstring == '*') {
			do {
				wildstring++;
			} while(*wildstring == '*');   // if a dork entered two or more * in a row, ignore them and go ahead

			if(*wildstring == '\0') {      // if * was the last char, the strings are equal
				return true;
			}
			else {
				*stopstring = *wildstring; // the next char to check after the *
			}
		}

		if(*stopstring != '\0') {
			if((*stopstring == *matchstring) || (*stopstring == '?')) {
				wildstring++;
				*stopstring = '\0';
			}
			matchstring++;
		}
		else {
			if((*wildstring == *matchstring) || (*wildstring == '?')) {
				wildstring++;
				matchstring++;
			}
			else {
// 				if(*wildstring != '\0') {
// 					wildstring = wildstringNew;
// 				}
// 				else {
					return false;
//				}
			}
		}

		if((*matchstring == '\0') && (*wildstring != '\0')) {
			// matchstring seems to be too short. Check if wildstring has any more chars except '*'
			while(*wildstring == '*') {  // ignore following '*'
				wildstring++;
			}

			if(*wildstring == '\0') {    // if wildstring end after '*', strings are equal
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}

} // end of namespace logan
