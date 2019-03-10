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
#include "Shared/easywsclient.hpp"
#include <thread>
#include <mutex>
#include <chrono>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <memory>

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif

using std::thread;
using std::mutex;

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
	thread websocket_thread;
	bool terminate = false;
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

		//Enhance this to take in server from config
		websocket_thread = thread(&MultiplayerScreen_Impl::websocket_loop, this);
		return true;
	}
	~MultiplayerScreen_Impl()
	{
		g_gameWindow->OnMousePressed.RemoveAll(this);
		if (m_lua)
			g_application->DisposeLua(m_lua);
		terminate = true;
		websocket_thread.join();
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

	void websocket_loop()
	{
		using easywsclient::WebSocket;
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) {
			printf("WSAStartup Failed.\n");
			//return 1;
		}
#endif
		//WebSocket::pointer ws = WebSocket::from_url("ws://107.173.67.49:4001/socket/websocket");
		//WebSocket::pointer ws = WebSocket::from_url("ws://localhost:4000/socket/websocket");
		std::unique_ptr<WebSocket> ws(WebSocket::from_url("ws://localhost:4000/socket/websocket"));
		//assert(ws);
		ws->send("{\"topic\": \"game:lobby\",\"event\" : \"phx_join\",\"payload\" : {},\"ref\" : 0}");
		ws->send("{\"topic\": \"game:lobby\",\"event\" : \"phx_join\",\"payload\" : {},\"ref\" : 0}");
		while (ws->getReadyState() != WebSocket::CLOSED) {
			WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
			ws->poll();
			ws->send("{\"topic\": \"game:lobby\",\"event\" : \"phx_join\",\"payload\" : {},\"ref\" : 0}");
//			ws->dispatch([wsp](const std::string & message) {
//				printf(">>> %s\n", message.c_str());
//				if (message == "world") { wsp->close(); }
//			});
			if (terminate)
			{
				break;
			}
		}
#ifdef _WIN32
		WSACleanup();
#endif
	}
};

MultiplayerScreen* MultiplayerScreen::Create()
{
	MultiplayerScreen_Impl* impl = new MultiplayerScreen_Impl();
	return impl;
}