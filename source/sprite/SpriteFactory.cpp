#include "SpriteFactory.h"
#include "SymbolPool.h"
#include "FilepathHelper.h"
#include "SpriteIO.h"

#include "Scale9SprLoader.h"
#include "IconSprLoader.h"
#include "TextboxSprLoader.h"
#include "ComplexSprLoader.h"
#include "AnimSprLoader.h"
#include "Anim2SprLoader.h"
#include "P3dSprLoader.h"
#include "P2dSprLoader.h"
#include "MeshSprLoader.h"

#include <simp/simp_types.h>
#include <simp/NodeFactory.h>
#include <simp/NodeScale9Spr.h>
#include <simp/NodeIconSpr.h>
#include <simp/NodeTextureSpr.h>
#include <simp/NodeComplexSpr.h>
#include <simp/NodeAnimationSpr.h>
#include <simp/NodeParticle3dSpr.h>
#include <simp/NodeParticle2dSpr.h>
#include <simp/NodeShapeSpr.h>
#include <simp/NodeMeshSpr.h>
#include <simp/NodeMaskSpr.h>
#include <simp/NodeTrailSpr.h>
#include <simp/NodeAnim2Spr.h>

#include <sprite2/S2_Symbol.h>
#include <sprite2/ImageSprite.h>
#include <sprite2/Scale9Sprite.h>
#include <sprite2/IconSprite.h>
#include <sprite2/TextureSprite.h>
#include <sprite2/TextboxSprite.h>
#include <sprite2/ComplexSprite.h>
#include <sprite2/AnimSprite.h>
#include <sprite2/Anim2Sprite.h>
#include <sprite2/Particle3dSprite.h>
#include <sprite2/Particle2dSprite.h>
#include <sprite2/ShapeSprite.h>
#include <sprite2/MeshSprite.h>
#include <sprite2/MaskSprite.h>
#include <sprite2/TrailSprite.h>
#include <sprite2/SkeletonSprite.h>
#include <sprite2/SymType.h>

#include <assert.h>

namespace gum
{

SINGLETON_DEFINITION(SpriteFactory);

SpriteFactory::SpriteFactory()
{
}

s2::Sprite* SpriteFactory::Create(s2::Symbol* sym, uint32_t id) const
{
	s2::Sprite* spr = NULL;
	switch (sym->Type())
	{
	case s2::SYM_IMAGE:
		spr = new s2::ImageSprite(sym, id);
		break;
	case s2::SYM_SCALE9:
		spr = new s2::Scale9Sprite(sym, id);
		break;
	case s2::SYM_ICON:
		spr = new s2::IconSprite(sym, id);
		break;
	case s2::SYM_TEXTURE:
		spr = new s2::TextureSprite(sym, id);
		break;
	case s2::SYM_TEXTBOX:
		spr = new s2::TextboxSprite(sym, id);
		break;
	case s2::SYM_COMPLEX:
		spr = new s2::ComplexSprite(sym, id);
		break;
	case s2::SYM_ANIMATION:
		spr = new s2::AnimSprite(sym, id);
		break;
	case s2::SYM_ANIM2:
		spr = new s2::Anim2Sprite(sym, id);
		break;
	case s2::SYM_PARTICLE3D:
		spr = new s2::Particle3dSprite(sym, id);
		break;
	case s2::SYM_PARTICLE2D:
		spr = new s2::Particle2dSprite(sym, id);
		break;
	case s2::SYM_SHAPE:
		spr = new s2::ShapeSprite(sym, id);
		break;
	case s2::SYM_MESH:
		spr = new s2::MeshSprite(sym, id);
		break;
	case s2::SYM_MASK:
		spr = new s2::MaskSprite(sym, id);
		break;
	case s2::SYM_TRAIL:
		spr = new s2::TrailSprite(sym, id);
		break;
	case s2::SYM_SKELETON:
		spr = new s2::SkeletonSprite(sym, id);
		break;
	default:
		assert(0);
	}
	return spr;
}

s2::Sprite* SpriteFactory::Create(const std::string& filepath) const
{
	s2::Symbol* sym = SymbolPool::Instance()->Fetch(filepath);
	if (!sym) {
		return NULL;
	} else {
		s2::Sprite* spr = Create(sym);
		sym->RemoveReference();
		return spr;
	}
}

s2::Sprite* SpriteFactory::Create(const Json::Value& val, const std::string& dir) const
{
	s2::Sprite* spr = NULL;

	std::string filepath = val["filepath"].asString();
	filepath = FilepathHelper::Absolute(dir, filepath);
	spr = Create(filepath);
	if (!spr) {
		return NULL;
	}

	SpriteIO io(true, true);
	io.Load(val, spr);

	switch (spr->GetSymbol()->Type())
	{
	case s2::SYM_IMAGE:
		{
		}
		break;
	case s2::SYM_SCALE9:
		{
			Scale9SprLoader loader(VI_DOWNCASTING<s2::Scale9Sprite*>(spr));
			loader.LoadJson(val, dir);
		}
		break;
	case s2::SYM_ICON:
		{
			IconSprLoader loader(VI_DOWNCASTING<s2::IconSprite*>(spr));
			loader.LoadJson(val, dir);
		}
		break;
	case s2::SYM_TEXTURE:
		{
		}
		break;
	case s2::SYM_TEXTBOX:
		{
			s2::TextboxSprite* text = VI_DOWNCASTING<s2::TextboxSprite*>(spr);
			TextboxSprLoader loader(text);
			loader.LoadJson(val);
		}
		break;
	case s2::SYM_COMPLEX:
		{
			ComplexSprLoader loader(VI_DOWNCASTING<s2::ComplexSprite*>(spr));
			loader.LoadJson(val, dir);
		}
		break;
	case s2::SYM_ANIMATION:
		{
			AnimSprLoader loader(VI_DOWNCASTING<s2::AnimSprite*>(spr));
			loader.LoadJson(val, dir);
		}
		break;
	case s2::SYM_PARTICLE3D:
		{
			P3dSprLoader loader(VI_DOWNCASTING<s2::Particle3dSprite*>(spr));
			loader.LoadJson(val, dir);
		}
		break;
	case s2::SYM_PARTICLE2D:
		{
			P2dSprLoader loader(VI_DOWNCASTING<s2::Particle2dSprite*>(spr));
			loader.LoadJson(val, dir);
		}
		break;
	case s2::SYM_MESH:
		{
			MeshSprLoader loader(VI_DOWNCASTING<s2::MeshSprite*>(spr));
			loader.LoadJson(val, dir);
		}
		break;
	case s2::SYM_MASK:
		{
		}
		break;
	case s2::SYM_TRAIL:
		{
		}
		break;
	case s2::SYM_SKELETON:
		{
		}
		break;
	default:
		assert(0);
	}

	return spr;
}

s2::Sprite* SpriteFactory::Create(uint32_t id)
{
	s2::Sprite* spr = NULL;
	int type;
	const void* data = simp::NodeFactory::Instance()->Create(id, &type);
	assert(data);
	switch (type)
	{
	case simp::TYPE_IMAGE:
		{
			s2::Symbol* sym = SymbolPool::Instance()->Fetch(id);
			spr = new s2::ImageSprite(sym, id);
			sym->RemoveReference();
		}
		break;
	case simp::TYPE_SCALE9_SPR:
		{
			const simp::NodeScale9Spr* node = (const simp::NodeScale9Spr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::Scale9Sprite* s9_spr = new s2::Scale9Sprite(sym, id);
			sym->RemoveReference();

			Scale9SprLoader loader(s9_spr);
			loader.LoadBin(node);

			spr = s9_spr;
		}
		break;
	case simp::TYPE_ICON_SPR:
		{
			const simp::NodeIconSpr* node = (const simp::NodeIconSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::IconSprite* icon_spr = new s2::IconSprite(sym, id);
			sym->RemoveReference();

			IconSprLoader loader(icon_spr);
			loader.LoadBin(node);

			spr = icon_spr;
		}
		break;
	case simp::TYPE_TEXTURE_SPR:
		{
			const simp::NodeTextureSpr* node = (const simp::NodeTextureSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::TextureSprite* tex_spr = new s2::TextureSprite(sym, id);
			sym->RemoveReference();

			spr = tex_spr;
		}
		break;
	case simp::TYPE_LABEL:
		{
			const simp::NodeLabel* node = (const simp::NodeLabel*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(id);
			s2::TextboxSprite* tb_spr = VI_DOWNCASTING<s2::TextboxSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			TextboxSprLoader loader(tb_spr);
			loader.LoadBin(node);

			spr = tb_spr;
		}
		break;
	case simp::TYPE_COMPLEX_SPR:
		{
			const simp::NodeComplexSpr* node = (const simp::NodeComplexSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::ComplexSprite* comp_spr = VI_DOWNCASTING<s2::ComplexSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			ComplexSprLoader loader(comp_spr);
			loader.LoadBin(node);

			spr = comp_spr;
		}
		break;
	case simp::TYPE_ANIM_SPR:
		{
			const simp::NodeAnimationSpr* node = (const simp::NodeAnimationSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::AnimSprite* anim_spr = VI_DOWNCASTING<s2::AnimSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			AnimSprLoader loader(anim_spr);
			loader.LoadBin(node);

			spr = anim_spr;
		}
		break;
	case simp::TYPE_ANIM2_SPR:
		{
			const simp::NodeAnim2Spr* node = (const simp::NodeAnim2Spr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::Anim2Sprite* anim2_spr = VI_DOWNCASTING<s2::Anim2Sprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			Anim2SprLoader loader(anim2_spr);
			loader.LoadBin(node);
			
			spr = anim2_spr;
		}
		break;
	case simp::TYPE_P3D_SPR:
		{
			const simp::NodeParticle3dSpr* node = (const simp::NodeParticle3dSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::Particle3dSprite* p3d_spr = VI_DOWNCASTING<s2::Particle3dSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			P3dSprLoader loader(p3d_spr);
			loader.LoadBin(node);

			spr = p3d_spr;
		}
		break;
	case simp::TYPE_P2D_SPR:
		{
			const simp::NodeParticle2dSpr* node = (const simp::NodeParticle2dSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::Particle2dSprite* p2d_spr = VI_DOWNCASTING<s2::Particle2dSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			P2dSprLoader loader(p2d_spr);
			loader.LoadBin(node);

			spr = p2d_spr;
		}
		break;
	case simp::TYPE_SHAPE_SPR:
		{
			const simp::NodeShapeSpr* node = (const simp::NodeShapeSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::ShapeSprite* shape_spr = VI_DOWNCASTING<s2::ShapeSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			spr = shape_spr;
		}
		break;
	case simp::TYPE_MESH_SPR:
		{
			const simp::NodeMeshSpr* node = (const simp::NodeMeshSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->mesh_id);
			s2::MeshSprite* mesh_spr = VI_DOWNCASTING<s2::MeshSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			MeshSprLoader loader(mesh_spr);
			loader.LoadBin(node);

			spr = mesh_spr;
		}
		break;
	case simp::TYPE_MASK_SPR:
		{
			const simp::NodeMaskSpr* node = (const simp::NodeMaskSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::MaskSprite* mask_spr = VI_DOWNCASTING<s2::MaskSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			spr = mask_spr;
		}
		break;
	case simp::TYPE_TRAIL_SPR:
		{
			const simp::NodeTrailSpr* node = (const simp::NodeTrailSpr*)data;

			s2::Symbol* sym = SymbolPool::Instance()->Fetch(node->sym);
			s2::TrailSprite* trail_spr = VI_DOWNCASTING<s2::TrailSprite*>(SpriteFactory::Instance()->Create(sym, id));
			sym->RemoveReference();

			spr = trail_spr;
		}
		break;
	case simp::TYPE_SCALE9: case simp::TYPE_ICON: case simp::TYPE_TEXTURE: case simp::TYPE_COMPLEX: case simp::TYPE_ANIMATION:
	case simp::TYPE_PARTICLE3D: case simp::TYPE_PARTICLE2D: case simp::TYPE_SHAPE: case simp::TYPE_MESH: case simp::TYPE_MASK:
	case simp::TYPE_TRAIL:
		spr = CreateFromSym(id);
		break;
	default:
		assert(0);
	}

	return spr;
}

s2::Sprite* SpriteFactory::CreateFromSym(uint32_t id)
{
	s2::Symbol* sym = SymbolPool::Instance()->Fetch(id);
	if (!sym) {
		return NULL;
	} else {
		s2::Sprite* spr = Create(sym);
		sym->RemoveReference();
		return spr;
	}
}

}