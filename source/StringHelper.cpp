#include "gum/StringHelper.h"

#ifndef NO_BOOST
	#include <boost/locale/encoding.hpp>
#endif // NO_BOOST

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>

//#include <Windows.h>

#ifdef NO_BOOST
extern "C" {
	typedef void* iconv_t;

	#if defined(__APPLE__)
		#define libiconv_open iconv_open 
		#define libiconv iconv
		#define libiconv_close iconv_close
	#endif
	extern iconv_t libiconv_open (const char* tocode, const char* fromcode);
	extern size_t libiconv (iconv_t cd,  char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft);
	extern int libiconv_close (iconv_t cd);
}
#endif // NO_BOOST

namespace gum
{

void StringHelper::Split(const CU_STR& src, const CU_STR& mid, 
						CU_VEC<CU_STR>& dst)
{
	char* cstr = new char [src.length()+1];
	std::strcpy(cstr, src.c_str());
	char* p = std::strtok(cstr, mid.c_str());
	while (p!=0)
	{
		dst.push_back(CU_STR(p));
		p = strtok(nullptr, mid.c_str());
	}
	delete[] cstr;
}

void StringHelper::Replace(CU_STR& str, const CU_STR& from, const CU_STR& to)
{
	if (from.empty()) {
		return;
	}
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

#ifdef NO_BOOST

static CU_STR code_convert(const char* source_charset, const char* to_charset, const CU_STR& str)
{
    iconv_t cd = libiconv_open(to_charset, source_charset);
    if (cd == (iconv_t)-1) {
    	printf("iconv_open error  %d %s %s!!!\n", errno, source_charset, to_charset);
        return "";
    }

    size_t inbuff_len = str.length();
    char* inbuff = (char*)str.c_str();

    size_t raw_outbuff_len = str.size()*4;
    char* raw_outbuff = (char*)malloc(raw_outbuff_len);
    char* outbuff = raw_outbuff;
    size_t outbuff_len = raw_outbuff_len;

    CU_STR ret = "";
    size_t ncov = libiconv(cd, &inbuff, &inbuff_len, &outbuff, &outbuff_len);
    if(ncov != (size_t)-1 ) {
        size_t len = raw_outbuff_len-outbuff_len;
        ret = CU_STR(raw_outbuff, len);
    }else {
    	printf("iconv error !!!\n");
    }

    free(raw_outbuff);
    libiconv_close(cd);
    return ret;
}

#endif // NO_BOOST

//CU_STR StringHelper::ToUtf8(const CU_STR& str)
//{
//	int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(),
//		str.length(), nullptr, 0);
//	std::wstring utf16_str(size, '\0');
//	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(),
//		str.length(), &utf16_str[0], size);
//
//	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
//		utf16_str.length(), nullptr, 0,
//		nullptr, nullptr);
//	CU_STR utf8_str(utf8_size, '\0');
//	WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
//		utf16_str.length(), &utf8_str[0], utf8_size,
//		nullptr, nullptr);
//	return utf8_str;
//}

CU_STR StringHelper::UTF8ToGBK(const char *c_str)
{
	if(!c_str) {
		return CU_STR();
	} else {
		return UTF8ToGBK(CU_STR(c_str));
	}
}

CU_STR StringHelper::GBKToUTF8(const char *c_str)
{
	if(!c_str) {
		return CU_STR();
	} else {
		return GBKToUTF8(CU_STR(c_str));
	}
}

CU_STR StringHelper::UTF8ToGBK(const CU_STR& str)
{
#ifdef NO_BOOST
	return code_convert("UTF-8", "GBK", str);
#else
	std::string std_str = boost::locale::conv::from_utf(str.c_str(), "GBK");
	return CU_STR(std_str.c_str());
#endif // NO_BOOST
}

CU_STR StringHelper::GBKToUTF8(const CU_STR& str)
{
#ifdef NO_BOOST
	return code_convert("GBK", "UTF-8", str);
#else
	std::string std_str = boost::locale::conv::to_utf<char>(str.c_str(), "GBK");
	return CU_STR(std_str.c_str());
#endif // NO_BOOST
}

CU_STR StringHelper::FromChar(const char* c_str)
{
	CU_STR str;
	if (c_str) {
		str.assign(c_str);
	}
	return str;
}

}
