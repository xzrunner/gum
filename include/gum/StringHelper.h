#ifndef _GUM_STRING_HELPER_H_
#define _GUM_STRING_HELPER_H_

#include <cu/cu_stl.h>

#include <sstream>
#include <algorithm>
#include <fstream>

#include <stdint.h>

namespace gum
{

class StringHelper
{
public:
	template<typename T>
	static CU_STR ToString(const T& d) {
		std::stringstream ss;
		ss << d;
		return CU_STR(ss.str());
	}

	template<typename T>
	static void FromString(const CU_STR& str, T& ret) {
		std::stringstream ss(str.c_str());
		ss >> ret;
	}

	template<typename T>
	static T FromString(const CU_STR& str) {
		std::stringstream ss(str);
		T ret;
		ss >> ret;
		return ret;
	}

	static void ToLower(CU_STR& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}

	static void ToUpper(CU_STR& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	}

	static CU_STR GetLine(std::ifstream& fin)
	{
		CU_STR line;
		while ((line.empty() || line == " ") && getline(fin, line))
			;
		return line;
	}

	static void Split(const CU_STR& src, const CU_STR& mid, 
		CU_VEC<CU_STR>& dst);

//	static CU_STR ToUtf8(const CU_STR& str);

	static CU_STR UTF8ToGBK(const CU_STR& str);
	static CU_STR GBKToUTF8(const CU_STR& str);

	static CU_STR UTF8ToGBK(const char * c_str);
	static CU_STR GBKToUTF8(const char * c_str);

	static CU_STR FromChar(const char* c_str);

}; // StringHelper

}

#endif // _GUM_STRING_HELPER_H_
