#ifndef _GUM_DTEX_H_
#define _GUM_DTEX_H_

#include <CU_Singleton.h>

#include <string>
#include <map>
#include <set>

#include <stdint.h>

namespace dtex { class CacheSymbol; }

namespace gum
{

class DTex
{
public:
	void InitHook(void (*draw_begin)(), void (*draw_end)());

	void CreatePkg(int pkg_id);

	void LoadSymStart();
	void LoadSymbol(const std::string& filepath, int tex_id, int tex_w, int tex_h);
	void LoadSymFinish();
	const float* QuerySymbol(const std::string& filepath, int* tex_id) const;

	void Clear();

	void DebugDraw() const;

private:
	struct Filepath2ID
	{
		uint32_t Add(const std::string& filepath);
		uint32_t Query(const std::string& filepath) const;

		static uint32_t HashStr(const std::string& str);

		std::map<std::string, int> map_filepath;
		std::set<int> set_id;

		std::map<std::string, int> conflict;
	};

private:
	dtex::CacheSymbol* m_c2;
	Filepath2ID m_file2id;

	SINGLETON_DECLARATION(DTex)

}; // DTex

}

#endif // _GUM_DTEX_H_
