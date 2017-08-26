#ifndef _GUM_LOAD_IMAGE_TASK_H_
#define _GUM_LOAD_IMAGE_TASK_H_

#include <CU_Singleton.h>
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

	void Flush();

	void OnLoad(bimp::ImportStream& is);

	void Init(Image* img);
	void Release();

private:
	class FileLoader;

private:
	static const unsigned int TASK_TYPE = 2;

private:
	Image* m_img;

	void*  m_data;
	size_t m_size;

}; // LoadImageTask

class LoadImageTaskMgr
{
public:
	LoadImageTask* Fetch(Image* img);

	void AddResult(LoadImageTask* task);

	bool IsEmpty() { return m_count == 0; }

	void Flush();
	
private:
	int m_count;

	mt::TaskQueue m_freelist;
	mt::SafeTaskQueue m_result;

	SINGLETON_DECLARATION(LoadImageTaskMgr)

}; // LoadImageTaskMgr

}

#endif // _GUM_LOAD_IMAGE_TASK_H_