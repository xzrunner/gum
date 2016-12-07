#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _gum_wrap_c_h_
#define _gum_wrap_c_h_

#include <stdint.h>
#include <stdbool.h>

void  gum_gc();

int   gum_get_sym_count();
int   gum_get_spr_count();
int   gum_get_img_count();

void  gum_update(float dt);

void  gum_store_snapshot(const char* filepath);
int   gum_compare_snapshot(const char* filepath);

void  gum_create_pkg(const char* filepath, const char* name, int id);
void* gum_create_spr(const char* pkg, const char* spr);
void* gum_create_spr_by_id(int id);
void* gum_create_spr_from_file(const char* filepath);

void  gum_spr_draw(const void* spr, float x, float y, float angle, float sx, float sy);
void  gum_spr_update(void* spr);
void  gum_spr_release(void* spr);

void* gum_spr_fetch_child(const void* spr, const char* name);
void* gum_spr_fetch_child_by_index(const void* spr, int idx);
void  gum_spr_mount_child(void* parent, const char* name, void* child);

void  gum_spr_set_pos(void* spr, float x, float y);
void  gum_spr_set_angle(void* spr, float angle);
void  gum_spr_set_scale(void* spr, float sx, float sy);

void  gum_spr_get_pos(const void* spr, float* x, float* y);
void  gum_spr_get_angle(const void* spr, float* angle);
void  gum_spr_get_scale(const void* spr, float* sx, float* sy);

int   gum_spr_get_sym_id(void* spr);
const char* gum_spr_get_name(void* spr);
int   gum_spr_get_sym_type(void* spr);

bool  gum_spr_get_visible(void* spr);
void  gum_spr_set_visible(void* spr, bool visible);
bool  gum_spr_get_editable(void* spr);
void  gum_spr_set_editable(void* spr, bool editable);

void  gum_spr_set_frame(void* spr, int frame);
void  gum_spr_set_action(void* spr, const char* action);

int   gum_spr_get_frame(void* spr);
int   gum_spr_get_frame_count(void* spr);
int   gum_spr_get_component_count(void* spr);

uint32_t gum_spr_get_col_mul(void* spr);
uint32_t gum_spr_get_col_add(void* spr);
void  gum_spr_get_col_map(void* spr, uint32_t* rmap, uint32_t* gmap, uint32_t* bmap);

void  gum_spr_set_col_mul(void* spr, uint32_t rgba);
void  gum_spr_set_col_add(void* spr, uint32_t rgba);
void  gum_spr_set_col_map(void* spr, uint32_t rmap, uint32_t gmap, uint32_t bmap);
void  gum_spr_set_filter(void* spr, int mode);

const char* gum_spr_get_text(void* spr);
void  gum_spr_set_text(void* spr, const char* text);

void  gum_spr_get_aabb(const void* spr, float aabb[4]);
void  gum_spr_draw_aabb(const void* spr, float x, float y, float angle, float sx, float sy, const float mat[6]);
bool  gum_spr_point_test(const void* spr, float x, float y);
void* gum_spr_point_query(const void* spr, float x, float y, float mat[6]);

bool  gum_spr_has_action(const void* spr, const char* name);

bool  gum_spr_get_text_size(const void* spr, float* w, float* h);

bool  gum_spr_get_scissor(const void* spr, float* x, float* y, float* w, float* h);
bool  gum_spr_set_scissor(void* spr, float x, float y, float w, float h);

#endif // _gum_wrap_c_h_

#ifdef __cplusplus
}
#endif