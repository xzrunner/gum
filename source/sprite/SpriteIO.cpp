#include "SpriteIO.h"
#include "trans_color.h"
#include "BlendModes.h"
#include "CameraModes.h"
#include "FastBlendModes.h"
#include "FilterModes.h"

#include <sprite2/S2_Sprite.h>
#include <sprite2/FilterFactory.h>
#include <sprite2/RFEdgeDetection.h>
#include <sprite2/RFGaussianBlur.h>
#include <sprite2/RFOuterGlow.h>
#include <sprite2/RenderShader.h>
#include <sprite2/RenderCamera.h>

namespace gum
{

SpriteIO::SpriteIO(bool m_compress, bool render_open)
	: m_compress(m_compress)
	, m_render_open(render_open)
{
	m_position		= sm::vec2(0, 0);
	m_angle			= 0;
	m_scale			= sm::vec2(1, 1);
	m_shear			= sm::vec2(0, 0);
	m_offset		= sm::vec2(0, 0);

	m_blend			= s2::BM_NULL;
	m_fast_blend	= s2::FBM_NULL;
	m_filter		= NULL;
	m_camera		= s2::CM_ORTHO;

	m_visible		= true;
	m_editable		= true;
}

SpriteIO::~SpriteIO() 
{ 
	if (m_filter) {
		delete m_filter; 
	}
}

void SpriteIO::Load(const Json::Value& val, s2::Sprite* spr)
{
	Load(val);

	LoadGeometry(spr);
	LoadRender(spr);
	LoadInfo(spr);
	LoadEdit(spr);
}

void SpriteIO::Store(Json::Value& val, const s2::Sprite* spr)
{
	StoreGeometry(spr);
	StoreRender(spr);
	StoreInfo(spr);
	StoreEdit(spr);

	Store(val);
}

void SpriteIO::Load(const Json::Value& val)
{
	LoadGeometry(val);
	LoadRender(val);
	LoadInfo(val);
	LoadEdit(val);
}

void SpriteIO::Store(Json::Value& val)
{
	StoreGeometry(val);
	StoreRender(val);
	StoreInfo(val);
	StoreEdit(val);
}

/************************************************************************/
/* geometry                                                             */
/************************************************************************/

void SpriteIO::LoadGeometry(s2::Sprite* spr)
{
	spr->SetShear(m_shear);
	spr->SetScale(m_scale);
	spr->SetOffset(m_offset);
	spr->SetPosition(m_position);
	spr->SetAngle(m_angle);
}

void SpriteIO::StoreGeometry(const s2::Sprite* spr)
{
	m_position	= spr->GetPosition();
	m_angle		= spr->GetAngle();
	m_scale		= spr->GetScale();
	m_shear		= spr->GetShear();
	m_offset	= spr->GetOffset();
}

void SpriteIO::LoadGeometry(const Json::Value& val)
{
	// scale
	sm::vec2 scale(1, 1);
	if (val.isMember("x scale") && val.isMember("y scale"))
	{
		scale.x = static_cast<float>(val["x scale"].asDouble());
		scale.y = static_cast<float>(val["y scale"].asDouble());
	}
	else if (val.isMember("scale"))
	{
		scale.x = scale.y = static_cast<float>(val["scale"].asDouble());
	}
	m_scale = scale;

	// shear
	sm::vec2 shear(0, 0);
	if (val.isMember("x shear") && val.isMember("y shear"))
	{
		shear.x = static_cast<float>(val["x shear"].asDouble());
		shear.y = static_cast<float>(val["y shear"].asDouble());
	}
	m_shear = shear;

	// offset
	sm::vec2 offset(0 ,0);
	if (val.isMember("x offset") && val.isMember("y offset"))
	{
		offset.x = static_cast<float>(val["x offset"].asDouble());
		offset.y = static_cast<float>(val["y offset"].asDouble());
	}
	m_offset = offset;

	// translate
	sm::vec2 pos(0, 0);
	if (val.isMember("position") && val["position"].isMember("x") && val["position"].isMember("y")) {
		pos.x = static_cast<float>(val["position"]["x"].asDouble());
		pos.y = static_cast<float>(val["position"]["y"].asDouble());
	}
	m_position = pos;

	// rotate
	float angle = 0;
	if (val.isMember("angle")) {
		angle = static_cast<float>(val["angle"].asDouble());
	}
	m_angle = angle;
}

void SpriteIO::StoreGeometry(Json::Value& val)
{
	if (!m_compress || m_position != sm::vec2(0, 0)) {
		val["position"]["x"] = m_position.x;
		val["position"]["y"] = m_position.y;
	}

	if (!m_compress || m_angle != 0) {
		val["angle"] = m_angle;
	}

	if (!m_compress || m_scale != sm::vec2(1, 1)) {
		val["x scale"] = m_scale.x;
		val["y scale"] = m_scale.y;
	}

	if (!m_compress || m_shear != sm::vec2(0, 0)) {
		val["x shear"] = m_shear.x;
		val["y shear"] = m_shear.y;
	}

	if (!m_compress || m_offset != sm::vec2(0, 0)) {
		val["x offset"] = m_offset.x;
		val["y offset"] = m_offset.y;
	}
}

/************************************************************************/
/* render                                                               */
/************************************************************************/

void SpriteIO::LoadRender(s2::Sprite* spr)
{
	LoadColor(spr);
	LoadShader(spr);
	LoadCamera(spr);	
}

void SpriteIO::StoreRender(const s2::Sprite* spr)
{
	StoreColor(spr->GetColor());
	StoreShader(spr->GetShader());
	StoreCamera(spr->GetCamera());	
}

void SpriteIO::LoadRender(const Json::Value& val)
{
	LoadColor(val);
	LoadShader(val);
	LoadCamera(val);	
}

void SpriteIO::StoreRender(Json::Value& val)
{
	StoreColor(val);
	StoreShader(val);
	StoreCamera(val);	
}

/************************************************************************/
/* info                                                                 */
/************************************************************************/

void SpriteIO::LoadInfo(s2::Sprite* spr)
{
	spr->SetName(m_name);
}

void SpriteIO::StoreInfo(const s2::Sprite* spr)
{
	m_name = spr->GetName();
}

void SpriteIO::LoadInfo(const Json::Value& val)
{
	if (val.isMember("name")) {
		m_name = val["name"].asString();
	} else {
		m_name = "";
	}
}

void SpriteIO::StoreInfo(Json::Value& val)
{
	if (!m_compress || !m_name.empty()) {
		val["name"] = m_name;
	}
}

/************************************************************************/
/* edit                                                                 */
/************************************************************************/

void SpriteIO::LoadEdit(s2::Sprite* spr)
{
	spr->SetVisible(m_visible);
	spr->SetEditable(m_editable);
}

void SpriteIO::StoreEdit(const s2::Sprite* spr)
{
	m_visible = spr->IsVisible();
	m_editable = spr->IsEditable();
}

void SpriteIO::LoadEdit(const Json::Value& val)
{
	if (val.isMember("visible")) {
		m_visible = val["visible"].asBool();
	} else {
		m_visible = true;
	}
	if (val.isMember("editable")) {
		m_editable = val["editable"].asBool();
	} else {
		m_editable = true;
	}
}

void SpriteIO::StoreEdit(Json::Value& val)
{
	if (!m_compress || !m_visible) {
		val["visible"] = m_visible;
	}
	if (!m_compress || !m_editable) {
		val["editable"] = m_editable;
	}
}

/************************************************************************/
/* private                                                              */
/************************************************************************/

void SpriteIO::LoadColor(s2::Sprite* spr)
{
	spr->SetColor(m_col);
}

void SpriteIO::StoreColor(const s2::RenderColor& color)
{
	m_col = color;
}

void SpriteIO::LoadColor(const Json::Value& val)
{
	m_col.mul = s2::Color(0xffffffff);
	if (val.isMember("multi color")) {
		std::string str = val["multi color"].asString();
		if (!str.empty()) {
			m_col.mul = str2color(str, BGRA);
		}
	}

	m_col.add = s2::Color(0);
	if (val.isMember("add color")) {
		std::string str = val["add color"].asString();
		if (!str.empty()) {
			m_col.add = str2color(str, ABGR);
		}
	}

	m_col.rmap = s2::Color(255, 0, 0, 0);
	if (val.isMember("r trans")) {
		std::string str = val["r trans"].asString();
		if (!str.empty()) {
			m_col.rmap = str2color(str, RGBA);
		}
	}

	m_col.gmap = s2::Color(0, 255, 0, 0);
	if (val.isMember("g trans")) {
		std::string str = val["g trans"].asString();
		if (!str.empty()) {
			m_col.gmap = str2color(str, RGBA);
		}
	}

	m_col.bmap = s2::Color(0, 0, 255, 0);
	if (val.isMember("b trans")) {
		std::string str = val["b trans"].asString();
		if (!str.empty()) {
			m_col.bmap = str2color(str, RGBA);
		}
	}
}

void SpriteIO::StoreColor(Json::Value& val)
{
	if (!m_compress || m_col.mul != s2::Color(0xffffffff)) {
		val["multi color"]	= color2str(m_col.mul, BGRA);
	}
	if (!m_compress || m_col.add != s2::Color(0)) {
		val["add color"]	= color2str(m_col.add, ABGR);
	}

	if (!m_compress || m_col.rmap.r != 255 || m_col.rmap.g != 0 || m_col.rmap.b != 0) {
		val["r trans"]		= color2str(m_col.rmap, RGBA);
	}
	if (!m_compress || m_col.gmap.r != 0 || m_col.gmap.g != 255 || m_col.gmap.b != 0) {
		val["g trans"]		= color2str(m_col.gmap, RGBA);
	}
	if (!m_compress || m_col.bmap.r != 0 || m_col.bmap.g != 0 || m_col.bmap.b != 255) {
		val["b trans"]		= color2str(m_col.bmap, RGBA);
	}
}

void SpriteIO::LoadShader(s2::Sprite* spr)
{
	s2::RenderShader rs = spr->GetShader();
	rs.SetBlend(m_blend);
	rs.SetFastBlend(m_fast_blend);
	rs.SetFilter(m_filter);
	spr->SetShader(rs);
}

void SpriteIO::StoreShader(const s2::RenderShader& shader)
{
	m_blend      = shader.GetBlend();
	m_fast_blend = shader.GetFastBlend();
	if (shader.GetFilter()) {
		m_filter = shader.GetFilter()->Clone();
	} else {
		m_filter = NULL;
	}
}

void SpriteIO::LoadShader(const Json::Value& val)
{
	m_blend = s2::BM_NULL;
	m_fast_blend = s2::FBM_NULL;
	if (m_filter) {
		delete m_filter;
		m_filter = NULL;
	}

	if (!m_render_open) {
		return;
	}

	if (val.isMember("blend")) {
		std::string disc = val["blend"].asString();
		m_blend = BlendModes::Instance()->Name2Mode(disc);
	}

	if (val.isMember("fast_blend")) {
		std::string disc = val["fast_blend"].asString();
		m_fast_blend = FastBlendModes::Instance()->Name2Mode(disc);
	}

	if (val.isMember("filter")) 
	{
		if (val["filter"].isString()) 
		{
			std::string disc = val["filter"].asString();
			s2::FilterMode mode = FilterModes::Instance()->Name2Mode(disc);
			m_filter = s2::FilterFactory::Instance()->Create(mode);
		} 
		else 
		{
			const Json::Value& fval = val["filter"];
			std::string disc = fval["mode"].asString();
			s2::FilterMode mode = FilterModes::Instance()->Name2Mode(disc);
			m_filter = s2::FilterFactory::Instance()->Create(mode);
			switch (mode)
			{
			case s2::FM_EDGE_DETECTION:
				{
					float blend = (float)(fval["blend"].asDouble());
					s2::RFEdgeDetection* filter = static_cast<s2::RFEdgeDetection*>(m_filter);
					filter->SetBlend(blend);
				}
				break;
			case s2::FM_GAUSSIAN_BLUR:
				{
					int iterations = fval["iterations"].asInt();
					s2::RFGaussianBlur* filter = static_cast<s2::RFGaussianBlur*>(m_filter);
					filter->SetIterations(iterations);
				}
				break;
			case s2::FM_OUTER_GLOW:
				{
					int iterations = fval["iterations"].asInt();
					s2::RFOuterGlow* filter = static_cast<s2::RFOuterGlow*>(m_filter);
					filter->SetIterations(iterations);
				}
				break;
			}
		}
	}
	else
	{
		m_filter = s2::FilterFactory::Instance()->Create(s2::FM_NULL);
	}
}

void SpriteIO::StoreShader(Json::Value& val)
{
	if (!m_render_open) {
		return;
	}

	if (m_blend != s2::BM_NULL) {
		val["blend"] = BlendModes::Instance()->ModeToName(m_blend);
	}

	if (m_fast_blend != s2::FBM_NULL) {
		val["fast_blend"] = FastBlendModes::Instance()->Mode2Name(m_fast_blend);
	}

	s2::FilterMode mode = s2::FM_NULL;
	if (m_filter) {
		mode = m_filter->GetMode();
	}
	if (mode != s2::FM_NULL)
	{
		Json::Value fval;
		fval["mode"] = FilterModes::Instance()->Mode2Name(mode);
		switch (mode)
		{
		case s2::FM_EDGE_DETECTION:
			{
				s2::RFEdgeDetection* filter = static_cast<s2::RFEdgeDetection*>(m_filter);
				fval["blend"] = filter->GetBlend();
			}
			break;
		case s2::FM_GAUSSIAN_BLUR:
			{
				s2::RFGaussianBlur* filter = static_cast<s2::RFGaussianBlur*>(m_filter);
				fval["iterations"] = filter->GetIterations();
			}
			break;
		case s2::FM_OUTER_GLOW:
			{
				s2::RFOuterGlow* filter = static_cast<s2::RFOuterGlow*>(m_filter);
				fval["iterations"] = filter->GetIterations();
			}
			break;
		}
		val["filter"] = fval;
	}
}

void SpriteIO::LoadCamera(s2::Sprite* spr)
{
	s2::RenderCamera rc = spr->GetCamera();
	rc.mode = m_camera;
	spr->SetCamera(rc);
}

void SpriteIO::StoreCamera(const s2::RenderCamera& rc)
{
	m_camera = rc.mode;
}

void SpriteIO::LoadCamera(const Json::Value& val)
{
	if (val.isMember("camera")) {
		std::string disc = val["camera"].asString();
		m_camera = CameraModes::Instance()->Name2Mode(disc);
	} else {
		m_camera = s2::CM_ORTHO;
	}
}

void SpriteIO::StoreCamera(Json::Value& val)
{
	if (m_camera != s2::CM_ORTHO) {
		val["camera"] = CameraModes::Instance()->Mode2Name(m_camera);
	}
}

}