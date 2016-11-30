#include "SpriteFactory.h"
#include "SymbolPool.h"

#include <simp/Package.h>
#include <simp/NodeFactory.h>
#include <sprite2/DrawNode.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/SprTimer.h>

namespace gum
{

extern "C"
void gum_gc()
{
	SymbolPool* sym = SymbolPool::Instance();
	int sym_before = sym->Count();
	int spr_before = s2::Sprite::GetCount();
	sym->GC();
	int sym_after = sym->Count();
	int spr_after = s2::Sprite::GetCount();
	printf("[GUM] gc sym before %d, after %d\n", sym_before, sym_after);
	printf("[GUM] gc spr before %d, after %d\n", spr_before, spr_after);
}

extern "C"
void gum_update(float dt)
{
	s2::SprTimer::Instance()->Update(dt);
}

extern "C"
void gum_create_pkg(const char* filepath, const char* name, int id)
{
	simp::Package* pkg = new simp::Package(filepath);
	simp::NodeFactory::Instance()->AddPkg(pkg, name, id);
}

extern "C"
void* gum_create_spr(const char* pkg, const char* spr)
{
	uint32_t id = simp::NodeFactory::Instance()->GetID(pkg, spr);
	return SpriteFactory::Instance()->CreateFromSym(id);
}

extern "C"
void* gum_create_spr_by_id(int id)
{
	return SpriteFactory::Instance()->CreateFromSym(id);
}

extern "C"
void* gum_create_spr_from_file(const char* filepath)
{
	return SpriteFactory::Instance()->Create(filepath);
}

extern "C"
void gum_spr_draw(const void* spr, float x, float y, float angle, float sx, float sy)
{
	s2::RenderParams params;
	params.mt.SetTransformation(x, y, angle, sx, sy, 0, 0, 0, 0);
	s2::DrawNode::Draw(static_cast<const s2::Sprite*>(spr), params);
}

extern "C"
void gum_spr_update(void* spr) {
	static_cast<s2::Sprite*>(spr)->Update(s2::RenderParams());
}

extern "C"
void gum_spr_release(void* spr) {
	s2::Sprite* test = static_cast<s2::Sprite*>(spr);
	test->RemoveReference();
}

extern "C"
void* gum_fetch_child(const void* spr, const char* name) {
	return static_cast<const s2::Sprite*>(spr)->FetchChild(name);
}

extern "C"
void gum_spr_set_pos(void* spr, float x, float y) {
	static_cast<s2::Sprite*>(spr)->SetPosition(sm::vec2(x, y));
}

extern "C"
void gum_spr_set_angle(void* spr, float angle) {
	static_cast<s2::Sprite*>(spr)->SetAngle(angle);
}

extern "C"
void gum_spr_set_scale(void* spr, float sx, float sy) {
	static_cast<s2::Sprite*>(spr)->SetScale(sm::vec2(sx, sy));
}

}