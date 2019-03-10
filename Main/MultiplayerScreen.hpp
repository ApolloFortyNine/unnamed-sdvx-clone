#pragma once
#include "ApplicationTickable.hpp"
#include "jansson.h"
#include <string>

class MultiplayerScreen : public IApplicationTickable
{
protected:
	MultiplayerScreen() = default;
public:
	virtual ~MultiplayerScreen() = default;
	static MultiplayerScreen* Create();
private:
	void m_mousePressed(int32 button);
};

class MultiplayerPlayer
{
public:
	std::string id;
	int score;
	std::string name;

	MultiplayerPlayer(std::string id, std::string name, int score) : id(id), name(name), score(score)
	{
	}

	std::string getJson()
	{
		json_t *root = json_object();

		json_object_set_new(root, "id", json_string(id.c_str()));
		json_object_set_new(root, "score", json_integer(score));
		json_object_set_new(root, "name", json_string(name.c_str()));

		std::string s = json_dumps(root, 0);

		return s;
	}
};