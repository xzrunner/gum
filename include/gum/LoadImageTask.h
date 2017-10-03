#ifndef _GUM_LOAD_IMAGE_TASK_H_
#define _GUM_LOAD_IMAGE_TASK_H_

#include <cu/cu_macro.h>
#include <multitask/Task.h>

namespace bimp { class ImportStream; }

namespace gum
{

class Image;
class LoadImageTask : public mt::Task
{
public:
	LoadImageTask(const std::shared_ptr<Image>& img);
	virtual ~LoadImageTask();

	virtual void Run();

	void Flush();

	void OnLoad(bimp::ImportStream& is);

	void Initialize(const std::shared_ptr<Image>& img);
	void Terminate();

private:
	class FileLoader;

private:
	std::shared_ptr<Image> m_img;

	void*  m_data;
	size_t m_size;

}; // LoadImageTask

class LoadImageTaskMgr
{
public:
	LoadImageTask* Fetch(const std::shared_ptr<Image>& img);

	void AddResult(LoadImageTask* task);

	bool IsEmpty() { return m_count == 0; }

	void Flush();
	
private:
	int m_count;

	mt::TaskQueue m_freelist;
	mt::SafeTaskQueue m_result;

	CU_SINGLETON_DECLARATION(LoadImageTaskMgr)

}; // LoadImageTaskMgr

}

#endif // _GUM_LOAD_IMAGE_TASK_H_