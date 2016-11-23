#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _gum_wrap_c_h_
#define _gum_wrap_c_h_

#include <stdint.h>

void gum_update(float dt);

void gum_create_pkg(const char* filepath, const char* name, int id);
void* gum_create_spr(const char* pkg, const char* spr);

void gum_draw_spr(const void* spr);
void gum_update_spr(void* spr);

#endif // _gum_wrap_c_h_

#ifdef __cplusplus
}
#endif