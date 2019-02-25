#pragma once
#include "ApplicationTickable.hpp"

class LobbyScreen : public IApplicationTickable
{
protected:
	LobbyScreen() = default;
public:
	virtual ~LobbyScreen() = default;
	static LobbyScreen* Create();
private:
	void m_mousePressed(int32 button);
};