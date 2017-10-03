#ifndef _GUM_LUA_WRAP_AUDIO_H_
#define _GUM_LUA_WRAP_AUDIO_H_

#include <cu/cu_macro.h>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

namespace gum
{

extern "C" int luaopen_gum_audio(lua_State* L);

}

#endif // _GUM_LUA_WRAP_AUDIO_H_