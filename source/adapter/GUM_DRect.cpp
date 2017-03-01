//#include "GUM_DRect.h"
//#include "RenderContext.h"
//
//#include <SM_Calc.h>
//#include <drect/RenderTarget.h>
//#include <dtex2/DebugDraw.h>
//#include <shaderlab/ShaderMgr.h>
//#include <shaderlab/Sprite2Shader.h>
//#include <sprite2/S2_RVG.h>
//#include <sprite2/Color.h>
//#include <sprite2/RenderCtxStack.h>
//
//namespace gum
//{
//
//SINGLETON_DEFINITION(DRect);
//
//DRect::DRect()
//	: m_draw_init(false)
//{
//	RenderContext* rc = RenderContext::Instance();
//	m_rt = new drect::RenderTarget(rc->GetImpl(), rc->GetWidth(), rc->GetHeight(), EXTEND);
//
//	int w = rc->GetWidth() + EXTEND * 2,
//		h = rc->GetHeight() + EXTEND * 2;
//	m_ctx.SetScreen(w, h);
//	m_ctx.SetProjection(w, h);
//}
//
//void DRect::Bind()
//{
//	s2::RenderCtxStack::Instance()->Push(m_ctx);
//
//	m_rt->Bind();
//}
//
//void DRect::Unbind()
//{
//	m_rt->Unbind();
//
//	s2::RenderCtxStack::Instance()->Pop();
//}
//
//bool DRect::Draw(const sm::vec2& offset, float scale)
//{
//	if (!m_draw_init)
//	{
//		m_ctx.SetModelView(offset, scale);
//		m_draw_init = true;
//		return false;
//	}
//
//	RenderContext* rc = RenderContext::Instance();
//	float dst_hw = rc->GetWidth() * 0.5f,
//		  dst_hh = rc->GetHeight() * 0.5f;
//	sm::rect dst_rect;
//	dst_rect.xmin = (-dst_hw) / scale - offset.x;
//	dst_rect.ymin = (-dst_hh) / scale - offset.y;
//	dst_rect.xmax = dst_hw / scale - offset.x;
//	dst_rect.ymax = dst_hh / scale - offset.y;
//
//	sm::rect src_rect;
//	float src_hw = dst_hw + EXTEND,
//		  src_hh = dst_hh + EXTEND;
//	float src_scale = m_ctx.GetMVScale();
//	sm::vec2 src_offset = m_ctx.GetMVOffset();
//	src_rect.xmin = (-src_hw) / src_scale + src_offset.x;
//	src_rect.ymin = (-src_hh) / src_scale + src_offset.y;
//	src_rect.xmax = src_hw / src_scale + src_offset.x;
//	src_rect.ymax = src_hh / src_scale + src_offset.y;
//
//	if (!sm::is_rect_contain_rect(src_rect, dst_rect)) {
//		m_ctx.SetModelView(offset, scale);
//		return false;
//	}
//
//	s2::RenderCtxStack::Instance()->Push(s2::RenderContext(rc->GetWidth(), rc->GetHeight(), rc->GetWidth(), rc->GetHeight()));
//
//	float vertices[8];
//	vertices[0] = -dst_hw; vertices[1] = -dst_hh;
//	vertices[2] =  dst_hw; vertices[3] = -dst_hh;
//	vertices[4] =  dst_hw; vertices[5] =  dst_hh;
//	vertices[6] = -dst_hw; vertices[7] =  dst_hh;
//
//	float texcoords[8];
//	sm::vec2 sz = src_rect.Size();
//	float xmin = (dst_rect.xmin - src_rect.xmin) / sz.x,
//		  ymin = (dst_rect.ymin - src_rect.ymin) / sz.y,
//		  xmax = (dst_rect.xmax - src_rect.xmin) / sz.x,
//		  ymax = (dst_rect.ymax - src_rect.ymin) / sz.y;
//	texcoords[0] = xmin; texcoords[1] = ymin;
//	texcoords[2] = xmax; texcoords[3] = ymin;
//	texcoords[4] = xmax; texcoords[5] = ymax;
//	texcoords[6] = xmin; texcoords[7] = ymax;
//
//	sl::ShaderMgr::Instance()->SetShader(sl::SPRITE2);
//	sl::Sprite2Shader* shader = static_cast<sl::Sprite2Shader*>(sl::ShaderMgr::Instance()->GetShader());
//	shader->SetColor(0xffffffff, 0);
//	shader->SetColorMap(0x000000ff, 0x0000ff00, 0x00ff0000);
//	shader->Draw(vertices, texcoords, m_rt->GetTexID());
//
//	s2::RenderCtxStack::Instance()->Pop();
//
//	return true;
//}
//
//void DRect::DebugDraw() const
//{
//	dtex::DebugDraw::Draw(m_rt->GetTexID(), 2);
//}
//
//}