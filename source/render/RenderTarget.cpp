#include "RenderTarget.h"
#include "RenderContext.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/FilterShader.h>
#include <sprite2/RenderCtxStack.h>

namespace gum
{

RenderTarget::RenderTarget(int width, int height)
	: s2::RenderTarget(width, height)
{
}

void RenderTarget::Draw() const
{
	s2::RenderContext* s2_ctx = const_cast<s2::RenderContext*>(s2::RenderCtxStack::Instance()->Top());
	if (!s2_ctx) {
		return;
	}
	s2_ctx->SetModelView(sm::vec2(0, 0), 1);

	float vertices[8], texcoords[8];

	RenderContext* ctx = RenderContext::Instance();
	float hw = ctx->GetWidth() * 0.5f,
		  hh = ctx->GetHeight() * 0.5f;
	vertices[0] = -hw; vertices[1] = -hh;
	vertices[2] =  hw; vertices[3] = -hh; 
	vertices[4] =  hw; vertices[5] =  hh;
	vertices[6] = -hw; vertices[7] =  hh;

	texcoords[0] = 0; texcoords[1] = 0;
	texcoords[2] = 1; texcoords[3] = 0;
	texcoords[4] = 1; texcoords[5] = 1;
	texcoords[6] = 0; texcoords[7] = 1;

	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	switch (sl_mgr->GetShaderType()) 
	{
	case sl::SPRITE2:
		{
			sl::Sprite2Shader* shader = static_cast<sl::Sprite2Shader*>(sl_mgr->GetShader());
			shader->SetColor(0xffffffff, 0);
			shader->SetColorMap(0x000000ff, 0x0000ff00, 0x00ff0000);
			shader->Draw(vertices, texcoords, GetTexID());
		}
		break;
	case sl::FILTER:
		{
			sl::FilterShader* shader = static_cast<sl::FilterShader*>(sl_mgr->GetShader());
			shader->SetColor(0xffffffff, 0);
			shader->Draw(vertices, texcoords, GetTexID());
		}
		break;
	}
}

}