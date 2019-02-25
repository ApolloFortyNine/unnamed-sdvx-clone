#include "stdafx.h"
#include "MultiplayerScreen.hpp"
#include "LobbyScreen.hpp"
#include "Application.hpp"
#include "TransitionScreen.hpp"
#include "SettingsScreen.hpp"
#include <Shared/Profiling.hpp>
#include "Scoring.hpp"
#include "SongSelect.hpp"
#include <Audio/Audio.hpp>
#include "Track.hpp"
#include "Camera.hpp"
#include "Background.hpp"
#include "HealthGauge.hpp"
#include "Shared/Jobs.hpp"
#include "ScoreScreen.hpp"
#include "Shared/Enum.hpp"
#include "lua.hpp"
#include "Shared/LuaBindable.hpp"

class MultiplayerScreen_Impl : public MultiplayerScreen
{
private:
	lua_State* m_lua = nullptr;
	LuaBindable* m_luaBinds;

	void Back()
	{
		//g_gameWindow->OnAnyEvent.RemoveAll(this);
		
		//g_input.OnButtonReleased.RemoveAll(this);
		//g_input.Cleanup();
		//g_input.Init(*g_gameWindow);
		g_application->RemoveTickable(this);
	}

	int lBack(lua_State* L)
	{
		Back();
		return 0;
	}

	void CreateGame()
	{
		g_application->AddTickable(LobbyScreen::Create());
	}

	int lCreateGame(lua_State* L)
	{
		CreateGame();
		return 0;
	}

	void JoinGame()
	{
		//g_application->AddTickable(SettingsScreen::Create());
	}

	int lJoinGame(lua_State* L)
	{
		JoinGame();
		return 0;
	}

	void MousePressed(MouseButton button)
	{
		if (IsSuspended())
			return;
		lua_getglobal(m_lua, "mouse_pressed");
		lua_pushnumber(m_lua, (int32)button);
		if (lua_pcall(m_lua, 1, 1, 0) != 0)
		{
			Logf("Lua error on mouse_pressed: %s", Logger::Error, lua_tostring(m_lua, -1));
			g_gameWindow->ShowMessageBox("Lua Error on mouse_pressed", lua_tostring(m_lua, -1), 0);
			assert(false);
		}
	}

public:
	bool Init()
	{
		CheckedLoad(m_lua = g_application->LoadScript("multiplayer"));
		m_luaBinds = new LuaBindable(m_lua, "MultiplayerMenu");
		m_luaBinds->AddFunction("Back", this, &MultiplayerScreen_Impl::lBack);
		m_luaBinds->AddFunction("JoinGame", this, &MultiplayerScreen_Impl::lJoinGame);
		m_luaBinds->AddFunction("CreateGame", this, &MultiplayerScreen_Impl::lCreateGame);
		m_luaBinds->Push();
		lua_settop(m_lua, 0);
		g_gameWindow->OnMousePressed.Add(this, &MultiplayerScreen_Impl::MousePressed);
		return true;
	}
	~MultiplayerScreen_Impl()
	{
		g_gameWindow->OnMousePressed.RemoveAll(this);
		if (m_lua)
			g_application->DisposeLua(m_lua);
	}

	virtual void Render(float deltaTime)
	{
		if (IsSuspended())
			return;

		lua_getglobal(m_lua, "render");
		lua_pushnumber(m_lua, deltaTime);
		if (lua_pcall(m_lua, 1, 0, 0) != 0)
		{
			Logf("Lua error: %s", Logger::Error, lua_tostring(m_lua, -1));
			g_gameWindow->ShowMessageBox("Lua Error", lua_tostring(m_lua, -1), 0);
			assert(false);
		}
	}
	virtual void OnSuspend()
	{
	}
	virtual void OnRestore()
	{
		g_gameWindow->SetCursorVisible(true);
		g_application->ReloadSkin();
		g_application->ReloadScript("multiplayer", m_lua);
		m_luaBinds->Push();
		lua_settop(m_lua, 0);
		g_application->DiscordPresenceMenu("Multiplayer Screen");
	}
};

MultiplayerScreen* MultiplayerScreen::Create()
{
	MultiplayerScreen_Impl* impl = new MultiplayerScreen_Impl();
	return impl;
}