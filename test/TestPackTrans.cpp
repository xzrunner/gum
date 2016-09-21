#include "TestPackTrans.h"

#include <simp/NodeFactory.h>
#include <simp/Package.h>
#include <sprite2/DrawNode.h>
#include <sprite2/ComplexSprite.h>
#include <gum/RenderContext.h>
#include <gum/SymbolFactory.h>
#include <gum/SpriteFactory.h>
#include <shaderlab.h>

namespace test
{

void TestPackTrans::Init()
{
	sl::ShaderMgr* sl_mgr = sl::ShaderMgr::Instance();
	sl_mgr->CreateContext(4096);
	sl::RenderContext* sl_rc = sl_mgr->GetContext();
	sl_mgr->CreateShader(sl::SPRITE2, new sl::Sprite2Shader(sl_rc));
	sl_mgr->CreateShader(sl::FILTER, new sl::FilterShader(sl_rc));
	gum::RenderContext::Instance()->SetCamera(0, 0, 1, 1);

 	simp::Package* pkg = new simp::Package("pack/test");
	simp::NodeFactory::Instance()->AddPkg(pkg, "test", 0);

	{
		s2::Symbol* sym = gum::SymbolFactory::Instance()->Create("test", "img");
		s2::Sprite* spr = new s2::ComplexSprite(sym);
		spr->SetPosition(sm::vec2(0, 0));
		m_sprites.push_back(spr);
	}
}

void TestPackTrans::Resize(int width, int height)
{
	gum::RenderContext::Instance()->OnSize(width, height);
}

void TestPackTrans::Draw() const
{
	for (int i = 0, n = m_sprites.size(); i < n; ++i) 
	{
		s2::Sprite* spr = m_sprites[i];
		s2::RenderParams params;
		params.mt = spr->GetTransMatrix();
		params.color = spr->Color();
		s2::DrawNode::Draw(spr, params);
	}
}

void TestPackTrans::Update()
{
	for (int i = 0, n = m_sprites.size(); i < n; ++i) 
	{
		s2::Sprite* spr = m_sprites[i];

//		spr->Translate(sm::vec2(0.1, 0.1));

		s2::RenderParams params;
		params.mt = spr->GetTransMatrix();
		params.color = spr->Color();
		spr->Update(params);
	}
}

}