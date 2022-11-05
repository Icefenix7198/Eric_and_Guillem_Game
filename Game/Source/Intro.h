#ifndef __INTRO_H__
#define __INTRO_H__

#include "Module.h"
#include "Textures.h"
#include "Point.h"

#include "PugiXml\src\pugixml.hpp"

class Intro : public Module
{
public:

	Intro();

	// Destructor
	virtual ~Intro();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);
	bool Start();
	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

public:
	bool colidersCreated = false;

	pugi::xml_node parameters;
	SDL_Texture* texture;
	const char* texturePath;
	SDL_Rect* rect;
private:

	SString mapFileName;
	SString mapFolder;
	bool mapLoaded;
};



#endif // __INTRO_H__
