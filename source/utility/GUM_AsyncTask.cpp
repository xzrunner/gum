#include "GUM_AsyncTask.h"

#include <tasks_loader.h>

namespace gum
{

SINGLETON_DEFINITION(AsyncTask);

AsyncTask::AsyncTask()
{
	m_loader = tasks_loader_create(4);
}

AsyncTask::~AsyncTask()
{
	tasks_loader_release(m_loader);
}

void AsyncTask::Load(const std::string& filepath, 
					 void (*load_cb)(const char* filepath, void (*unpack)(const void* data, size_t size, void* ud), void* ud), 
					 void (*parser_cb)(const void* data, size_t size, void* ud), 
					 void* parser_ud)
{
	struct tasks_load_cb params;
	params.load = load_cb;
	params.parser = parser_cb;
	params.parser_ud = parser_ud;
	tasks_load_file(m_loader, filepath.c_str(), &params, "");
}

void AsyncTask::Update()
{
	tasks_loader_update(m_loader);
}

void AsyncTask::Clear()
{
	tasks_loader_clear(m_loader);
}

bool AsyncTask::IsEmpty() const
{
	return tasks_loader_empty(m_loader);
}

}