#ifndef _GUM_DTEX_H_
#define _GUM_DTEX_H_

#include "ResourceUID.h"

#include <CU_Singleton.h>

#include <string>
#include <map>
#include <set>

#include <stdint.h>

namespace dtex { class CacheSymbol; class CacheGlyph; class Texture; struct Rect; }

namespace gum
{

class DTex
{
public:
	void InitHook(void (*draw_begin)(), void (*draw_end)());

	// C3
	void CreatePkg(int pkg_id);

	// C2
	void LoadSymStart();
	void LoadSymbol(UID sym_id, int tex_id, int tex_w, int tex_h);
	void LoadSymFinish();
	const float* QuerySymbol(UID sym_id, int* tex_id) const;

	// CG
	void DrawGlyph(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key);
	void LoadGlyph(uint32_t* bitmap, int width, int height, uint64_t key);

	void Clear();

	void Flush();

	void DebugDraw() const;

private:
	dtex::CacheSymbol* m_c2;

	dtex::CacheGlyph* m_cg;

	SINGLETON_DECLARATION(DTex)

}; // DTex

}

#endif // _GUM_DTEX_H_
