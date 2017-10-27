#ifndef _GUM_IMAGE_SYM_LOADER_H_
#define _GUM_IMAGE_SYM_LOADER_H_

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

namespace bimp { class FilePath; }

namespace gum
{

class ImageSymbol;

class ImageSymLoader : private cu::Uncopyable
{
public:
	ImageSymLoader(ImageSymbol& sym);

	void Load(const bimp::FilePath& res_path, float scale, bool async);

private:
	ImageSymbol& m_sym;

}; // ImageSymLoader

}

#endif // _GUM_IMAGE_SYM_LOADER_H_