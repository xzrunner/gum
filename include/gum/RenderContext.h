#ifndef _GUM_RENDER_CONTEXT_H_
#define _GUM_RENDER_CONTEXT_H_

#include <render/render.h>

#include <CU_Singleton.h>

namespace gum
{

class OrthoCamera;
class Pseudo3DCamera;

class RenderContext
{
public:
	void Init();

	void OnSize(int w, int h);
	void SetCamera(float x, float y, float sx, float sy);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	RID CreateTexture(const uint8_t* data, int width, int height, TEXTURE_FORMAT format);
	void ReleaseTexture(RID id);
	void UpdateTexture(const uint8_t* data, int w, int h, int id);

//	const Camera* GetCamera() const { return m_cam; }

private:
	OrthoCamera* m_ortho_cam;
	Pseudo3DCamera* m_p3d_cam;

	int m_width, m_height;

	SINGLETON_DECLARATION(RenderContext)
	
}; // RenderContext

}

#endif // _GUM_RENDER_CONTEXT_H_
