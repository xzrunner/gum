#include "gum/RenderContext.h"
#include "gum/RenderTargetMgr.h"

#include <unirender/gl/RenderContext.h>
#include <unirender/Blackboard.h>
#include <shaderlab/RenderContext.h>
#include <shaderlab/Blackboard.h>
#include <painting2/Context.h>
#include <painting2/Blackboard.h>
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
	ur::Blackboard::Instance()->SetRenderContext(ur_rc);
	
	m_sl_rc = std::make_shared<sl::RenderContext>();
	sl::Blackboard::Instance()->SetRenderContext(m_sl_rc);

	ur::gl::RenderContext::Callback cb;
	cb.flush_shader = [&]() {
		m_sl_rc->GetShaderMgr().FlushShader();
	};
	cb.flush_render_shader = [&]() {
		m_sl_rc->GetShaderMgr().FlushRenderShader();
	};
	ur_rc->RegistCB(cb);

	m_ur_rc = ur_rc;

	m_pt2_ctx = std::make_shared<pt2::Context>();
	pt2::Blackboard::Instance()->SetContext(m_pt2_ctx);
}

void RenderContext::OnSize(int w, int h)
{
	if (m_width == w && m_height == h) {
		return;
	}

	m_width = w;
	m_height = h;

	m_rt_mgr.OnSize(w, h);

	s2::Blackboard::Instance()->SetScreenSize(w, h);
}

void RenderContext::Bind()
{
	ur::Blackboard::Instance()->SetRenderContext(m_ur_rc);
	sl::Blackboard::Instance()->SetRenderContext(m_sl_rc);
	pt2::Blackboard::Instance()->SetContext(m_pt2_ctx);
}

void RenderContext::Unbind()
{
	m_pt2_ctx.reset();
	m_sl_rc.reset();
	m_ur_rc.reset();

	pt2::Blackboard::Instance()->SetContext(nullptr);
	sl::Blackboard::Instance()->SetRenderContext(nullptr);
	ur::Blackboard::Instance()->SetRenderContext(nullptr);
}

}