#include "SprTransLoader.h"

#include <sprite2/S2_Sprite.h>
#include <sprite2/RenderColor.h>
#include <sprite2/RenderShader.h>
#include <sprite2/RenderCamera.h>
#include <simp/NodeTrans.h>

namespace gum
{

void SprTransLoader::Load(s2::Sprite* spr, const simp::NodeTrans* trans)
{
	int idx = 0;
	if (trans->type & simp::NodeTrans::SHEAR_MASK) {
		float x = ToFloat(trans->data[idx++]),
			y = ToFloat(trans->data[idx++]);
		spr->SetShear(sm::vec2(x, y));
	}
	if (trans->type & simp::NodeTrans::SCALE_MASK) {
		float x = ToFloat(trans->data[idx++]),
			  y = ToFloat(trans->data[idx++]);
		spr->SetScale(sm::vec2(x, y));
	}
	if (trans->type & simp::NodeTrans::OFFSET_MASK) {
		float x = ToFloat(trans->data[idx++]),
			  y = ToFloat(trans->data[idx++]);
		spr->SetOffset(sm::vec2(x, y));
	}
	if (trans->type & simp::NodeTrans::POSITION_MASK) {
		float x = ToFloat(trans->data[idx++]),
			  y = ToFloat(trans->data[idx++]);
		spr->SetPosition(sm::vec2(x, y));
	}
	if (trans->type & simp::NodeTrans::ANGLE_MASK) {
		float angle = ToFloat(trans->data[idx++]);
		spr->SetAngle(angle);
	}

	s2::RenderColor rc;
	if (trans->type & simp::NodeTrans::COL_MUL_MASK) {
		rc.mul.FromRGBA(trans->data[idx++]);
	}
	if (trans->type & simp::NodeTrans::COL_ADD_MASK) {
		rc.add.FromRGBA(trans->data[idx++]);
	}
	if (trans->type & simp::NodeTrans::COL_R_MASK) {
		rc.rmap.FromRGBA(trans->data[idx++]);
	}
	if (trans->type & simp::NodeTrans::COL_G_MASK) {
		rc.gmap.FromRGBA(trans->data[idx++]);
	}
	if (trans->type & simp::NodeTrans::COL_B_MASK) {
		rc.bmap.FromRGBA(trans->data[idx++]);
	}
	spr->SetColor(rc);

	s2::RenderShader rs;
	if (trans->type & simp::NodeTrans::BLEND_MASK) {
		rs.SetBlend(s2::BlendMode(trans->data[idx++]));
	}
	if (trans->type & simp::NodeTrans::FAST_BLEND_MASK) {
		rs.SetFastBlend(s2::FastBlendMode(trans->data[idx++]));
	}
	if (trans->type & simp::NodeTrans::FILTER_MASK) {
		s2::FilterMode mode = s2::FilterMode(trans->data[idx++]);
		rs.SetFilter(mode);
	}
	spr->SetShader(rs);

	if (trans->type & simp::NodeTrans::CAMERA_MASK) {
		s2::RenderCamera rc;
		rc.mode = s2::CameraMode(trans->data[idx++]);
		spr->SetCamera(rc);
	}

	if (trans->name) {
		spr->SetName(trans->name);
	}
}

float SprTransLoader::ToFloat(int i)
{
	return i / 1024.0f;
}

}