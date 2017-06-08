#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _gum_wrap_c_h_
#define _gum_wrap_c_h_

#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif // __cplusplus

void  gum_init();

void* gum_get_render_context();

void  gum_on_size(int w, int h);
void  gum_get_screen_size(int* w, int* h);

void  gum_gc();

int   gum_get_sym_count();
int   gum_get_img_count();

void  gum_update(float dt);
void  gum_flush();

void  gum_store_snapshot(const char* filepath);
int   gum_compare_snapshot(const char* filepath);

void* gum_create_img(const char* filepath);
int   gum_get_img_texid(void* img);

void  gum_debug_draw();

void  gum_clear();

bool  gum_is_async_task_empty();

/************************************************************************/
/* simp id                                                              */
/************************************************************************/

void  gum_load_pkg_ids(const char* filepath);
int   gum_get_pkg_num();
void  gum_get_all_pkg_names(const char* names[]);
int   gum_query_pkg_id(const char* name);

/************************************************************************/
/* pkg                                                                  */
/************************************************************************/

bool  gum_pkg_exists(const char* name);
bool  gum_create_pkg(const char* name, int id, const char* spr_path, const char* tex_path);
void  gum_release_pkg(int pkg_id);
void  gum_pkg_set_release_tag();
void  gum_pkg_release_after_last_tag();
int   gum_pkg_get_page_count(const char* name);
void  gum_pkg_set_page_filepath(const char* name, int page, const char* filepath);
void  gum_pkg_get_texture_count(int pkg_id, int* tex_count, int* lod_count);
void  gum_pkg_set_texture_filepath(int pkg_id, int tex, int lod, const char* filepath);
char** gum_pkg_get_export_names(const char* name, int* count);

/************************************************************************/
/* symbol                                                               */
/************************************************************************/

void* gum_create_sym_model(const void* model);

void* gum_fetch_sym(uint32_t id);

/************************************************************************/
/* sprite                                                               */
/************************************************************************/

void* gum_create_spr(const char* pkg, const char* spr, bool flatten);
void* gum_create_spr_by_id(int id, bool flatten);
void* gum_create_spr_from_file(const char* filepath);

void* gum_fetch_actor_cached(const char* pkg, const char* spr, bool* is_new);
void  gum_return_actor_cached(void* actor);

/************************************************************************/
/* rt                                                                   */
/************************************************************************/

struct gum_region {
	float xmin, ymin;
	float xmax, ymax;
};

void* gum_rt_fetch();
void  gum_rt_return(void* rt);
void  gum_rt_bind(void* rt);
void  gum_rt_unbind(void* rt);
void  gum_rt_draw(void* rt, struct gum_region* src);
int   gum_rt_get_texid(void* rt);

/************************************************************************/
/* dtex                                                                 */
/************************************************************************/

void  gum_dtex_set_c2_max_edge(int max_edge);

/************************************************************************/
/* gtxt                                                                 */
/************************************************************************/

void  gum_gtxt_init(int cap_bitmap, int cap_layout);
void  gum_gtxt_clear();
void  gum_gtxt_add_font(const char* name, const char* path);
void  gum_gtxt_add_color(const char* name, unsigned int color);
void  gum_gtxt_add_user_font_char(const char* str, const char* pkg, const char* node);
void  gum_gtxt_print(const char* str, float x, float y, int font_size, uint32_t font_color);
void  gum_gtxt_size(const char* str, int font_size, float* w, float* h);

/************************************************************************/
/* stat                                                                 */
/************************************************************************/

void  gum_stat_enable_graph(bool enable);
void  gum_stat_enable_console(bool enable);
void  gum_stat_enable_file(bool enable);

bool  gum_stat_is_graph_enable();
bool  gum_stat_is_console_enable();
bool  gum_stat_is_file_enable();

void  gum_stat_enable_tag(bool enable);
bool  gum_stat_is_tag_enable();
void  gum_stat_print_tag(const char* type, const char* msg);

void  gum_stat_no_stat_begin();
void  gum_stat_no_stat_end();

void  gum_stat_fps_begin();
void  gum_stat_fps_end();
int   gum_stat_get_fps();

void  gum_stat_opt_enable(bool enable);

void  gum_stat_update();
void  gum_stat_print();
void  gum_stat_reset();
void  gum_stat_flush();

void  gum_stat_set_mem(float tot, float lua);

#endif // _gum_wrap_c_h_

#ifdef __cplusplus
}
#endif