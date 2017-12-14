#ifndef _GUM_LOAD_GLYPH_TASK_H_
#define _GUM_LOAD_GLYPH_TASK_H_

#include "gum/ResourceUID.h"

#include <cu/cu_macro.h>
#include <multitask/Task.h>
#include <gtxt/gtxt_glyph.h>

namespace gum
{

class LoadGlyphTask : public mt::Task
{
public:
	LoadGlyphTask(int thread_idx, int unicode, const gtxt_glyph_style* gs, UID uid);

	virtual void Run() override {}

	void Initialize(int thread_idx, int unicode, const gtxt_glyph_style* gs, UID uid);

private:
	int m_thread_idx;

	int m_unicode;
	gtxt_glyph_style m_gs;

	UID m_uid;

	friend class LoadGlyphTaskMgr;

}; // LoadGlyphTask

class LoadGlyphTaskMgr
{
public:
	void Add(int thread_idx, int unicode, const gtxt_glyph_style* gs, UID uid);

	void Flush();

private:
	std::vector<LoadGlyphTask*> m_freelists;
	std::vector<LoadGlyphTask*> m_results;

	CU_SINGLETON_DECLARATION(LoadGlyphTaskMgr)

}; // LoadGlyphTaskMgr

}

#endif // _GUM_LOAD_GLYPH_TASK_H_