#ifndef _GUM_RENDER_CONTEXT_H_
#define _GUM_RENDER_CONTEXT_H_

#include <CU_Singleton.h>

#include <stdint.h>

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

	int  CreateTexture(const uint8_t* data, int width, int height, int format);
	void ReleaseTexture(int id);
	void UpdateTexture(const uint8_t* data, int w, int h, int id);
	void UpdateSubTex(const uint8_t* data, int x, int y, int w, int h, int id);

//	const Camera* GetCamera() const { return m_cam; }

private:
	OrthoCamera* m_ortho_cam;
	Pseudo3DCamera* m_p3d_cam;

	int m_width, m_height;

	SINGLETON_DECLARATION(RenderContext)
	
}; // RenderContext

}

#endif // _GUM_RENDER_CONTEXT_H_
