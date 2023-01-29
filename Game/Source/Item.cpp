#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

#include "EntityManager.h"
#include "Player.h"

#include "Map.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	pbody = app->physics->CreateCircleSensor(position.x + 16, position.y + 16, 16, bodyType::KINEMATIC);
	pbody->body->SetTransform({ PIXEL_TO_METERS(parameters.attribute("x").as_int()),PIXEL_TO_METERS(parameters.attribute("y").as_int()) }, 0);
	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ITEM;
	pbody->listener = this;
	return true;
}

bool Item::Reset()
{
	bool ret = true;
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	if (pbody == nullptr)
	{
		pbody = app->physics->CreateCircle(position.x, position.y - 45, 32 / 2, bodyType::DYNAMIC);
	}
	else
	{
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y)), 0);

	}
	this->renderable = true;
	pbody->listener = this;
	pbody->ctype = ColliderType::ITEM;

	return ret;
}

bool Item::Update()
{
	//If not GameMap no print ni fisica
	if (app->scene->actualScene != app->scene->GAMEMAP)
	{
		return true;
	}

	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	
	if (this->pbody->ctype == ColliderType::ITEM)
	{
		app->render->DrawTexture(texture, position.x, position.y);
	}

	return true;
}

bool Item::CleanUp()
{
	//TODO ERIC O GUILLEM: AQUI METER QUE SE DESTRUYA EL ITEM
	pbody->~PhysBody();

	return true;
}

bool Item::LoadState(pugi::xml_node& data)
{
	Reset();
	//Damos como data uno de los enemy directamente (habiendo uno por cada enemigo)
	float x = data.attribute("x").as_int();
	float y = data.attribute("y").as_int();

	pbody->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

	pbody->ctype = (ColliderType)data.attribute("collider").as_int();

	return true;
}

bool Item::SaveState(pugi::xml_node& data)
{
	pugi::xml_node vida = data.append_child("moneda");

	vida.append_attribute("x") = position.x;
	vida.append_attribute("y") = position.y;
	vida.append_attribute("collider") = (int)pbody->ctype;

	return true;
}