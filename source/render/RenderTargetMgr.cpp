#include "RenderTargetMgr.h"
#include "RenderTarget.h"

#include <dtex2/DebugDraw.h>

#include <assert.h>

namespace gum
{

SINGLETON_DEFINITION(RenderTargetMgr);

RenderTargetMgr::RenderTargetMgr()
{
}

RenderTarget* RenderTargetMgr::Fetch()
{
	for (int i = 0; i < MAX_COUNT; ++i) {
		if (m_items[i].available) {
			m_items[i].available = false;
			return m_items[i].rt;
		}
	}
	return NULL;
}

void RenderTargetMgr::Return(RenderTarget* rt)
{
	if (!rt) {
		return;
	}
	for (int i = 0; i < MAX_COUNT; ++i) {
		if (m_items[i].rt == rt) {
			m_items[i].available = true;
			return;
		}
	}
}

void RenderTargetMgr::OnSize(int w, int h)
{
	if (w == 0 || h == 0) {
		return;
	}

	if (m_items[0].rt && 
		m_items[0].rt->Width() == w && 
		m_items[0].rt->Height() == h) {
		return;
	}

	for (int i = 0; i < MAX_COUNT; ++i) {
		delete m_items[i].rt;
		m_items[i].rt = new RenderTarget(w, h);
		m_items[i].available = true;
	}
}

void RenderTargetMgr::DebugDraw() const
{
	if (m_items[0].rt) {
		dtex::DebugDraw::Draw(m_items[0].rt->GetTexID(), 1);
	}
	if (m_items[1].rt) {
		dtex::DebugDraw::Draw(m_items[1].rt->GetTexID(), 2);
	}
}

}