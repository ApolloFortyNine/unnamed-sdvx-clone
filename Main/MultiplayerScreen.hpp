#pragma once
#include "ApplicationTickable.hpp"

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