#include "gum/lua_wrap_audio.h"
#include "gum/GUM_Audio.h"

#include <uniaudio/AudioContext.h>
#include <uniaudio/Source.h>

namespace gum
{

static int
lcreate(lua_State* L) {
	const char* filepath = luaL_checkstring(L, 1);
	bool stream = lua_toboolean(L, 2);
	void* source = NULL;
	try {
		source = Audio::Instance()->GetContext()->CreateSource(filepath, stream);
	} catch (std::exception& e) {
		luaL_error(L, e.what());
		return 0;
	}
	if (!source) {
		luaL_error(L, "could not create source.");
		return 0;
	} else {
		lua_pushlightuserdata(L, source);
		return 1;
	}
}

static int
lplay(lua_State* L) {
	void* source = lua_touserdata(L, 1);
	try {
		ua::Source* s = static_cast<ua::Source*>(source);
		s->Play();
	} catch (std::exception& e) {
		luaL_error(L, "could not play source.");
	}
	return 0;
}

static int
lstop(lua_State* L) {
	void* source = lua_touserdata(L, 1);
	try {
		ua::Source* s = static_cast<ua::Source*>(source);
		s->Stop();
	} catch (std::exception& e) {
		luaL_error(L, "could not stop source.");
	}
	return 0;
}

extern "C" int luaopen_gum_audio(lua_State* L)
{
	luaL_Reg l[] = 
	{
		{ "create", lcreate },

		{ "play", lplay },
		{ "stop", lstop },

		{ NULL, NULL },		
	};
	luaL_newlib(L, l);
	return 1;
}

}