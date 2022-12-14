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

	bool Reset();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	

public:
	//Movido para guardar
	PhysBody* playerBody;
	PhysBody* weapon;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	SDL_Texture* Wtexture;
	Animation* currentAnimation = nullptr;
	Animation* currentAnimation2 = nullptr;
	Animation Nulle, Run, Jump, JumpIdle, Weapon;
	SDL_RendererFlip Invert;
	const char* texturePath;
	const char* FxJump;
	const char* FxWin;
	const char* FxLose;
	// L07 TODO 5: Add physics to the player - declare a Physics body

	int pickCoinFxId;
	int DeadSound;
	int WinSound;
	int JumpSound;

	
	//Movimiento vertical del player (solo positivo de momento)
	int jump=0;
	int CanJump=2;
	//Movimiento horizontal del player
	int movement=0;

	//Llama
	float direction = 1;;
	int swordFrames = 0;
	bool swordExist = false;
	

	//Debugging
	bool ShowVectors=false;
	bool GodMode = false;
	
};

#endif // __PLAYER_H__