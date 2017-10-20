#ifndef _GUM_TEXTBOX_LOADER_H_
#define _GUM_TEXTBOX_LOADER_H_

#include <cu/uncopyable.h>

#include <json/json.h>

#include <string>

namespace s2 { class Textbox; }
namespace simp { class NodeLabel; }

namespace gum
{

class TextboxLoader : private cu::Uncopyable
{
public:
	TextboxLoader(s2::Textbox& tb);

	void LoadJson(const Json::Value& val);
	void LoadBin(const simp::NodeLabel* node);

private:
	s2::Textbox& m_tb;

}; // TextboxLoader

}

#endif // _GUM_TEXTBOX_LOADER_H_
