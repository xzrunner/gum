#ifndef _GUM_DTEX_H_
#define _GUM_DTEX_H_

#include <SM_Rect.h>
#include <cu/cu_macro.h>
#include <dtex2/CacheGlyph.h>
#include <s2loader/ResourceUID.h>

#include <stdint.h>

namespace dtex { class CacheSymbol; class CacheGlyph; class Texture; struct Rect; }

namespace gum
{

class DTex
{
public:
	void InitHook(void (*draw_begin)(), void (*draw_end)(), void (*error_reload)());

	// C3
	void CreatePkg(int pkg_id);

	// C2
	void SetC2Enable(bool enable) { m_c2_enable = enable; }
	bool IsC2Enable() const { return m_c2_enable; }
	void LoadSymStart();
	void LoadSymbol(s2loader::UID sym_id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region,
		int padding = 0, int extrude = 0, int src_extrude = 0);
	void LoadSymFinish();
	const float* QuerySymbol(s2loader::UID sym_id, int& tex_id, int& block_id) const;
	void ClearSymbolCache();

	// CG
	void DrawGlyph(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key);
	void LoadGlyph(uint32_t* bitmap, int width, int height, uint64_t key);
	bool QueryGlyph(uint64_t key, float* texcoords, int& tex_id) const;
	bool ExistGlyph(uint64_t key) const { return m_cg->Exist(key); }

	void Clear();

	void Flush();

	void DebugDraw() const;

private:
	dtex::CacheSymbol* m_c2;
	bool m_c2_enable;

	dtex::CacheGlyph* m_cg;

	CU_SINGLETON_DECLARATION(DTex)

}; // DTex

}

#endif // _GUM_DTEX_H_
