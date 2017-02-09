#ifndef _GUM_RENDER_TARGET_MGR_H_
#define _GUM_RENDER_TARGET_MGR_H_

#include <CU_Singleton.h>

namespace gum
{

class RenderTarget;

class RenderTargetMgr
{
public:
	RenderTarget* Fetch();
	void Return(RenderTarget* rt);

	void OnSize(int w, int h);

	void DebugDraw() const;

private:
	struct Item
	{
		RenderTarget* rt;
		bool available;

		Item() : rt(NULL), available(true) {}
	};

private:
	static const int MAX_COUNT = 2;

private:
	Item m_items[MAX_COUNT];

	SINGLETON_DECLARATION(RenderTargetMgr);

}; // RenderTargetMgr

}

#endif // _GUM_RENDER_TARGET_MGR_H_