#include "GUM_DRect.h"
#include "RenderContext.h"

#include <drect/RenderTarget.h>
#include <dtex2/DebugDraw.h>
#include <sprite2/S2_RVG.h>
#include <sprite2/Color.h>
#include <sprite2/RenderCtxStack.h>

namespace gum
{

SINGLETON_DEFINITION(DRect);

static const int EXTEND = 512;

DRect::DRect()
{
	RenderContext* rc = RenderContext::Instance();
	m_rt = new drect::RenderTarget(rc->GetImpl(), rc->GetWidth(), rc->GetHeight(), EXTEND);
}

void DRect::Bind()
{
	RenderContext* rc = RenderContext::Instance();
	int w = rc->GetWidth() + EXTEND * 2,
		h = rc->GetHeight() + EXTEND * 2;
	s2::RenderCtxStack::Instance()->Push(s2::RenderCtx(w, h, w, h));

	m_rt->Bind();
}

void DRect::Unbind()
{
	m_rt->Unbind();

	s2::RenderCtxStack::Instance()->Pop();
}

void DRect::DebugDraw() const
{
	dtex::DebugDraw::Draw(m_rt->GetTexID(), 2);

	s2::RVG::SetColor(s2::Color(255, 0, 0));
	s2::RVG::Rect(sm::vec2(0, 0), 0.5f, 0.5f, false);
}

}