#ifndef _GUM_IMAGE_LOADER_H_
#define _GUM_IMAGE_LOADER_H_

#include <string>

namespace gum
{

class ImageLoader
{
public:
	ImageLoader(const std::string& filepath);

	bool Load();
	bool AsyncLoad(int format, int width, int height);

	int GetID() const { return m_id; }
	int GetFormat() const { return m_format; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	bool LoadRaw();
	bool LoadBin();

	bool DecodePVR2(const void* data);
	bool DecodePVR4(const void* data);
	bool DecodeETC2(const void* data);
	
	static void LoadTextureCB(int format, int w, int h, const void* data, void* ud);

private:
	std::string m_filepath;

	int m_id;
	int m_format;
	int m_width, m_height;

}; // ImageLoader

}

#endif // _GUM_IMAGE_LOADER_H_