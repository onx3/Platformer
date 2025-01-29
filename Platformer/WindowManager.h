#pragma once
#include "AstroidsPrivate.h"

static float gsPixelCount = 16;

class WindowManager
{
public:
	WindowManager();
	~WindowManager();

	void PollEvents();

	sf::RenderWindow * GetWindow();
	sf::Event GetEvent() const;

private:
	sf::RenderWindow * mpWindow;
	sf::Event mEvent;
};