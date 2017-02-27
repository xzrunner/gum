#include "RenderTargetMgr.h"
#include "RenderTarget.h"

#include <dtex2/DebugDraw.h>

#include <assert.h>

namespace gum
{

SINGLETON_DEFINITION(RenderTargetMgr);

RenderTargetMgr::RenderTargetMgr()
	: m_size(0, 0)
{
}

RenderTarget* RenderTargetMgr::Fetch()
{
	for (int i = 0, n = m_items.size(); i < n; ++i) 
	{
		if (!m_items[i].available) {
			continue;
		}
		m_items[i].available = false;
		assert(m_items[i].rt);
		return m_items[i].rt;
	}

	Item item;
	item.rt = new RenderTarget(m_size.x, m_size.y);
	item.available = false;
	m_items.push_back(item);
	return item.rt;
}

void RenderTargetMgr::Return(RenderTarget* rt)
{
	if (!rt) {
		return;
	}

	for (int i = 0, n = m_items.size(); i < n; ++i) {
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

	if (m_size.x == w && m_size.y == h) {
		return;
	}
	m_size.x = w;
	m_size.y = h;

	for (int i = 0, n = m_items.size(); i < n; ++i) {
		if (m_items[i].rt) {
			m_items[i].rt->Resize(w, h);
		} else {
			m_items[i].rt = new RenderTarget(w, h);
		}
	}
}

void RenderTargetMgr::DebugDraw() const
{
	int n = std::min(4, static_cast<int>(m_items.size()));
	for (int i = 0; i < n; ++i) {
		dtex::DebugDraw::Draw(m_items[i].rt->GetTexID(), i + 1);
	}
}

}