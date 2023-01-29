#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Animation.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Timer.h"

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
	int lives = 4;
	bool inmune = false;
	Timer inmunityFrames;
	int blinkingFrames = 1;

	int coinsPicked = 0;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	SDL_Texture* Wtexture;
	SDL_Texture* fullHP;
	SDL_Texture* voidHP;
	Animation* currentAnimation = nullptr;
	Animation* currentAnimation2 = nullptr;
	Animation Nulle, Run, Jump, JumpIdle, Weapon;
	SDL_RendererFlip Invert;
	const char* texturePath;
	const char* FxJump;
	const char* FxWin;
	const char* FxLose;

	const char* pathFullHP;
	const char* pathNoneHP;
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
	float direction = 1;
	int dirToGo;
	Timer swordDuration;
	bool swordExist = false;
	float posFlameX;
	float posFlameY;
	

	//Debugging
	bool ShowVectors=false;
	bool GodMode = false;
	
};

#endif // __PLAYER_H__