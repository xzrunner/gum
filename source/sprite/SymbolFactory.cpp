#include "SymbolFactory.h"
#include "SymbolFile.h"

#include "GUM_ImageSymbol.h"
#include "ImageSymLoader.h"
#include "ProxyImage.h"
#include "Scale9SymLoader.h"
#include "IconSymLoader.h"
#include "TextureSymLoader.h"
#include "GUM_TextboxSymbol.h"
#include "TextboxLoader.h"
#include "ComplexSymLoader.h"
#include "AnimSymLoader.h"
#include "Anim2SymLoader.h"
#include "P3dSymLoader.h"
#include "P2dSymLoader.h"
#include "ShapeSymLoader.h"
#include "MeshSymLoader.h"
#include "MaskSymLoader.h"
#include "TrailSymLoader.h"
#include "SkeletonSymLoader.h"

#include <logger.h>
#include <simp/NodeFactory.h>
#include <simp/simp_types.h>
#include <simp/NodePicture.h>
#include <timp/PkgMgr.h>
#include <timp/TIMP_Package.h>

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
#include <sprite2/SymType.h>
#include <sprite2/CacheMatVisitor.h>

#include <fstream>

#include <assert.h>

namespace gum
{

SINGLETON_DEFINITION(SymbolFactory);

SymbolFactory::SymbolFactory()
	: m_anchor_sym(NULL)
{
}

s2::Symbol* SymbolFactory::Create(const std::string& filepath, bool flatten, int type) const
{
	s2::Symbol* ret = NULL;

	if (type == s2::SYM_UNKNOWN) {
		type = SymbolFile::Instance()->Type(filepath);
	}
	switch (type)
	{
	case s2::SYM_IMAGE:
		{
			ImageSymbol* sym = new ImageSymbol;
			ImageSymLoader loader(sym);
			loader.Load(ResPath(filepath), 1, false);
			ret = sym;
		}
		break;
	case s2::SYM_SCALE9:
		{
			s2::Scale9Symbol* sym = new s2::Scale9Symbol();
			Scale9SymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ICON:
		{
			s2::IconSymbol* sym = new s2::IconSymbol();
			IconSymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TEXTURE:
		{
			s2::TextureSymbol* sym = new s2::TextureSymbol;
			TextureSymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TEXTBOX:
		{
			TextboxSymbol* sym = new TextboxSymbol();
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
			s2::ComplexSymbol* sym = new s2::ComplexSymbol();
			ComplexSymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ANIMATION:
		{
			s2::AnimSymbol* sym = new s2::AnimSymbol();
			AnimSymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ANIM2:
		{
			s2::Anim2Symbol* sym = new s2::Anim2Symbol;
			Anim2SymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_PARTICLE3D:
		{
			s2::Particle3dSymbol* sym = new s2::Particle3dSymbol;
			P3dSymLoader loader(flatten);
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_PARTICLE2D:
		{
			s2::Particle2dSymbol* sym = new s2::Particle2dSymbol;
			P2dSymLoader loader(flatten);
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_SHAPE:
		{
			s2::ShapeSymbol* sym = new s2::ShapeSymbol;
			ShapeSymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_MESH:
		{
			s2::MeshSymbol* sym = new s2::MeshSymbol;
			MeshSymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_MASK:
		{
			s2::MaskSymbol* sym = new s2::MaskSymbol;
			MaskSymLoader loader(sym, flatten);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TRAIL:
		{
			s2::TrailSymbol* sym = new s2::TrailSymbol;
			TrailSymLoader loader(flatten);
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_SKELETON:
		{
			s2::SkeletonSymbol* sym = new s2::SkeletonSymbol;
			SkeletonSymLoader loader(sym);
			loader.LoadJson(filepath);
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

s2::Symbol* SymbolFactory::Create(uint32_t id, bool flatten) const
{
	if (id == 0xffffffff) {
		if (!m_anchor_sym) {
			m_anchor_sym = new s2::AnchorSymbol();
		}
		return m_anchor_sym;
	}

	s2::Symbol* ret = NULL;

	int type;
	const void* data = simp::NodeFactory::Instance()->Create(id, &type);
	if(!data) {
		LOGW("Create sym fail: id %u", id);
		return NULL;
	}
	switch (type)
	{
	case simp::TYPE_IMAGE:
		{
			const simp::NodePicture* pic = (const simp::NodePicture*)data;
			sm::vec2 offset(pic->offx * 0.5f, pic->offy * 0.5f);
			ImageSymbol* sym = NULL;
			if (pic->texid < simp::NodePicture::MAX_IN_PKG)
			{
				int pkg_id = simp::NodeID::GetPkgID(id);
				const timp::Package* t_pkg = timp::PkgMgr::Instance()->Query(pkg_id);

				const bimp::FilePath& fp = t_pkg->GetTexPath(pic->texid, 0);

				sym = new ImageSymbol(id);
				ImageSymLoader loader(sym);

				bool async = true;
				const simp::Package* s_pkg = simp::NodeFactory::Instance()->QueryPkg(pkg_id);
				loader.Load(ResPath(fp.GetFilepath(), fp.GetOffset()), s_pkg->GetScale(), async);
				const timp::Package::TextureDesc& tex = t_pkg->GetTexDesc(pic->texid);
				Image* img = const_cast<Image*>(sym->GetImage());
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
				std::string filepath = ProxyImage::GetFilepath(pic->texid - simp::NodePicture::MAX_IN_PKG);
				Image* img = ImageMgr::Instance()->Query(ResPath(filepath));
				assert(img);

				int pkg_id = simp::NodeID::GetPkgID(id);
				const simp::Package* s_pkg = simp::NodeFactory::Instance()->QueryPkg(pkg_id);

				sym = new ImageSymbol(id);
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
			s2::Scale9Symbol* sym = new s2::Scale9Symbol(id);
			Scale9SymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeScale9*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ICON:
		{
			s2::IconSymbol* sym = new s2::IconSymbol(id);
			IconSymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeIcon*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_TEXTURE:
		{
			s2::TextureSymbol* sym = new s2::TextureSymbol(id);
			TextureSymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeTexture*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_LABEL:
		{
			TextboxSymbol* sym = new TextboxSymbol(id);
			TextboxLoader loader(sym->GetTextbox());
			loader.LoadBin((const simp::NodeLabel*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_COMPLEX:
		{
			s2::ComplexSymbol* sym = new s2::ComplexSymbol(id);
			ComplexSymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeComplex*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ANIMATION:
		{
			s2::AnimSymbol* sym = new s2::AnimSymbol(id);
			AnimSymLoader loader(sym, flatten, NULL, NULL);
			loader.LoadBin((const simp::NodeAnimation*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ANIM2:
		{
			s2::Anim2Symbol* sym = new s2::Anim2Symbol(id);
			Anim2SymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeAnim2*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_PARTICLE3D:
		{
			s2::Particle3dSymbol* sym = new s2::Particle3dSymbol(id);
			P3dSymLoader loader(flatten);
			loader.LoadBin((const simp::NodeParticle3d*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case simp::TYPE_PARTICLE2D:
		{
			s2::Particle2dSymbol* sym = new s2::Particle2dSymbol(id);
			P2dSymLoader loader(flatten);
			loader.LoadBin((const simp::NodeParticle2d*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case simp::TYPE_SHAPE:
		{
			s2::ShapeSymbol* sym = new s2::ShapeSymbol(id);
			ShapeSymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeShape*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_MESH:
		{
			s2::MeshSymbol* sym = new s2::MeshSymbol(id);
			MeshSymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeMesh*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_MASK:
		{
			s2::MaskSymbol* sym = new s2::MaskSymbol(id);
			MaskSymLoader loader(sym, flatten);
			loader.LoadBin((const simp::NodeMask*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_TRAIL:
		{
			s2::TrailSymbol* sym = new s2::TrailSymbol(id);
			TrailSymLoader loader(flatten);
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

s2::Symbol* SymbolFactory::Create(const std::string& pkg_name, const std::string& node_name, bool flatten) const
{
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(pkg_name, node_name);
	if (id != 0xffffffff) {
		return Create(id, flatten);
	} else {
		LOGW("Create sym fail: pkg %s, node %s", 
			pkg_name.c_str(), node_name.c_str());
		return NULL;
	}
}

}