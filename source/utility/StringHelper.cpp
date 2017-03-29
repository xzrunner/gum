#include "StringHelper.h"

// #include <boost/locale/encoding.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>

//#include <Windows.h>

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

namespace gum
{

void StringHelper::Split(const std::string& src, const std::string& mid, 
						std::vector<std::string>& dst)
{
	char* cstr = new char [src.length()+1];
	std::strcpy(cstr, src.c_str());
	char* p = std::strtok(cstr, mid.c_str());
	while (p!=0)
	{
		dst.push_back(std::string(p));
		p = strtok(NULL, mid.c_str());
	}
	delete[] cstr;
}


static std::string code_convert(const char* source_charset, const char* to_charset, const std::string& str)
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

    std::string ret = "";
    size_t ncov = libiconv(cd, &inbuff, &inbuff_len, &outbuff, &outbuff_len);
    if(ncov != (size_t)-1 ) {
        size_t len = raw_outbuff_len-outbuff_len;
        ret = std::string(raw_outbuff, len);
    }else {
    	printf("iconv error !!!\n");
    }

    free(raw_outbuff);
    libiconv_close(cd);
    return ret;
}

//std::string StringHelper::ToUtf8(const std::string& str)
//{
//	int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(),
//		str.length(), NULL, 0);
//	std::wstring utf16_str(size, '\0');
//	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str.c_str(),
//		str.length(), &utf16_str[0], size);
//
//	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
//		utf16_str.length(), NULL, 0,
//		NULL, NULL);
//	std::string utf8_str(utf8_size, '\0');
//	WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
//		utf16_str.length(), &utf8_str[0], utf8_size,
//		NULL, NULL);
//	return utf8_str;
//}

std::string StringHelper::UTF8ToGBK(const std::string& str)
{
	return code_convert("UTF-8", "GBK", str);
	// return boost::locale::conv::from_utf(str, "GBK");
}

std::string StringHelper::GBKToUTF8(const std::string& str)
{
	return code_convert("GBK", "UTF-8", str);
	// return boost::locale::conv::to_utf<char>(str, "GBK");
}

}
