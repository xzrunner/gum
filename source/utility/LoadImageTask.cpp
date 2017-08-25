#include "LoadImageTask.h"
#include "Image.h"
#include "RenderContext.h"

#include <gimg_import.h>
#include <gimg_typedef.h>
#include <gimg_pvr.h>
#include <gimg_etc2.h>
#include <bimp/FileLoader.h>
#include <bimp/BIMP_ImportStream.h>
#include <timp/TextureFormat.h>
#include <timp/TextureLoader.h>
#include <unirender/UR_RenderContext.h>

namespace gum
{

int LoadImageTask::m_count = 0;

LoadImageTask::LoadImageTask(Image* img)
	: mt::Task(TASK_TYPE, true)
	, m_img(img)
	, m_data(NULL)
	, m_size(0)
{
	++m_count;

	if (m_img) {
		m_img->SetLoadFinished(false);
		m_img->AddReference();
	}	
}

LoadImageTask::~LoadImageTask()
{
	--m_count;

	if (m_img) {
		m_img->RemoveReference();
	}
	free(m_data);
}

class LoadImageTask::FileLoader : public bimp::FileLoader
{
public:
	FileLoader(const std::string& filepath, bool use_cache, LoadImageTask* task)
		: bimp::FileLoader(filepath, use_cache)
		, m_task(task)
	{}
	FileLoader(fs_file* file, uint32_t offset, bool use_cache, LoadImageTask* task)
		: bimp::FileLoader(file, offset, use_cache)
		, m_task(task)
	{}

protected:
	virtual void OnLoad(bimp::ImportStream& is) 
	{
		m_task->OnLoad(is);
	}

private:
	LoadImageTask* m_task;

}; // LoadImageTask::FileLoader

void LoadImageTask::Run()
{
	if (!m_img) {
		return;
	}
	const ResPath& path = m_img->GetResPath();
	if (path.IsSingleFile()) {
		LoadImageTask::FileLoader loader(path.GetFilepath(), false, this);
		loader.Load();
	} else {
		fs_file* file = fs_open(path.GetFilepath().c_str(), "rb");
		LoadImageTask::FileLoader loader(file, path.GetOffset(), false, this);
		loader.Load();
		fs_close(file);
	}
}

bool LoadImageTask::Finish()
{
	if (!m_data || m_size == 0) {
		return true;
	}

	timp::TextureLoader loader(static_cast<const char*>(m_data), m_size);
	loader.Load();

	const void* pixels = loader.GetData();
	int width = loader.GetWidth(),
		height = loader.GetHeight();
	int format = loader.GetFormat();
	switch (format)
	{
	case timp::TEXTURE_RGBA4: case timp::TEXTURE_RGBA8:
		RenderContext::Instance()->GetImpl()->UpdateTexture(m_img->GetTexID(), pixels, width, height);
		break;
	case timp::TEXTURE_PVR2:
#if defined( __APPLE__ ) && !defined(__MACOSX)
		RenderContext::Instance()->GetImpl()->UpdateTexture(m_img->GetTexID(), pixels, width, height);
#endif
		break;
	case timp::TEXTURE_PVR4:
		{
#if defined( __APPLE__ ) && !defined(__MACOSX)
			RenderContext::Instance()->GetImpl()->UpdateTexture(m_img->GetTexID(), pixels, width, height);
#else
			uint8_t* uncompressed = gimg_pvr_decode(static_cast<const uint8_t*>(pixels), width, height);
			gimg_revert_y(uncompressed, width, height, GPF_RGBA);
			RenderContext::Instance()->GetImpl()->UpdateTexture(m_img->GetTexID(), uncompressed, width, height);
			free(uncompressed);
#endif
		}
		break;
	case timp::TEXTURE_ETC1:
		break;
	case timp::TEXTURE_ETC2:
		{
			ur::RenderContext* rc = RenderContext::Instance()->GetImpl();
			if (rc->IsSupportETC2()) {
				rc->UpdateTexture(m_img->GetTexID(), pixels, width, height);
			} else {
				uint8_t* uncompressed = gimg_etc2_decode(static_cast<const uint8_t*>(pixels), width, height, ETC2PACKAGE_RGBA_NO_MIPMAPS);
				rc->UpdateTexture(m_img->GetTexID(), uncompressed, width, height);
				free(uncompressed);
			}
		}
		break;
	}
	m_img->SetLoadFinished(true);

	return true;
}

void LoadImageTask::OnLoad(bimp::ImportStream& is)
{
	if (m_data) {
		free(m_data); m_data = NULL;
	}
	m_size = is.Size();
	m_data = malloc(m_size);
	if (m_data) {
		memcpy(m_data, is.Stream(), m_size);
	} else {
		m_size = 0;
	}
}

}