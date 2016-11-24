#include "SpriteFactory.h"

#include <simp/Package.h>
#include <simp/NodeFactory.h>
#include <sprite2/DrawNode.h>
#include <sprite2/S2_Sprite.h>
#include <sprite2/SprTimer.h>

namespace gum
{

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
void gum_draw_spr(const void* spr, float x, float y, float angle, float sx, float sy)
{
	s2::RenderParams params;
	params.mt.SetTransformation(x, y, angle, sx, sy, 0, 0, 0, 0);
	s2::DrawNode::Draw(static_cast<const s2::Sprite*>(spr), params);
}

extern "C"
void gum_update_spr(void* spr) {
	static_cast<s2::Sprite*>(spr)->Update(s2::RenderParams());
}

extern "C"
void* gum_fetch_child(const void* spr, const char* name) {
	return static_cast<const s2::Sprite*>(spr)->FetchChild(name);
}

}