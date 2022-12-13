#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Reset();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	bool fly = false;
	iPoint tilePos;
	iPoint tileObjective;

private:

	SDL_Texture* texture;
	const char* texturePath;

	enum state
	{
		IDLE,
		CHASE,
		DEAD
	};

	int actualState = IDLE;
	//IDLE MOVEMENT
	int direction = 1;
	float speed = 0.8;
	
	//Following player
	uint pathStep = 1;
	iPoint posNextStep;

	//DONE 4: Add a physics to an item
	PhysBody* pbody;

	bool ShowVectors=false;
};

#endif // __ENEMY_H__