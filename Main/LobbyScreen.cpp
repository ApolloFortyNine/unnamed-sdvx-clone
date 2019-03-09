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
#include <iostream>
#include "Beatmap/MapDatabase.hpp"
#include "SongFilter.hpp"

class LobbyScreen_Impl : public LobbyScreen
{
private:
	lua_State* m_lua = nullptr;
	LuaBindable* m_luaBinds;
	MapDatabase* m_mapDB;
	bool m_suspended = false;

	void Quit()
	{
		g_application->RemoveTickable(this);
	}

	int lQuit(lua_State* L)
	{
		Quit();
		return 0;
	}

	//void SetMapDB(MapDatabase* db)
	//{
	//	m_mapDB = db;
	//	for (String p : Path::GetSubDirs(g_gameConfig.GetString(GameConfigKeys::SongFolder)))
	//	{
	//		SongFilter* filter = new FolderFilter(p, m_mapDB);
	//		if (filter->GetFiltered(Map<int32, SongSelectIndex>()).size() > 0)
	//			AddFilter(filter, FilterType::Folder);
	//	}
	//	m_SetLuaTable();
	//}

	//void AddFilter(SongFilter* filter, FilterType type)
	//{
	//	if (type == FilterType::Level)
	//		m_levelFilters.Add(filter);
	//	else
	//		m_folderFilters.Add(filter);
	//}

	void StartGame()
	{
		if (g_application->DifficultyIndex_selected)
		{
			Game* game = Game::Create(*g_application->DifficultyIndex_selected, g_application->GameFlags_selected);
			if (!game)
			{
				Logf("Failed to start game", Logger::Error);
				return;
			}
			game->GetScoring().autoplay = false; // look at this more, i changed this
			m_suspended = true;

			// Transition to game
			TransitionScreen* transistion = TransitionScreen::Create(game);
			g_application->AddTickable(transistion);
		}
	}

	int lStartGame(lua_State* L)
	{
		StartGame();
		return 0;
	}

	void SongSelectMulti()
	{
		g_application->AddTickable(SongSelect::Create(PlayingMode::Multiplayer));
		//lua_getglobal(m_lua, "set_map_select");
		//lua_pushinteger(m_lua, g_application->MapIndex_selected);
		//if (lua_pcall(m_lua, 1, 0, 0) != 0)
		//{
		//	Logf("Lua error on set_index: %s", Logger::Error, lua_tostring(m_lua, -1));
		//	g_gameWindow->ShowMessageBox("Lua Error on set_index", lua_tostring(m_lua, -1), 0);
		//	assert(false);
		//}
	}

	int lSongSelectMulti(lua_State* L)
	{
		SongSelectMulti();
		return 0;
	}

	//void m_SetLuaMapIndex()
	//{
	//	lua_getglobal(m_lua, "set_index");
	//	lua_pushinteger(m_lua, m_currentlySelectedLuaMapIndex + 1);
	//	if (lua_pcall(m_lua, 1, 0, 0) != 0)
	//	{
	//		Logf("Lua error on set_index: %s", Logger::Error, lua_tostring(m_lua, -1));
	//		g_gameWindow->ShowMessageBox("Lua Error on set_index", lua_tostring(m_lua, -1), 0);
	//		assert(false);
	//	}
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
		m_luaBinds->AddFunction("SongSelectMulti", this, &LobbyScreen_Impl::lSongSelectMulti);
		m_luaBinds->AddFunction("StartGame", this, &LobbyScreen_Impl::lStartGame);
		m_luaBinds->Push();
		lua_settop(m_lua, 0);
		g_gameWindow->OnMousePressed.Add(this, &LobbyScreen_Impl::MousePressed);

		// borrowed
		//m_mapDatabase.AddSearchPath(g_gameConfig.GetString(GameConfigKeys::SongFolder));

		//m_mapDatabase.OnMapsAdded.Add(m_selectionWheel.GetData(), &SelectionWheel::OnMapsAdded);
		//m_mapDatabase.OnMapsUpdated.Add(m_selectionWheel.GetData(), &SelectionWheel::OnMapsUpdated);
		//m_mapDatabase.OnMapsCleared.Add(m_selectionWheel.GetData(), &SelectionWheel::OnMapsCleared);
		//m_mapDatabase.OnSearchStatusUpdated.Add(m_selectionWheel.GetData(), &SelectionWheel::OnSearchStatusUpdated);
		//m_mapDatabase.StartSearching();

		// end

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
		m_suspended = true;
	}
	virtual void OnRestore()
	{
		m_suspended = false;
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