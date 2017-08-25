#ifndef _GUM_LOAD_IMAGE_TASK_H_
#define _GUM_LOAD_IMAGE_TASK_H_

#include <multitask/Task.h>

namespace bimp { class ImportStream; }

namespace gum
{

class Image;
class LoadImageTask : public mt::Task
{
public:
	LoadImageTask(Image* img);
	virtual ~LoadImageTask();

	virtual void Run();
	virtual bool Finish();

	void OnLoad(bimp::ImportStream& is);

	static bool IsTaskEmpty() { return m_count == 0; }

private:
	class FileLoader;

private:
	static const unsigned int TASK_TYPE = 2;

private:
	Image* m_img;

	void*  m_data;
	size_t m_size;

	static int m_count;

}; // LoadImageTask

}

#endif // _GUM_LOAD_IMAGE_TASK_H_