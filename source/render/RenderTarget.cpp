#include "RenderTarget.h"

#include <unirender/RenderContext.h>
#include <unirender/RenderTarget.h>
#include <unirender/Texture.h>
#include <shaderlab/ShaderMgr.h>
#include <dtex2/DebugDraw.h>

namespace gum
{

SINGLETON_DEFINITION(RenderTarget);

RenderTarget::RenderTarget()
	: m_screen0(NULL)
	, m_screen1(NULL)
{
}

void RenderTarget::OnSize(ur::RenderContext* rc, int w, int h)
{
	if (w == 0 || h == 0) {
		return;
	}
	if (m_screen0 && 
		m_screen0->GetTexture()->Width() == w &&
		m_screen0->GetTexture()->Height() == h) {
		return;
	}

	delete m_screen0;
	delete m_screen1;

	m_screen0 = new ur::RenderTarget(rc, w, h);
	m_screen1 = new ur::RenderTarget(rc, w, h);
}

void RenderTarget::DebugDraw() const
{
	dtex::DebugDraw::Draw(m_screen0->GetTexture()->ID(), 1);
}

}