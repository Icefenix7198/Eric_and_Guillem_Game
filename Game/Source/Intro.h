#ifndef __INTRO_H__
#define __INTRO_H__

#include "Module.h"
#include "Animation.h"

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
	bool Update();

	void Draw();

	// Called before quitting
	bool CleanUp();

public:
	bool colidersCreated = false;

	pugi::xml_node parameters;
	SDL_Texture* texture;
	SDL_Texture* texture2;
	const char* texturePath;
	const char* texturePath2;
	int change;

private:

	SString mapFileName;
	SString mapFolder;
	bool mapLoaded;
};



#endif // __INTRO_H__
