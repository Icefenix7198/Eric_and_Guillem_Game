#include "EntityManager.h"
#include "Player.h"
#include "Item.h"
#include "Life.h"
#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

bool EntityManager::Reset() {

	bool ret = true;

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false)
		{
			pEntity->Enable();
		}
		ret = item->data->Reset();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L02: DONE 2: Instantiate entity according to the type and add the new entoty it to the list of Entities

	switch (type)
	{

	case EntityType::PLAYER:
		entity = new Player();
		break;

	case EntityType::ITEM:
		entity = new Item();
		break;

	case EntityType::LIFE:
		entity = new Life();
		break;

	case EntityType::ENEMY:
		entity = new Enemy();
		break;

	default: break;
	}

	// Created entities are added to the list
	AddEntity(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	
	/*entity->Disable();*/

	ListItem<Entity*>* item;
	
	for (item = entities.start; item != NULL; item = item->next)
	{
	
		if (item->data == entity)
		{
			entities.Del(item);
		}
		

	}
			

	
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
 	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Update();
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node& data)
{
	float x = data.child("player").attribute("x").as_int();
	float y = data.child("player").attribute("y").as_int();
	int lifes= data.child("player").attribute("lifes").as_int();
	int coins = data.child("player").attribute("coins").as_int();

	app->scene->player->playerBody->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

	app->scene->player->lives = lifes;
	app->scene->player->coinsPicked = coins;

	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	pugi::xml_node enemigo = data.child("enemies").first_child();
	pugi::xml_node vida = data.child("vidas").first_child();
	pugi::xml_node moneda = data.child("monedas").first_child();

	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		
		if (pEntity->type == EntityType::ENEMY)
		{
			pEntity->LoadState(enemigo);
			enemigo = enemigo.next_sibling();
		}

		if (pEntity->type == EntityType::ITEM)
		{
			pEntity->LoadState(moneda);
			moneda = moneda.next_sibling();
		}

		if (pEntity->type == EntityType::LIFE)
		{
			pEntity->LoadState(vida);
			vida = vida.next_sibling();
		}


		if (pEntity->active == false) continue;

	}


	return true;
}

bool EntityManager::SaveState(pugi::xml_node& data)
{
	pugi::xml_node player = data.append_child("player");
	pugi::xml_node enemies = data.append_child("enemies");
	pugi::xml_node monedas = data.append_child("monedas");
	pugi::xml_node vidas = data.append_child("vidas");

	player.append_attribute("x") = app->scene->player->position.x;
	player.append_attribute("y") = app->scene->player->position.y;
	player.append_attribute("lifes") = app->scene->player->lives;
	player.append_attribute("coins") = app->scene->player->coinsPicked;

	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL; item = item->next)
	{
		pEntity = item->data;
		if(pEntity->type==EntityType::ENEMY)
		{
			pEntity->SaveState(enemies);
		}
		
		
		if (pEntity->type == EntityType::ITEM)
		{
			pEntity->SaveState(monedas);
		}

		if (pEntity->type == EntityType::LIFE)
		{
			pEntity->SaveState(vidas);
		}
		
		if (pEntity->active == false) continue;


	}

	



	return true;
}