#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"

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
	app->map->Load();

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
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	//if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 1;

	//if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 1;

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
		

	//app->render->DrawTexture(img, 380, 100); // Placeholder not needed any more

	//Draw actual scene
	switch (actualScene)
	{
	case Scene::INTRO:
		//Draw intro
		app->intro->Draw();

		//Cambiar al mapa de juego
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			actualScene = Scenes::GAMEMAP;
		break;
	case Scene::GAMEMAP:
		// Draw map
		app->map->Draw();
		break;
	case Scene::WIN:
		app->victory->Draw();
		break;
	case Scene::LOSE:
		app->victory->Draw();
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
