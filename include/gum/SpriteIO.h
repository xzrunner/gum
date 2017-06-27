#ifndef _GUM_SPRITE_IO_H_
#define _GUM_SPRITE_IO_H_

#include <SM_Vector.h>
#include <sprite2/RenderColor.h>
#include <sprite2/BlendMode.h>
#include <sprite2/FastBlendMode.h>
#include <sprite2/RenderFilter.h>
#include <sprite2/CameraMode.h>

#include <json/json.h>

namespace s2 { class Sprite; class RenderShader; class RenderCamera; }

namespace gum
{

class SpriteIO
{
public:
	SpriteIO(bool compress, bool render_open);
	~SpriteIO();

	void Load(const Json::Value& val, s2::Sprite* spr, const std::string& dir);
	void Store(Json::Value& val, const s2::Sprite* spr, const std::string& dir);

	void Load(const Json::Value& val, const std::string& dir);
	void Store(Json::Value& val, const std::string& dir);

protected:
	// geometry
	virtual void LoadGeometry(s2::Sprite* spr);
	virtual void StoreGeometry(const s2::Sprite* spr);
	virtual void LoadGeometry(const Json::Value& val);
	virtual void StoreGeometry(Json::Value& val);

	// render
	virtual void LoadRender(s2::Sprite* spr);
	virtual void StoreRender(const s2::Sprite* spr);
	virtual void LoadRender(const Json::Value& val, const std::string& dir);
	virtual void StoreRender(Json::Value& val, const std::string& dir);

	// info
	virtual void LoadInfo(s2::Sprite* spr);
	virtual void StoreInfo(const s2::Sprite* spr);
	virtual void LoadInfo(const Json::Value& val);
	virtual void StoreInfo(Json::Value& val);

	// edit
	virtual void LoadEdit(s2::Sprite* spr);
	virtual void StoreEdit(const s2::Sprite* spr);
	virtual void LoadEdit(const Json::Value& val);
	virtual void StoreEdit(Json::Value& val);

private:
	void LoadColor(s2::Sprite* spr);
	void StoreColor(const s2::RenderColor& color);
	void LoadColor(const Json::Value& val);
	void StoreColor(Json::Value& val);

	void LoadShader(s2::Sprite* spr);
	void StoreShader(const s2::RenderShader& shader);
	void LoadShader(const Json::Value& val, const std::string& dir);
	void StoreShader(Json::Value& val, const std::string& dir);

	void LoadCamera(s2::Sprite* spr);
	void StoreCamera(const s2::RenderCamera& camera);	
	void LoadCamera(const Json::Value& val);
	void StoreCamera(Json::Value& val);

public:
	// geometry
	sm::vec2			m_position;
	float				m_angle;
	sm::vec2			m_scale;
	sm::vec2			m_shear;
	sm::vec2			m_offset;

	// shader
	s2::RenderColor		m_col;
	s2::BlendMode		m_blend;
	s2::FastBlendMode	m_fast_blend;
	s2::RenderFilter*	m_filter;
	float               m_downsample;
	s2::CameraMode		m_camera;
	
	// info
	std::string			m_name;
	bool                m_need_actor;

	// edit
	bool				m_visible;
	bool				m_editable;

protected:
	bool m_compress;
	bool m_render_open;

}; // SpriteDataIO

}

#endif // _GUM_SPRITE_IO_H_
