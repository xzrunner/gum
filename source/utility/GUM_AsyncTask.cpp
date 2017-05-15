#include <stdlib.h>

#include "GUM_AsyncTask.h"

#include <tasks_loader.h>

#define THREAD_NUM  (4)

namespace gum
{

SINGLETON_DEFINITION(AsyncTask);

AsyncTask::AsyncTask()
{
	m_loader = tasks_loader_create(THREAD_NUM);
}

AsyncTask::~AsyncTask()
{
	tasks_loader_release(m_loader);
	free(m_loader);
	m_loader = NULL;
}

void AsyncTask::Load(const std::string& filepath, 
					 void (*load_cb)(const char* filepath, void (*unpack)(const void* data, size_t size, void* ud), void* ud), 
					 void (*parser_cb)(const void* data, size_t size, void* ud), 
					 void (*release_cb)(void* ud),
					 void* parser_ud)
{
	struct tasks_load_cb params;
	params.load      = load_cb;
	params.parser    = parser_cb;
	params.release   = release_cb;
	params.parser_ud = parser_ud;
	tasks_load_file(m_loader, filepath.c_str(), &params, "");
}

void AsyncTask::Update()
{
	tasks_loader_update(m_loader);
}

void AsyncTask::Clear()
{
	// tasks_loader_clear(m_loader);
	tasks_loader_release(m_loader);
	free(m_loader);
	m_loader = tasks_loader_create(THREAD_NUM);
}

bool AsyncTask::IsEmpty() const
{
	return tasks_loader_empty(m_loader);
}

}