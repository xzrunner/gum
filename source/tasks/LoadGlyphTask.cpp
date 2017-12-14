#include "gum/LoadGlyphTask.h"
#include "gum/DTex.h"
#include "gum/ThreadPool.h"

#include <assert.h>

namespace gum
{

LoadGlyphTask::LoadGlyphTask(int thread_idx, int unicode, 
	                         const gtxt_glyph_style* gs, UID uid)
	: m_thread_idx(thread_idx)
	, m_unicode(unicode)
	, m_gs(*gs)
	, m_uid(uid)
{
}

void LoadGlyphTask::Initialize(int thread_idx, int unicode, 
	                           const gtxt_glyph_style* gs, UID uid)
{
	m_thread_idx = thread_idx;
	m_unicode = unicode;
	m_gs = *gs;
	m_uid = uid;
}

/************************************************************************/
/* class LoadGlyphTaskMgr                                               */
/************************************************************************/

CU_SINGLETON_DEFINITION(LoadGlyphTaskMgr)

LoadGlyphTaskMgr::LoadGlyphTaskMgr()
{
	int thread_count = ThreadPool::Instance()->GetThreadCount();
	m_freelists.resize(thread_count);
	m_results.resize(thread_count);
}

void LoadGlyphTaskMgr::Add(int thread_idx, int unicode, const gtxt_glyph_style* gs, UID uid)
{
	assert(thread_idx >= 0 && thread_idx < static_cast<int>(m_freelists.size()));
	LoadGlyphTask* task = m_freelists[thread_idx];
	if (!task) {
		task = new LoadGlyphTask(thread_idx, unicode, gs, uid);
	} else {
		task = m_freelists[thread_idx];
		task->SetNext(nullptr);
		m_freelists[thread_idx] = static_cast<LoadGlyphTask*>(task->GetNext());
		task->Initialize(thread_idx, unicode, gs, uid);
		task->RemoveReference();
	}

	assert(task->m_thread_idx >= 0 && task->m_thread_idx < static_cast<int>(m_results.size()));
	LoadGlyphTask* last = m_results[task->m_thread_idx];
	task->SetNext(last);
	m_results[task->m_thread_idx] = task;
	task->AddReference();
}

void LoadGlyphTaskMgr::Flush()
{
	for (int i = 0, n = m_results.size(); i < n; ++i)
	{
		LoadGlyphTask* t = m_results[i];
		while (t)
		{
			struct gtxt_glyph_layout layout;
			uint32_t* bmp = gtxt_glyph_get_bitmap(t->m_unicode, &t->m_gs, &layout);
			if (!bmp) {
				return;
			}
			int w = static_cast<int>(layout.sizer.width);
			int h = static_cast<int>(layout.sizer.height);
			DTex::Instance()->LoadGlyph(bmp, w, h, t->m_uid);

			LoadGlyphTask* next = static_cast<LoadGlyphTask*>(t->GetNext());
			t->SetNext(m_freelists[i]);
			m_freelists[i] = t;
			t = next;
		}
		m_results[i] = nullptr;
	}
}

}