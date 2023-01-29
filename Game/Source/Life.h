#ifndef __LIFE_H__
#define __LIFE_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Life : public Entity
{
public:

	Life();
	virtual ~Life();

	bool Awake();

	bool Start();

	bool Reset();

	bool Update();

	bool CleanUp();

	bool LoadState(pugi::xml_node& data);

	bool SaveState(pugi::xml_node& data);

	
public:

	bool isPicked = false;
	bool renderable = true;

private:

	SDL_Texture* texture;
	const char* texturePath;

	//DONE 4: Add a physics to an item
	PhysBody* pbody;
};

#endif // __LIFE_H__