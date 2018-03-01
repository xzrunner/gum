#include "gum/RenderTarget.h"

#include <unirender/RenderContext.h>
#include <unirender/Blackboard.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/RenderContext.h>
#include <shaderlab/Blackboard.h>
#include <painting2/RenderScissor.h>
#include <painting2/Blackboard.h>
#include <painting2/RenderContext.h>
#include <painting2/WindowContext.h>

namespace gum
{

RenderTarget::RenderTarget(int width, int height)
	: pt2::RenderTarget(width, height)
{
}

void RenderTarget::Bind()
{
	auto& pt2_rc = pt2::Blackboard::Instance()->GetRenderContext();
	pt2_rc.GetScissor().Disable();

	int w = Width(),
		h = Height();

	m_old_wc = pt2::Blackboard::Instance()->GetWindowContext();
	auto new_wc = std::make_shared<pt2::WindowContext>(
		static_cast<float>(w), static_cast<float>(h), w, h);
	new_wc->Bind();
	pt2::Blackboard::Instance()->SetWindowContext(new_wc);
	// use last model view
	new_wc->SetModelView(m_old_wc->GetMVOffset(), m_old_wc->GetMVScale());

	pt2::RenderTarget::Bind();
}

void RenderTarget::Unbind()
{
	m_old_wc->Bind();
	pt2::Blackboard::Instance()->SetWindowContext(m_old_wc);

	auto& pt2_rc = pt2::Blackboard::Instance()->GetRenderContext();
	pt2_rc.GetScissor().Enable();

	pt2::RenderTarget::Unbind();
}

void RenderTarget::Draw(const sm::rect& src, const sm::rect& dst, int dst_w, int dst_h) const
{
	auto& shader_mgr = sl::Blackboard::Instance()->GetRenderContext().GetShaderMgr();
	shader_mgr.FlushShader();

	float vertices[8], texcoords[8];

	int w, h;
	if (dst_w != 0 && dst_h != 0) {
		w = dst_w;
		h = dst_h;
	} else {
		w = Width();
		h = Height();
	}

	auto old_wc = pt2::Blackboard::Instance()->GetWindowContext();
	auto new_wc = std::make_shared<pt2::WindowContext>(static_cast<float>(w), static_cast<float>(h), w, h);
	int vp_x, vp_y, vp_w, vp_h;
	old_wc->GetViewport(vp_x, vp_y, vp_w, vp_h);
	new_wc->SetViewport(vp_x, vp_y, vp_w, vp_h);
	new_wc->Bind();
	pt2::Blackboard::Instance()->SetWindowContext(new_wc);

	auto& pt2_rc = pt2::Blackboard::Instance()->GetRenderContext();
	pt2_rc.GetScissor().Disable();

	float hw = w * 0.5f,
		  hh = h * 0.5f;
	vertices[0] = w * dst.xmin - hw; vertices[1] = h * dst.ymin - hh;
	vertices[2] = w * dst.xmax - hw; vertices[3] = h * dst.ymin - hh; 
	vertices[4] = w * dst.xmax - hw; vertices[5] = h * dst.ymax - hh;
	vertices[6] = w * dst.xmin - hw; vertices[7] = h * dst.ymax - hh;

	texcoords[0] = src.xmin; texcoords[1] = src.ymin;
	texcoords[2] = src.xmax; texcoords[3] = src.ymin;
	texcoords[4] = src.xmax; texcoords[5] = src.ymax;
	texcoords[6] = src.xmin; texcoords[7] = src.ymax;

	switch (shader_mgr.GetShaderType()) 
	{
	case sl::SPRITE2:
		{
			sl::Sprite2Shader* shader = static_cast<sl::Sprite2Shader*>(shader_mgr.GetShader());
			shader->SetColor(0xffffffff, 0);
			shader->SetColorMap(0x000000ff, 0x0000ff00, 0x00ff0000);
			shader->DrawQuad(vertices, texcoords, GetTexID());
		}
		break;
	case sl::FILTER:
		{
			sl::FilterShader* shader = static_cast<sl::FilterShader*>(shader_mgr.GetShader());
			shader->SetColor(0xffffffff, 0);
			shader->Draw(vertices, texcoords, GetTexID());
		}
		break;
	default:
		break;
	}

	old_wc->Bind();
	pt2::Blackboard::Instance()->SetWindowContext(old_wc);

	pt2_rc.GetScissor().Enable();
}

void RenderTarget::Clear()
{
	Bind();

	auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
	ur_rc.Clear(0);

	Unbind();
}

}