#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Animation.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	Animation* currentAnimation = nullptr;
	Animation Anim;
	SDL_RendererFlip Invert;
	const char* texturePath;

	// L07 TODO 5: Add physics to the player - declare a Physics body

	PhysBody* playerBody;

	int pickCoinFxId;

	
	//Movimiento vertical del player (solo positivo de momento)
	int jump=0;
	int CanJump=2;
	//Movimiento horizontal del player
	int movement=0;
	//Debugging
	bool ShowVectors=false;
	bool GodMode = false;

};

#endif // __PLAYER_H__