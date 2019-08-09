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

	void SongSelectEnded()
	{
		if (g_application->MapIndex_selected)
		{
			m_SetLuaMap();
			lua_getglobal(m_lua, "has_song_selected_true");
			if (lua_pcall(m_lua, 0, 0, 0) != 0)
			{
				Logf("Lua error on calling has_song_selected_true: %s", Logger::Error, lua_tostring(m_lua, -1));
			}
		}
	}

	void SongSelectMulti()
	{
		g_application->AddTickable(SongSelect::Create(PlayingMode::Multiplayer));
	}

	int lSongSelectMulti(lua_State* L)
	{
		SongSelectMulti();
		return 0;
	}

	void MousePressed(MouseButton button)
	{
		if (IsSuspended())
			return;

		lua_getglobal(m_lua, "random_func");
		lua_pushstring(m_lua, "i'm from c++!");
		if (lua_pcall(m_lua, 1, 0, 0) != 0)
		{
			Logf("Lua error on calling random_func: %s", Logger::Error, lua_tostring(m_lua, -1));
		}

		lua_getglobal(m_lua, "mouse_pressed");
		lua_pushnumber(m_lua, (int32)button);
		if (lua_pcall(m_lua, 1, 1, 0) != 0)
		{
			Logf("Lua error on mouse_pressed: %s", Logger::Error, lua_tostring(m_lua, -1));
			g_gameWindow->ShowMessageBox("Lua Error on mouse_pressed", lua_tostring(m_lua, -1), 0);
			assert(false);
		}
	}

	void m_PushStringToTable(const char* name, const char* data)
	{
		lua_pushstring(m_lua, name);
		lua_pushstring(m_lua, data);
		lua_settable(m_lua, -3);
	}
	void m_PushFloatToTable(const char* name, float data)
	{
		lua_pushstring(m_lua, name);
		lua_pushnumber(m_lua, data);
		lua_settable(m_lua, -3);
	}
	void m_PushIntToTable(const char* name, int data)
	{
		lua_pushstring(m_lua, name);
		lua_pushinteger(m_lua, data);
		lua_settable(m_lua, -3);
	}

	void m_SetLuaMap()
	{
		//int songIndex = 0;
		/*for (auto& song : m_SourceCollection())
		{
			lua_pushinteger(m_lua, ++songIndex);
			lua_newtable(m_lua);
			m_PushStringToTable("title", song.second.GetDifficulties()[0]->settings.title.c_str());
			m_PushStringToTable("artist", song.second.GetDifficulties()[0]->settings.artist.c_str());
			m_PushStringToTable("bpm", song.second.GetDifficulties()[0]->settings.bpm.c_str());
			m_PushIntToTable("id", song.second.GetMap()->id);
			m_PushStringToTable("path", song.second.GetMap()->path.c_str());
			int diffIndex = 0;
			lua_pushstring(m_lua, "difficulties");
			lua_newtable(m_lua);
			for (auto& diff : song.second.GetDifficulties())
			{
				lua_pushinteger(m_lua, ++diffIndex);
				lua_newtable(m_lua);
				auto settings = diff->settings;
				m_PushStringToTable("jacketPath", Path::Normalize(song.second.GetMap()->path + "/" + settings.jacketPath).c_str());
				m_PushIntToTable("level", settings.level);
				m_PushIntToTable("difficulty", settings.difficulty);
				m_PushIntToTable("id", diff->id);
				m_PushStringToTable("effector", settings.effector.c_str());
				m_PushIntToTable("topBadge", Scoring::CalculateBestBadge(diff->scores));
				lua_pushstring(m_lua, "scores");
				lua_newtable(m_lua);
				int scoreIndex = 0;
				for (auto& score : diff->scores)
				{
					lua_pushinteger(m_lua, ++scoreIndex);
					lua_newtable(m_lua);
					m_PushFloatToTable("gauge", score->gauge);
					m_PushIntToTable("flags", score->gameflags);
					m_PushIntToTable("score", score->score);
					m_PushIntToTable("perfects", score->crit);
					m_PushIntToTable("goods", score->almost);
					m_PushIntToTable("misses", score->miss);
					m_PushIntToTable("timestamp", score->timestamp);
					m_PushIntToTable("badge", Scoring::CalculateBadge(*score));
					lua_settable(m_lua, -3);
				}
				lua_settable(m_lua, -3);
				lua_settable(m_lua, -3);
			}
			lua_settable(m_lua, -3);
			lua_settable(m_lua, -3);
		}*/

		// broke for
		//lua_pushinteger(m_lua, ++songIndex);
		//lua_newtable(m_lua);
		//m_PushStringToTable("title", g_application->DifficultyIndex_selected->settings.title.c_str());
		//m_PushStringToTable("artist", g_application->DifficultyIndex_selected->settings.artist.c_str());
		//m_PushStringToTable("bpm", g_application->DifficultyIndex_selected->settings.bpm.c_str());
		//m_PushIntToTable("id", g_application->MapIndex_selected->id);
		//m_PushStringToTable("path", g_application->MapIndex_selected->path.c_str());
		//int diffIndex = 0;
		//lua_pushstring(m_lua, "difficulties");
		//lua_newtable(m_lua);

		//// broke for
		//lua_pushinteger(m_lua, ++diffIndex);
		//lua_newtable(m_lua);
		//auto settings = g_application->DifficultyIndex_selected->settings;
		//m_PushStringToTable("jacketPath", Path::Normalize(g_application->MapIndex_selected->path + "/" + settings.jacketPath).c_str());
		//m_PushIntToTable("level", settings.level);
		//m_PushIntToTable("difficulty", settings.difficulty);
		//m_PushIntToTable("id", g_application->DifficultyIndex_selected->id);
		//m_PushStringToTable("effector", settings.effector.c_str());
		//m_PushIntToTable("topBadge", Scoring::CalculateBestBadge(g_application->DifficultyIndex_selected->scores));
		//lua_pushstring(m_lua, "scores");
		//lua_newtable(m_lua);
		//int scoreIndex = 0;
		//for (auto& score : g_application->DifficultyIndex_selected->scores)
		//{
		//	lua_pushinteger(m_lua, ++scoreIndex);
		//	lua_newtable(m_lua);
		//	m_PushFloatToTable("gauge", score->gauge);
		//	m_PushIntToTable("flags", score->gameflags);
		//	m_PushIntToTable("score", score->score);
		//	m_PushIntToTable("perfects", score->crit);
		//	m_PushIntToTable("goods", score->almost);
		//	m_PushIntToTable("misses", score->miss);
		//	m_PushIntToTable("timestamp", score->timestamp);
		//	m_PushIntToTable("badge", Scoring::CalculateBadge(*score));
		//	lua_settable(m_lua, -3);
		//}
		//lua_settable(m_lua, -3);
		//lua_settable(m_lua, -3);

		//lua_settable(m_lua, -3);
		//lua_settable(m_lua, -3);
		
		
		//lua_pushstring(m_lua, "selected_song");
		lua_newtable(m_lua);

		m_PushStringToTable("title", g_application->DifficultyIndex_selected->settings.title.c_str());
		m_PushStringToTable("artist", g_application->DifficultyIndex_selected->settings.artist.c_str());
		m_PushStringToTable("bpm", g_application->DifficultyIndex_selected->settings.bpm.c_str());
		m_PushIntToTable("id", g_application->MapIndex_selected->id);
		m_PushStringToTable("path", g_application->MapIndex_selected->path.c_str());



		auto settings = g_application->DifficultyIndex_selected->settings;
		m_PushStringToTable("diff_jacketPath", Path::Normalize(g_application->MapIndex_selected->path + "/" + settings.jacketPath).c_str());
		m_PushIntToTable("diff_level", settings.level);
		m_PushIntToTable("diff_difficulty", settings.difficulty);
		m_PushIntToTable("diff_id", g_application->DifficultyIndex_selected->id);
		m_PushStringToTable("diff_effector", settings.effector.c_str());
		m_PushIntToTable("diff_topBadge", Scoring::CalculateBestBadge(g_application->DifficultyIndex_selected->scores));

		//lua_settable(m_lua, -3);
		lua_setglobal(m_lua, "selected_song");
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
		SongSelectEnded();
	}
};

LobbyScreen* LobbyScreen::Create()
{
	LobbyScreen_Impl* impl = new LobbyScreen_Impl();
	return impl;
}