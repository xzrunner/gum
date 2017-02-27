#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _gum_wrap_c_h_
#define _gum_wrap_c_h_

#include <stdint.h>
#include <stdbool.h>

void  gum_init();

void* gum_get_render_context();

void  gum_on_size(int w, int h);

void  gum_gc();

int   gum_get_sym_count();
int   gum_get_img_count();

void  gum_update(float dt);
void  gum_flush();

void  gum_store_snapshot(const char* filepath);
int   gum_compare_snapshot(const char* filepath);

// pkg
bool  gum_pkg_exists(const char* name);
bool  gum_create_pkg(const char* name, int id, const char* spr_path, const char* tex_path);
int   gum_pkg_get_page_count(const char* name);
void  gum_pkg_set_page_filepath(const char* name, int page, const char* filepath);
void  gum_pkg_get_texture_count(int pkg_id, int* tex_count, int* lod_count);
void  gum_pkg_set_texture_filepath(int pkg_id, int tex, int lod, const char* filepath);

// spr
void* gum_create_spr(const char* pkg, const char* spr);
void* gum_create_spr_by_id(int id);
void* gum_create_spr_from_file(const char* filepath);

void  gum_init_gtxt(int cap_bitmap, int cap_layout);
void  gum_draw_text(const char* str, int x, int y, int w);

// rt
void* gum_rt_fetch();
void  gum_rt_return(void* rt);
void  gum_rt_bind(void* rt);
void  gum_rt_unbind(void* rt);
void  gum_rt_draw(void* rt);
int   gum_rt_get_texid(void* rt);

// dtex
void  gum_dtex_c2_enable(void* spr, bool enable);
void  gum_dtex_c2_force_cached(void* spr, bool cache);
void  gum_dtex_c2_force_cached_set_dirty(void* spr, bool dirty);

void* gum_create_img(const char* filepath);
int   gum_get_img_texid(void* img);

void  gum_debug_draw();

#endif // _gum_wrap_c_h_

#ifdef __cplusplus
}
#endif