#pragma once

#include <cu/cu_macro.h>

#include <memory>

namespace gum
{

class RenderContext;

class Blackboard
{
public:
	void SetRenderContext(const std::shared_ptr<RenderContext>& rc) { m_rc = rc; }
	const std::shared_ptr<RenderContext>& GetRenderContext() const { return m_rc; }

private:
	std::shared_ptr<RenderContext> m_rc = nullptr;

	CU_SINGLETON_DECLARATION(Blackboard);

}; // Blackboard

}