#ifndef _GUM_IMAGE_SYM_LOADER_H_
#define _GUM_IMAGE_SYM_LOADER_H_

#include <cu/uncopyable.h>

#include <string>
#include <memory>

namespace gum
{

class ImageSymbol;
class ResPath;

class ImageSymLoader : private cu::Uncopyable
{
public:
	ImageSymLoader(const std::shared_ptr<ImageSymbol>& sym);

	void Load(const ResPath& res_path, float scale, bool async);

private:
	std::shared_ptr<ImageSymbol> m_sym;

}; // ImageSymLoader

}

#endif // _GUM_IMAGE_SYM_LOADER_H_