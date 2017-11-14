#include "gum/SymbolFactory.h"
#include "gum/SymbolFile.h"
#include "gum/ImagePool.h"

#include "gum/ImageSymbol.h"
#include "gum/ImageSymLoader.h"
#include "gum/ProxyImage.h"
#include "gum/Scale9SymLoader.h"
#include "gum/IconSymLoader.h"
#include "gum/TextureSymLoader.h"
#include "gum/TextboxSymbol.h"
#include "gum/TextboxLoader.h"
#include "gum/ComplexSymLoader.h"
#include "gum/AnimSymLoader.h"
#include "gum/Anim2SymLoader.h"
#include "gum/P3dSymLoader.h"
#include "gum/P2dSymLoader.h"
#include "gum/ShapeSymLoader.h"
#include "gum/MeshSymLoader.h"
#include "gum/MaskSymLoader.h"
#include "gum/TrailSymLoader.h"
#include "gum/SkeletonSymLoader.h"

#include <logger.h>
#include <simp/NodeFactory.h>
#include <simp/simp_types.h>
#include <simp/NodePicture.h>
#include <timp/PkgMgr.h>
#include <timp/Package.h>

#include <sprite2/AnchorSymbol.h>
#include <sprite2/Scale9Symbol.h>
#include <sprite2/IconSymbol.h>
#include <sprite2/TextureSymbol.h>
#include <sprite2/ComplexSymbol.h>
#include <sprite2/AnimSymbol.h>
#include <sprite2/Anim2Symbol.h>
#include <sprite2/Particle3dSymbol.h>
#include <sprite2/Particle2dSymbol.h>
#include <sprite2/ShapeSymbol.h>
#include <sprite2/MeshSymbol.h>
#include <sprite2/MaskSymbol.h>
#include <sprite2/TrailSymbol.h>
#include <sprite2/SkeletonSymbol.h>
#include <sprite2/AudioSymbol.h>
#include <sprite2/SymType.h>
#include <sprite2/CacheMatVisitor.h>

#include <fstream>

#include <assert.h>

namespace gum
{

CU_SINGLETON_DEFINITION(SymbolFactory);

SymbolFactory::SymbolFactory()
{
}

s2::SymPtr SymbolFactory::Create(const CU_STR& filepath, int type) const
{
	s2::SymPtr ret = nullptr;

	if (type == s2::SYM_UNKNOWN) {
		type = SymbolFile::Instance()->Type(filepath);
	}
	switch (type)
	{
	case s2::SYM_IMAGE:
		{
			auto sym = mm::allocate_shared<ImageSymbol>();
			ImageSymLoader loader(*sym);
			loader.Load(bimp::FilePath(filepath), 1, false);
			ret = sym;
		}
		break;
	case s2::SYM_SCALE9:
		{
			auto sym = mm::allocate_shared<s2::Scale9Symbol>();
			Scale9SymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ICON:
		{
			auto sym = mm::allocate_shared<s2::IconSymbol>();
			IconSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TEXTURE:
		{
			auto sym = mm::allocate_shared<s2::TextureSymbol>();
			TextureSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TEXTBOX:
		{
			auto sym = mm::allocate_shared<TextboxSymbol>();
			TextboxLoader loader(sym->GetTextbox());
			Json::Value val;
			Json::Reader reader;
			std::locale::global(std::locale(""));
			std::ifstream fin(filepath.c_str());
			std::locale::global(std::locale("C"));
			reader.parse(fin, val);
			fin.close();
			loader.LoadJson(val);
			ret = sym;
		}
		break;
	case s2::SYM_COMPLEX:
		{
			auto sym = mm::allocate_shared<s2::ComplexSymbol>();
			ComplexSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ANIMATION:
		{
			auto sym = mm::allocate_shared<s2::AnimSymbol>();
			AnimSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ANIM2:
		{
			auto sym = mm::allocate_shared<s2::Anim2Symbol>();
			Anim2SymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_PARTICLE3D:
		{
			auto sym = mm::allocate_shared<s2::Particle3dSymbol>();
			P3dSymLoader loader;
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_PARTICLE2D:
		{
			auto sym = mm::allocate_shared<s2::Particle2dSymbol>();
			P2dSymLoader loader;
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_SHAPE:
		{
			auto sym = mm::allocate_shared<s2::ShapeSymbol>();
			ShapeSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_MESH:
		{
			auto sym = mm::allocate_shared<s2::MeshSymbol>();
			MeshSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_MASK:
		{
			auto sym = mm::allocate_shared<s2::MaskSymbol>();
			MaskSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TRAIL:
		{
			auto sym = mm::allocate_shared<s2::TrailSymbol>();
			TrailSymLoader loader;
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_SKELETON:
		{
			auto sym = mm::allocate_shared<s2::SkeletonSymbol>();
			SkeletonSymLoader loader(*sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_AUDIO:
		{
			auto sym = mm::allocate_shared<s2::AudioSymbol>();
			ret = sym;
		}
		break;
	default:
		LOGW("Error Sym Type: filepath %s, type %d", filepath.c_str(), type);
	}

	if (!ret) {
		LOGW("Create sym fail: filepath %s", filepath.c_str());
	}

	return ret;
}

s2::SymPtr SymbolFactory::Create(uint32_t id) const
{
	if (id == 0xffffffff) {
		if (!m_anchor_sym) {
			m_anchor_sym = mm::allocate_shared<s2::AnchorSymbol>();
		}
		return m_anchor_sym;
	}

	s2::SymPtr ret = nullptr;

	int type;
	const void* data = simp::NodeFactory::Instance()->Create(id, &type);
	if(!data) {
		LOGW("Create sym fail: id %u", id);
		return nullptr;
	}
	switch (type)
	{
	case simp::TYPE_IMAGE:
		{
			const simp::NodePicture* pic = (const simp::NodePicture*)data;
			sm::vec2 offset(pic->offx * 0.5f, pic->offy * 0.5f);
			std::shared_ptr<ImageSymbol> sym = nullptr;
			if (pic->texid < simp::NodePicture::MAX_IN_PKG)
			{
				int pkg_id = simp::NodeID::GetPkgID(id);
				const timp::Package* t_pkg = timp::PkgMgr::Instance()->Query(pkg_id);

				const bimp::FilePath& fp = t_pkg->GetTexPath(pic->texid, 0);

				sym = mm::allocate_shared<ImageSymbol>(id);
				ImageSymLoader loader(*sym);

				bool async = true;
				const simp::Package* s_pkg = simp::NodeFactory::Instance()->QueryPkg(pkg_id);
				loader.Load(fp, s_pkg->GetScale(), async);
				const timp::Package::TextureDesc& tex = t_pkg->GetTexDesc(pic->texid);
				auto& img = sym->GetImage();
				if (img && async) {
					img->AsyncLoad(pkg_id, tex.type, tex.w, tex.h);
				}

				sym->SetRegion(
					sm::ivec2(pic->region[0], pic->region[1]), 
					sm::ivec2(pic->region[2], pic->region[3]),
					offset,
					pic->lod,
					s_pkg->GetScale());
			}
			else
			{
				CU_STR filepath = ProxyImage::GetFilepath(pic->texid - simp::NodePicture::MAX_IN_PKG);
				auto img = ImagePool::Instance()->Query(bimp::FilePath(filepath));
				assert(img);

				int pkg_id = simp::NodeID::GetPkgID(id);
				const simp::Package* s_pkg = simp::NodeFactory::Instance()->QueryPkg(pkg_id);

				sym = mm::allocate_shared<ImageSymbol>(id);
				sym->SetImage(img);
				sym->SetRegion(
					sm::ivec2(pic->region[0], pic->region[1]), 
					sm::ivec2(pic->region[2], pic->region[3]),
					offset,
					pic->lod,
					s_pkg->GetScale());
			}
			ret = sym;
		}
		break;
	case simp::TYPE_SCALE9:
		{
			auto sym = mm::allocate_shared<s2::Scale9Symbol>(id);
			Scale9SymLoader loader(*sym);
			loader.LoadBin((const simp::NodeScale9*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ICON:
		{
			auto sym = mm::allocate_shared<s2::IconSymbol>(id);
			IconSymLoader loader(*sym);
			loader.LoadBin((const simp::NodeIcon*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_TEXTURE:
		{
			auto sym = mm::allocate_shared<s2::TextureSymbol>(id);
			TextureSymLoader loader(*sym);
			loader.LoadBin((const simp::NodeTexture*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_LABEL:
		{
			auto sym = mm::allocate_shared<TextboxSymbol>(id);
			TextboxLoader loader(sym->GetTextbox());
			loader.LoadBin((const simp::NodeLabel*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_COMPLEX:
		{
			auto sym = mm::allocate_shared<s2::ComplexSymbol>(id);
			ComplexSymLoader loader(*sym);
			loader.LoadBin((const simp::NodeComplex*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ANIMATION:
		{
			auto sym = mm::allocate_shared<s2::AnimSymbol>(id);
			AnimSymLoader loader(*sym, nullptr, nullptr);
			loader.LoadBin((const simp::NodeAnimation*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ANIM2:
		{
			auto sym = mm::allocate_shared<s2::Anim2Symbol>(id);
			Anim2SymLoader loader(*sym);
			loader.LoadBin((const simp::NodeAnim2*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_PARTICLE3D:
		{
			auto sym = mm::allocate_shared<s2::Particle3dSymbol>(id);
			P3dSymLoader loader;
			loader.LoadBin((const simp::NodeParticle3d*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case simp::TYPE_PARTICLE2D:
		{
			auto sym = mm::allocate_shared<s2::Particle2dSymbol>(id);
			P2dSymLoader loader;
			loader.LoadBin((const simp::NodeParticle2d*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case simp::TYPE_SHAPE:
		{
			auto sym = mm::allocate_shared<s2::ShapeSymbol>(id);
			ShapeSymLoader loader(*sym);
			loader.LoadBin((const simp::NodeShape*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_MESH:
		{
			auto sym = mm::allocate_shared<s2::MeshSymbol>(id);
			MeshSymLoader loader(*sym);
			loader.LoadBin((const simp::NodeMesh*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_MASK:
		{
			auto sym = mm::allocate_shared<s2::MaskSymbol>(id);
			MaskSymLoader loader(*sym);
			loader.LoadBin((const simp::NodeMask*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_TRAIL:
		{
			auto sym = mm::allocate_shared<s2::TrailSymbol>(id);
			TrailSymLoader loader;
			loader.LoadBin((const simp::NodeTrail*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	default:
		LOGW("Create sym fail: id %u, type %d", id, type);
	}

#ifdef S2_SPR_CACHE_LOCAL_MAT_SHARE
 	if (ret && type != simp::TYPE_SCALE9 && type != simp::TYPE_ANIMATION) {
 		ret->Traverse(s2::CacheMatVisitor());
 	}
#endif // S2_SPR_CACHE_LOCAL_MAT_SHARE

	if (!ret) {
		LOGW("Create sym fail: id %u", id);
	}

	return ret;
}

s2::SymPtr SymbolFactory::Create(const CU_STR& pkg_name, const CU_STR& node_name) const
{
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(pkg_name, node_name);
	if (id != 0xffffffff) {
		return Create(id);
	} else {
		LOGW("Create sym fail: pkg %s, node %s", 
			pkg_name.c_str(), node_name.c_str());
		return nullptr;
	}
}

}