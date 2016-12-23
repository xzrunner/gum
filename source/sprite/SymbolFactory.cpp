#include "SymbolFactory.h"
#include "SymbolFile.h"

#include "ImageSymbol.h"
#include "ImageSymLoader.h"
#include "ProxyImage.h"
#include "Scale9SymLoader.h"
#include "IconSymLoader.h"
#include "TextureSymLoader.h"
#include "TextboxSymbol.h"
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

#include <simp/NodeFactory.h>
#include <simp/simp_types.h>
#include <simp/NodePicture.h>
#include <timp/PkgMgr.h>
#include <timp/Package.h>

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

#include <fstream>

#include <assert.h>

namespace gum
{

SINGLETON_DEFINITION(SymbolFactory);

SymbolFactory::SymbolFactory()
{
}

s2::Symbol* SymbolFactory::Create(const std::string& filepath, int type) const
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
			loader.Load(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_SCALE9:
		{
			s2::Scale9Symbol* sym = new s2::Scale9Symbol();
			Scale9SymLoader loader(sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ICON:
		{
			s2::IconSymbol* sym = new s2::IconSymbol();
			IconSymLoader loader(sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TEXTURE:
		{
			s2::TextureSymbol* sym = new s2::TextureSymbol;
			TextureSymLoader loader(sym);
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
			ComplexSymLoader loader(sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ANIMATION:
		{
			s2::AnimSymbol* sym = new s2::AnimSymbol();
			AnimSymLoader loader(sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_ANIM2:
		{
			s2::Anim2Symbol* sym = new s2::Anim2Symbol;
			Anim2SymLoader loader(sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_PARTICLE3D:
		{
			s2::Particle3dSymbol* sym = new s2::Particle3dSymbol;
			P3dSymLoader loader;
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_PARTICLE2D:
		{
			s2::Particle2dSymbol* sym = new s2::Particle2dSymbol;
			P2dSymLoader loader;
			loader.LoadJson(filepath);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case s2::SYM_SHAPE:
		{
			s2::ShapeSymbol* sym = new s2::ShapeSymbol;
			ShapeSymLoader loader(sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_MASK:
		{
			s2::MaskSymbol* sym = new s2::MaskSymbol;
			MaskSymLoader loader(sym);
			loader.LoadJson(filepath);
			ret = sym;
		}
		break;
	case s2::SYM_TRAIL:
		{
			s2::TrailSymbol* sym = new s2::TrailSymbol;
			TrailSymLoader loader;
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
		assert(0);
	}

	return ret;
}

s2::Symbol* SymbolFactory::Create(uint32_t id) const
{
	s2::Symbol* ret = NULL;

	int type;
	const void* data = simp::NodeFactory::Instance()->Create(id, &type);
	assert(data);
	switch (type)
	{
	case simp::TYPE_IMAGE:
		{
			const simp::NodePicture* pic = (const simp::NodePicture*)data;	
			ImageSymbol* sym = NULL;
			if (pic->texid < simp::NodePicture::MAX_IN_PKG)
			{
				int pkg_id = simp::NodeID::GetPkgID(id);
				const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
				std::string filepath = pkg->GetTexPath(pic->texid, 0);

				sym = new ImageSymbol(id);
				ImageSymLoader loader(sym);
				loader.Load(filepath);
				sym->SetRegion(
					sm::ivec2(pic->region[0], pic->region[1]), 
					sm::ivec2(pic->region[2], pic->region[3]));
			}
			else
			{
				std::string filepath = ProxyImage::GetFilepath(pic->texid - simp::NodePicture::MAX_IN_PKG);
				Image* img = ImageMgr::Instance()->Query(filepath);
				assert(img);

				sym = new ImageSymbol(id);
				sym->SetImage(img);
				sym->SetRegion(
					sm::ivec2(pic->region[0], pic->region[1]), 
					sm::ivec2(pic->region[2], pic->region[3]));
			}
			ret = sym;
		}
		break;
	case simp::TYPE_SCALE9:
		{
			s2::Scale9Symbol* sym = new s2::Scale9Symbol(id);
			Scale9SymLoader loader(sym);
			loader.LoadBin((const simp::NodeScale9*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ICON:
		{
			s2::IconSymbol* sym = new s2::IconSymbol(id);
			IconSymLoader loader(sym);
			loader.LoadBin((const simp::NodeIcon*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_TEXTURE:
		{
			s2::TextureSymbol* sym = new s2::TextureSymbol(id);
			TextureSymLoader loader(sym);
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
			ComplexSymLoader loader(sym);
			loader.LoadBin((const simp::NodeComplex*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_ANIMATION:
		{
			s2::AnimSymbol* sym = new s2::AnimSymbol(id);
			AnimSymLoader loader(sym);
			loader.LoadBin((const simp::NodeAnimation*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_PARTICLE3D:
		{
			s2::Particle3dSymbol* sym = new s2::Particle3dSymbol(id);
			P3dSymLoader loader;
			loader.LoadBin((const simp::NodeParticle3d*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case simp::TYPE_PARTICLE2D:
		{
			s2::Particle2dSymbol* sym = new s2::Particle2dSymbol(id);
			P2dSymLoader loader;
			loader.LoadBin((const simp::NodeParticle2d*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	case simp::TYPE_SHAPE:
		{
			s2::ShapeSymbol* sym = new s2::ShapeSymbol(id);
			ShapeSymLoader loader(sym);
			loader.LoadBin((const simp::NodeShape*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_MESH:
		{
			s2::MeshSymbol* sym = new s2::MeshSymbol(id);
			MeshSymLoader loader(sym);
			loader.LoadBin((const simp::NodeMesh*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_MASK:
		{
			s2::MaskSymbol* sym = new s2::MaskSymbol(id);
			MaskSymLoader loader(sym);
			loader.LoadBin((const simp::NodeMask*)data);
			ret = sym;
		}
		break;
	case simp::TYPE_TRAIL:
		{
			s2::TrailSymbol* sym = new s2::TrailSymbol(id);
			TrailSymLoader loader;
			loader.LoadBin((const simp::NodeTrail*)data);
			loader.Store(sym);
			ret = sym;
		}
		break;
	default:
		assert(0);
	}

	return ret;
}

s2::Symbol* SymbolFactory::Create(const std::string& pkg_name, const std::string& node_name) const
{
	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(pkg_name, node_name);
	if (id != 0xffffffff) {
		return Create(id);
	} else {
		return NULL;
	}
}

}