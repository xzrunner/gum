#include "RenderTarget.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/FilterShader.h>
#include <sprite2/RenderScissor.h>
#include <sprite2/RenderCtxStack.h>

namespace gum
{

RenderTarget::RenderTarget(int width, int height)
	: s2::RenderTarget(width, height)
{
}

void RenderTarget::Draw(const sm::rect& src, const sm::rect& dst, int dst_w, int dst_h) const
{
	sl::ShaderMgr::Instance()->FlushShader();

	float vertices[8], texcoords[8];

	float w, h;
	if (dst_w != 0 && dst_h != 0) {
		w = dst_w;
		h = dst_h;
	} else {
		w = Width();
		h = Height();
	}

	s2::RenderScissor::Instance()->Disable();
	s2::RenderCtxStack::Instance()->Push(s2::RenderContext(w, h, w, h));

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

	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	switch (sl_mgr->GetShaderType()) 
	{
	case sl::SPRITE2:
		{
			sl::Sprite2Shader* shader = static_cast<sl::Sprite2Shader*>(sl_mgr->GetShader());
			shader->SetColor(0xffffffff, 0);
			shader->SetColorMap(0x000000ff, 0x0000ff00, 0x00ff0000);
			shader->DrawQuad(vertices, texcoords, GetTexID());
		}
		break;
	case sl::FILTER:
		{
			sl::FilterShader* shader = static_cast<sl::FilterShader*>(sl_mgr->GetShader());
			shader->SetColor(0xffffffff, 0);
			shader->Draw(vertices, texcoords, GetTexID());
		}
		break;
	default:
		break;
	}

	s2::RenderCtxStack::Instance()->Pop();
	s2::RenderScissor::Instance()->Enable();
}

}