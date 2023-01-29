#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "Module.h"
#include "Animation.h"

#include "Textures.h"
#include "Point.h"

#include "GuiButton.h"
#include "GuiToggle.h"

#include "PugiXml\src\pugixml.hpp"

class Settings : public Module
{
public:

	Settings();

	// Destructor
	virtual ~Settings();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);
	bool Start();
	// Called each loop iteration
	bool Update();

	void Draw();

	// Called before quitting
	bool CleanUp();

	// Define multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

public:
	bool colidersCreated = false;

	pugi::xml_node parameters;
	SDL_Texture* texture;
	const char* texturePath;

private:

	SString mapFileName;
	SString mapFolder;
	bool mapLoaded;

	GuiButton* button1_1;
	GuiButton* button1_2;
	GuiButton* button1_3;
	GuiToggle* button2_1;
	GuiToggle* button2_2;
};



#endif // __SETTINGS_H__
