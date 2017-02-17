#ifndef _GUM_RENDER_TARGET_MGR_H_
#define _GUM_RENDER_TARGET_MGR_H_

#include <CU_Singleton.h>
#include <SM_Vector.h>

#include <vector>

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
	sm::ivec2 m_size;
	
	std::vector<Item> m_items;

	SINGLETON_DECLARATION(RenderTargetMgr);

}; // RenderTargetMgr

}

#endif // _GUM_RENDER_TARGET_MGR_H_