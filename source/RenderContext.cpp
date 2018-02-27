#include "gum/RenderContext.h"
#include "gum/RenderTargetMgr.h"

#include <unirender/gl/RenderContext.h>
#include <shaderlab/RenderContext.h>
#include <shaderlab/Blackboard.h>
#include <sprite2/Blackboard.h>

#include <stddef.h>

namespace gum
{

RenderContext::RenderContext() 
	: m_width(0)
	, m_height(0)
{
	int max_texture;
#ifdef EASY_EDITOR
	max_texture = 4096;
#else
	max_texture = 1024;
#endif // S2_EDITOR
	auto ur_rc = std::make_shared<ur::gl::RenderContext>(max_texture);

	m_sl_rc = std::make_shared<sl::RenderContext>(ur_rc);

	ur::gl::RenderContext::Callback cb;
	cb.flush_shader = [&]() {
		m_sl_rc->GetShaderMgr().FlushShader();
	};
	cb.flush_render_shader = [&]() {
		m_sl_rc->GetShaderMgr().FlushRenderShader();
	};
	ur_rc->RegistCB(cb);

	m_ur_rc = ur_rc;
}

void RenderContext::OnSize(int w, int h)
{
	if (m_width == w && m_height == h) {
		return;
	}

	m_width = w;
	m_height = h;

	RenderTargetMgr::Instance()->OnSize(w, h);

	s2::Blackboard::Instance()->SetScreenSize(w, h);
}

void RenderContext::Bind()
{
	sl::Blackboard::Instance()->SetRenderContext(m_sl_rc);
}

}