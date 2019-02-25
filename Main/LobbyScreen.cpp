#include "stdafx.h"
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

class LobbyScreen_Impl : public LobbyScreen
{
private:
	lua_State* m_lua = nullptr;
	LuaBindable* m_luaBinds;

	void Quit()
	{
		g_application->RemoveTickable(this);
	}

	int lQuit(lua_State* L)
	{
		Quit();
		return 0;
	}

	//void CreateGame()
	//{
	//	g_application->AddTickable(SongSelect::Create());
	//}

	//int lCreateGame(lua_State* L)
	//{
	//	CreateGame();
	//	return 0;
	//}

	//void JoinGame()
	//{
	//	g_application->AddTickable(SettingsScreen::Create());
	//}

	//int lJoinGame(lua_State* L)
	//{
	//	JoinGame();
	//	return 0;
	//}

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
		CheckedLoad(m_lua = g_application->LoadScript("lobby"));
		m_luaBinds = new LuaBindable(m_lua, "LobbyButtons");
		m_luaBinds->AddFunction("Quit", this, &LobbyScreen_Impl::lQuit);
		m_luaBinds->Push();
		lua_settop(m_lua, 0);
		g_gameWindow->OnMousePressed.Add(this, &LobbyScreen_Impl::MousePressed);
		return true;
	}
	~LobbyScreen_Impl()
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
		g_application->ReloadScript("lobby", m_lua);
		m_luaBinds->Push();
		lua_settop(m_lua, 0);
		g_application->DiscordPresenceMenu("Lobby Screen");
	}
};

LobbyScreen* LobbyScreen::Create()
{
	LobbyScreen_Impl* impl = new LobbyScreen_Impl();
	return impl;
}