 #include "gum/lua_wrap_audio.h"
#include "gum/Audio.h"

#include <uniaudio/AudioContext.h>
#include <uniaudio/Source.h>

namespace gum
{

static int
lenable(lua_State* L)
{
	bool enable = lua_toboolean(L, 1);
	try {
		Audio::Instance()->SetEnable(enable);
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lcreate(lua_State* L) 
{
	const char* filepath = luaL_checkstring(L, 1);
	bool stream = lua_toboolean(L, 2);
	void* source = nullptr;
	try {
		Audio* audio = Audio::Instance();
		if (audio->IsEnable()) {
			auto s = audio->GetContext()->CreateSource(filepath, stream);
			if (s) {
				source = new std::shared_ptr<ua::Source>(s);
			}
		} else {
			return 0;
		}
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
lrelease(lua_State* L) 
{
	void* source = lua_touserdata(L, 1);	
	try {
		delete static_cast<std::shared_ptr<ua::Source>*>(source);
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lstop_all(lua_State* L) 
{
	try {
		Audio* audio = Audio::Instance();
		if (audio->IsEnable()) {
			audio->GetContext()->Stop();
		}
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lpause_all(lua_State* L) 
{
	try {
		Audio* audio = Audio::Instance();
		if (audio->IsEnable()) {
			audio->GetContext()->Pause();
		}
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lresume_all(lua_State* L) 
{
	try {
		Audio* audio = Audio::Instance();
		if (audio->IsEnable()) {
			audio->GetContext()->Resume();
		}
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lrewind_all(lua_State* L) 
{
	try {
		Audio* audio = Audio::Instance();
		if (audio->IsEnable()) {
			audio->GetContext()->Rewind();
		}
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lplay(lua_State* L) 
{
	void* source = lua_touserdata(L, 1);
	try {
		Audio* audio = Audio::Instance();
		if (audio->IsEnable()) {
			ua::Source* s = static_cast<ua::Source*>(source);
			s->Play();
		}
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lstop(lua_State* L) 
{
	void* source = lua_touserdata(L, 1);
	try {
		Audio* audio = Audio::Instance();
		if (audio->IsEnable()) {
			ua::Source* s = static_cast<ua::Source*>(source);
			s->Stop();
		}
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

static int
lset_volume(lua_State* L)
{
	float volume = static_cast<float>(luaL_optnumber(L, 1, 1.0f));
	try {
		auto ctx = Audio::Instance()->GetContext();
		if (ctx) {
			ctx->SetVolume(volume);
		}
	} catch (std::exception& e) {
		luaL_error(L, e.what());
	}
	return 0;
}

extern "C" int luaopen_gum_audio(lua_State* L)
{
	luaL_Reg l[] = 
	{
		{ "enable", lenable },

		{ "create", lcreate },
		{ "release", lrelease },		

		{ "stop_all", lstop_all },
		{ "pause_all", lpause_all },
		{ "resume_all", lresume_all },
		{ "rewind_all", lrewind_all },

		{ "play", lplay },
		{ "stop", lstop },

		{ "set_volume", lset_volume },

		{ nullptr, nullptr },		
	};
	luaL_newlib(L, l);
	return 1;
}

}