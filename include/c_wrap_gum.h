#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _gum_wrap_c_h_
#define _gum_wrap_c_h_

#include <stdint.h>

void  gum_gc();

void  gum_update(float dt);

void  gum_create_pkg(const char* filepath, const char* name, int id);
void* gum_create_spr(const char* pkg, const char* spr);
void* gum_create_spr_by_id(int id);
void* gum_create_spr_from_file(const char* filepath);

void  gum_spr_draw(const void* spr, float x, float y, float angle, float sx, float sy);
void  gum_spr_update(void* spr);
void  gum_spr_release(void* spr);

void* gum_fetch_child(const void* spr, const char* name);

#endif // _gum_wrap_c_h_

#ifdef __cplusplus
}
#endif