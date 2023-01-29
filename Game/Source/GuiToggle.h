#ifndef __GUITOGGLE_H__
#define __GUITOGGLE_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiToggle : public GuiControl
{
public:

	GuiToggle(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiToggle();

	bool Update(float dt);
	bool Draw(Render* render);

	int mouseX, mouseY;
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;

	uint audioFxId;
};

#endif // __GUITOGGLE_H__