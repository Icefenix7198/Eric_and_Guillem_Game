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

	bool LoadState(pugi::xml_node& data);

	bool SaveState(pugi::xml_node& data);



public:

	bool fly = false;
	iPoint tilePos;
	iPoint tileObjective;
	
	enum state
	{
		IDLE,
		CHASE,
		DEAD
	};

private:

	SDL_Texture* texture;
	const char* texturePath;

	

	//Detection
	int alarmRange;
	int loseRange;

	int actualState = IDLE;
	//Movement
	int directionX = 1;
	int directionY = 1;
	float speed = 0.8;
	
	//Following player
	uint pathStep = 1;
	iPoint posNextStep;

	bool isDead = false;
	//DONE 4: Add a physics to an item
	PhysBody* pbody;

	bool ShowVectors=false;
};

#endif // __ENEMY_H__