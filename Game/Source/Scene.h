#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "Enemy.h"
#include "Life.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//Pantallazo negro y luego lo quita
	bool FadeToBlack(float ms);

	bool LoadState(pugi::xml_node& data);

	//Save actual scene
	bool SaveState(pugi::xml_node& data);

public:

	//L02: DONE 3: Declare a Player attribute 
	Player* player;

	//A que distancia del borde se empieza a mover la camara con el player
	int cameraMargin = 16; //Porque 16? Porque funciona bien, simplemente

	//Limites donde la camara dejara de moverse por salirse del mapa
	int maxCameraPosLeft;
	int maxCameraPosRigth;
	int maxCameraPosUp;
	int maxCameraPosDown;

	enum Scenes
	{
		INTRO,
		GAMEMAP,
		WIN,
		LOSE
	};
	int actualScene;
	bool pause = false;

	//Remaining time?
	int maxTime = 100;
	int timeLeft;
	int lastTime=0;
	Timer countDown;
	SString timeInText;
private:
	SDL_Texture* img;
	Timer fadingFrames;

};

#endif // __SCENE_H__