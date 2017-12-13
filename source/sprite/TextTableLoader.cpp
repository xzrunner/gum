#include "gum/TextTableLoader.h"
#include "gum/StringHelper.h"

#include <sprite2/TextTable.h>

#include <fstream>

namespace gum
{

void TextTableLoader::LoadFromCSV(const CU_STR& filepath)
{
	auto text_table = s2::TextTable::Instance();

	text_table->Clear();

	std::ifstream fin(filepath.c_str());
	CU_STR str_line;
	getline(fin, str_line);	// skip header
	while (getline(fin, str_line))
	{
		CU_VEC<CU_STR> items;
		StringHelper::Split(str_line, ",", items);

		CU_VEC<CU_STR> langs;
		copy(items.begin() + 1, items.end(), back_inserter(langs));
		for (auto& lang : langs) {
			StringHelper::Replace(lang, "\\n", "\n");
		}

		text_table->Insert(items[0], langs);
	}
	fin.close();
}

}