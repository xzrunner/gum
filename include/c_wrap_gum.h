#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _gum_wrap_c_h_
#define _gum_wrap_c_h_

#include <stdint.h>
#include <stdbool.h>

void* gum_get_render_context();

void  gum_gc();

int   gum_get_sym_count();
int   gum_get_img_count();

void  gum_update(float dt);

void  gum_store_snapshot(const char* filepath);
int   gum_compare_snapshot(const char* filepath);

bool  gum_pkg_exists(const char* name);
bool  gum_create_pkg(const char* name, int id, const char* spr_path, const char* tex_path);
int   gum_pkg_get_page_count(const char* name);
void  gum_pkg_set_page_filepath(const char* name, int page, const char* filepath);
void  gum_pkg_get_texture_count(int pkg_id, int* tex_count, int* lod_count);
void  gum_pkg_set_texture_filepath(int pkg_id, int tex, int lod, const char* filepath);
void* gum_create_spr(const char* pkg, const char* spr);
void* gum_create_spr_by_id(int id);
void* gum_create_spr_from_file(const char* filepath);

void  gum_init_gtxt(int cap_bitmap, int cap_layout);
void  gum_draw_text(const char* str, int x, int y, int w);

void  gum_debug_draw();

#endif // _gum_wrap_c_h_

#ifdef __cplusplus
}
#endif