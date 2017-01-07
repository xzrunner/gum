#ifndef _GUM_ASYNC_TASK_H_
#define _GUM_ASYNC_TASK_H_

#include <CU_Singleton.h>

#include <string>

struct tasks_loader;

namespace gum
{

class AsyncTask
{
public:
	void Load(
		const std::string& filepath, 
		void (*load_cb)(const char* filepath, void (*unpack)(const void* data, size_t size, void* ud), void* ud), 
		void (*parser_cb)(const void* data, size_t size, void* ud), 
		void* parser_ud);
	
	void Update();

	void Clear();

private:
	tasks_loader* m_loader;

	SINGLETON_DECLARATION(AsyncTask);

}; // AsyncTask

}

#endif // _GUM_ASYNC_TASK_H_