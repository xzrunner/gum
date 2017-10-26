#ifndef _GUM_TEXTURE_H_
#define _GUM_TEXTURE_H_

#include <unirender/typedef.h>
#include <cu/cu_stl.h>

#include <stdint.h>

namespace gum
{

class Texture
{
public:
	Texture();
	~Texture();

	void Load(const CU_STR& filepath);
	void Load(const uint8_t* data, int width, int height, ur::TEXTURE_FORMAT format);

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

	int GetID() const { return m_id; }

private:
	void Load();

private:
	int m_width, m_height;
	ur::TEXTURE_FORMAT m_format;

	int m_id;

}; // Texture

}

#endif // _GUM_TEXTURE_H_
