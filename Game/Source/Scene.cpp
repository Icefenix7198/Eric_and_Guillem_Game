#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "ModuleFadeToBlack.h"
#include "GuiControl.h"
#include "GuiButton.h"
#include "GuiManager.h"

//Diferent Scenes
#include "Intro.h"
#include "Map.h"
#include "Victoria.h"
#include "Muerte.h"
#include "Settings.h"

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

	for (pugi::xml_node itemNode = config.child("life"); itemNode; itemNode = itemNode.next_sibling("life"))
	{
		Life* life = (Life*)app->entityManager->CreateEntity(EntityType::LIFE);
		life->parameters = itemNode;
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

	// L15: DONE 2: Declare a GUI Button and create it using the GuiManager
	uint w, h;
	app->win->GetWindowSize(w, h);
	button1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "    PLAY    ", { 240,510,200,40 }, this);
	button2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "CONTINUE", { 240,560,200,40 }, this);
	button3 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "SETTINGS", { 240,610,200,40 }, this);
	button4 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "    EXIT    ", { 240,660,200,40 }, this);

	if (button1->state == GuiControlState::PRESSED)
	{
		actualScene = Scenes::SETTINGS;
	}

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
	//MENU SETTINGS
	if (button3->state == GuiControlState::PRESSED)
	{

	}

	//Pausar juego
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		pause = !pause;


	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN || button2->state == GuiControlState::PRESSED)
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

		//L15: Draw GUI
		app->guiManager->Draw();

		//Cambiar al mapa de juego
		if (button1->state == GuiControlState::PRESSED || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			app->render->camera.y = -768;
			actualScene = Scenes::GAMEMAP;
			countDown.Start();
			timeLeft = maxTime;
			for (size_t i = 0; i < 255*4; i++) //Ni idea porque no transiciona
			{
				app->render->DrawRectangle({ 0,0,300,300 }, 215, 215, 215,i/4);
			}
			//app->fade->JustFade();
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
		player->Reset();
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
		player->Reset();
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

	case Scene::SETTINGS:
		app->settings->Draw();
		
	default:
		break;
	}

	

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || button4->state == GuiControlState::PRESSED)
		ret = false;

	return ret;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
	case 1:
		LOG("Button 1 click");
		break;
	case 2:
		LOG("Button 2 click");
		break;
	case 3:
		LOG("Button 1 click");
		actualScene = Scenes::SETTINGS;
		break;
	case 4:
		LOG("Button 2 click");
		break;
	}

	return true;
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
	lastTime= data.child("scene").attribute("time").as_int();
	countDown.Start();


	return true;
	
}

bool Scene::SaveState(pugi::xml_node& data)
{
	pugi::xml_node scene = data.append_child("scene");
	pugi::xml_node camara = data.append_child("camara");
	
	camara.append_attribute("x")=app->render->camera.x;
	camara.append_attribute("y")= app->render->camera.y;

	scene.append_attribute("actualScene") = actualScene;
	scene.append_attribute("time") = maxTime - timeLeft;
	
	return true;
}

