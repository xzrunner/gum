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
int   gum_get_img_count();

void  gum_update(float dt);

void  gum_store_snapshot(const char* filepath);
int   gum_compare_snapshot(const char* filepath);

bool  gum_create_pkg(const char* filepath, const char* name, int id);
void* gum_create_spr(const char* pkg, const char* spr);
void* gum_create_spr_by_id(int id);
void* gum_create_spr_from_file(const char* filepath);

#endif // _gum_wrap_c_h_

#ifdef __cplusplus
}
#endif