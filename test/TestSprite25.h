#ifndef _TEST_SPRITE25_H_
#define _TEST_SPRITE25_H_

#include "TestTask.h"

#include <vector>

namespace s2 { class Sprite; }

namespace test
{

class TestSprite25 : public TestTask
{
public:
	virtual void Init();
	virtual void Resize(int width, int height);
	virtual void Draw() const;
	virtual void Update();

private:
	std::vector<s2::Sprite*> m_sprites;
	
}; // TestSprite25

}

#endif // _TEST_SPRITE25_H_