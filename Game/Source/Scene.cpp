#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Pathfinding.h"

//Diferent Scenes
#include "Intro.h"
#include "Map.h"
#include "Victoria.h"
#include "Muerte.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	for (pugi::xml_node itemNode = config.child("enemy"); itemNode; itemNode = itemNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
		enemy->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");
	actualScene = Scenes::INTRO;
	

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	
	// L03: DONE: Load map
	bool retLoad=app->map->Load();

	// L12 Create walkability map
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	//SET CAMERA MAXIMS
	maxCameraPosLeft = -1; //-1 para evitar una linea de pixels negra
	maxCameraPosRigth = -(app->map->mapData.width* app->map->mapData.tileWidth)+app->render->camera.w; //PosRigth es negativo ya que el mapa a moverse la camara se mueve a la izquierda
	maxCameraPosUp = 1;
	maxCameraPosDown = -(app->map->mapData.height * app->map->mapData.tileHeight) + app->render->camera.h;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//Pausar juego
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		pause = !pause;


	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (maxCameraPosUp >= app->render->camera.y)
		{
			app->render->camera.y += 1;
		}
	}
		

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (maxCameraPosDown <= app->render->camera.y)
		{
			app->render->camera.y -= 1;
		}
	}
		

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT )
	{
		if (maxCameraPosLeft >= app->render->camera.x)
		{
		app->render->camera.x += 1;
		}
		
	}

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (maxCameraPosRigth <= app->render->camera.x)
		{
		app->render->camera.x -= 1; 
		}
	}

	//Draw actual scene
	switch (actualScene)
	{
	case Scene::INTRO:
		//Draw intro
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->intro->Draw();

		//Cambiar al mapa de juego
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			app->render->camera.y = -768;
			actualScene = Scenes::GAMEMAP;
			countDown.Start();
			timeLeft = maxTime;
		}
		break;
	case Scene::GAMEMAP:
		// Draw map
		app->map->Draw();
		
		//Cuenta atras
		if (pause == false) 
		{
		timeLeft = maxTime - countDown.ReadSec()-lastTime; 
		}
		else
		{
			lastTime = maxTime - timeLeft;
			countDown.Start();
		}
		
		//Print time
		timeInText.Create("TIME %d", timeLeft);
		SDL_Color colorLetras;
		colorLetras.r = 155+(100-timeLeft)/2;
		colorLetras.g = 155- (100 - timeLeft)/3;
		colorLetras.b = 155- (100 - timeLeft)/3;
		colorLetras.a = 255;
		if (timeLeft < 10) { app->render->DrawText(timeInText.GetString(), 900, 5, 60, 20, colorLetras); }
		else { app->render->DrawText(timeInText.GetString(), 900, 5, 70, 20, colorLetras); }
		if (timeLeft<0)
		{
			actualScene = Scenes::LOSE;
		}
		
		break;
	case Scene::WIN:
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->victory->Draw();
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			actualScene = Scenes::INTRO;
			//app->entityManager->CleanUp();
			app->entityManager->Reset();
		}
		break;
	case Scene::LOSE:
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		app->lose->Draw();
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			actualScene = Scenes::INTRO;
			//app->entityManager->CleanUp();
			app->entityManager->Reset();
		}
		break;
	default:
		break;
	}

	

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

bool Scene::LoadState(pugi::xml_node& data)
{
	
	actualScene = data.child("scene").attribute("actualScene").as_int();
	app->render->camera.x = data.child("camera").attribute("x").as_int();
	app->render->camera.x = data.child("camera").attribute("y").as_int();


	return true;
	
}

bool Scene::SaveState(pugi::xml_node& data)
{
	pugi::xml_node scene = data.append_child("scene");
	pugi::xml_node camara = data.append_child("camara");
	
	camara.append_attribute("x")=app->render->camera.x;
	camara.append_attribute("y")= app->render->camera.y;

	scene.append_attribute("actualScene") = actualScene;
	
	return true;
}