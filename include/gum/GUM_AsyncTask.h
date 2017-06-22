#ifndef _GUM_ASYNC_TASK_H_
#define _GUM_ASYNC_TASK_H_

#include <CU_Singleton.h>

struct tasks_loader;

namespace gum
{

class ResPath;

class AsyncTask
{
public:
	void Load(
		const ResPath& res_path, 
		void (*load_cb)(const void* res_path, void (*unpack)(const void* data, size_t size, void* ud), void* ud), 
		void (*parser_cb)(const void* data, size_t size, void* ud), 
		void (*release_cb)(void* ud),
		void* parser_ud);
	
	void Update();

	void Clear();

	bool IsEmpty() const;

private:
	tasks_loader* m_loader;

	SINGLETON_DECLARATION(AsyncTask);

}; // AsyncTask

}

#endif // _GUM_ASYNC_TASK_H_