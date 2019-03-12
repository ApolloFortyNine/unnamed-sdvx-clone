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
	void m_PushStringToTable(const char* name, const char* data);
	void m_PushFloatToTable(const char* name, float data);
	void m_PushIntToTable(const char* name, int data);
	void m_SetLuaMaps();
	void m_mousePressed(int32 button);
};