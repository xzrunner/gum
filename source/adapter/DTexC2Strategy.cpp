#include "DTexC2Strategy.h"
#include "GUM_DTex.h"

#include <simp/NodeID.h>
#include <simp/NodeFactory.h>

namespace gum
{

SINGLETON_DEFINITION(DTexC2Strategy);

DTexC2Strategy::DTexC2Strategy()
{
	m_max_no_update_count = 128;
	m_no_update_count     = 0;
	m_discount            = 1;

	m_single_max_count	= 512;
	m_diff_spr_count	= 256;
	m_tot_count			= 10240;

	m_max_c2_edge = -1;
}

bool DTexC2Strategy::OnC2QueryFail(uint32_t id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region)
{
	if (id == 0xffffffff) {
		return false;
	}
	if (m_max_c2_edge > 0 && (region.Width() > m_max_c2_edge || region.Height() > m_max_c2_edge)) {
		return false;
	}

	Package* pkg = NULL;
	int pkg_id = simp::NodeID::GetPkgID(id);
	int node_id = simp::NodeID::GetNodeID(id);
	std::map<int, Package*>::iterator itr = m_pkgs.find(pkg_id);
	if (itr != m_pkgs.end()) {
		pkg = itr->second;
	} else {
		const simp::Package* p = simp::NodeFactory::Instance()->QueryPkgByID(pkg_id);
		if (!p) {
			return false;
		}
		pkg = new Package(pkg_id, p->GetMaxNodeID() + 1);
		m_pkgs.insert(std::make_pair(pkg_id, pkg));
	}
	pkg->AddCount(node_id, tex_id, tex_w, tex_h, region);

	bool loaded = false;
	if (pkg->GetSingleMaxCount() > m_single_max_count) {
		LoadPackage(pkg);
		loaded = true;
	} else if (pkg->GetDiffSprCount() > m_diff_spr_count) {
		LoadPackage(pkg);
		loaded = true;
	} else if (pkg->GetTotCount() > m_tot_count) {
		LoadPackage(pkg);
		loaded = true;
	}
	return loaded;
}

void DTexC2Strategy::Update()
{
	++m_no_update_count;
	if (m_no_update_count <= m_max_no_update_count) {
		m_discount = (float)(m_max_no_update_count - m_no_update_count) / m_max_no_update_count;
	}
}

void DTexC2Strategy::Clear()
{
	std::map<int, Package*>::iterator itr = m_pkgs.begin();
	for ( ; itr != m_pkgs.end(); ++itr) {
		delete itr->second;
	}
	m_pkgs.clear();
}

void DTexC2Strategy::LoadPackage(Package* pkg)
{
	pkg->Load();
	pkg->Clear();

	m_no_update_count = 0;
	m_discount = 1;
}

/************************************************************************/
/* class DTexC2Strategy::Package                                        */
/************************************************************************/

DTexC2Strategy::Package::
Package(int pkg_id, int sym_count)
	: m_pkg_id(pkg_id)
	, m_single_max_count(0)
	, m_diff_spr_count(0)
	, m_tot_count(0)
{
	m_sym_count = sym_count;
	m_symbols = new Symbol[m_sym_count];
}

DTexC2Strategy::Package::
~Package()
{
	delete[] m_symbols;
}

void DTexC2Strategy::Package::
AddCount(int id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region)
{
	++m_tot_count;
	if (m_symbols[id].count == 0) {
		++m_diff_spr_count;
		m_symbols[id].tex_id = tex_id;
		m_symbols[id].tex_w = tex_w;
		m_symbols[id].tex_h = tex_h;
		m_symbols[id].region = region;
	}
	++m_symbols[id].count;
	if (m_symbols[id].count > m_single_max_count) {
		m_single_max_count = m_symbols[id].count;
	}
}

void DTexC2Strategy::Package::
Load()
{
	DTex* dtex = DTex::Instance();
	dtex->LoadSymStart();
	for (int i = 0; i < m_sym_count; ++i) 
	{
		if (m_symbols[i].count == 0) {
			continue;
		}
		uint32_t id = simp::NodeID::ComposeID(m_pkg_id, i);
		UID uid = ResourceUID::BinNode(id);
		dtex->LoadSymbol(uid, m_symbols[i].tex_id, m_symbols[i].tex_w, m_symbols[i].tex_h, m_symbols[i].region, 1, 0, 1);
	}
	dtex->LoadSymFinish();
}

void DTexC2Strategy::Package::
Clear()
{
	m_single_max_count = m_diff_spr_count = m_tot_count = 0;
	for (int i = 0; i < m_sym_count; ++i) {
		m_symbols[i].count = 0;
	}
}

}