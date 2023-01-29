#include "Life.h"
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

Life::Life() : Entity(EntityType::ITEM)
{
	name.Create("life");
}

Life::~Life() {}

bool Life::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Life::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	pbody = app->physics->CreateCircleSensor(position.x + 16, position.y + 16, 16, bodyType::KINEMATIC);
	pbody->body->SetTransform({ PIXEL_TO_METERS(parameters.attribute("x").as_int()),PIXEL_TO_METERS(parameters.attribute("y").as_int()) }, 0);
	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::LIVE;
	pbody->listener = this;
	return true;
}

bool Life::Reset()
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
	pbody->ctype = ColliderType::LIVE;

	return ret;
}

bool Life::Update()
{
	//If not GameMap no print ni fisica
	if (app->scene->actualScene != app->scene->GAMEMAP)
	{
		return true;
	}

	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	
	if (this->pbody->ctype == ColliderType::LIVE)
	{
		app->render->DrawTexture(texture, position.x, position.y);
	}

	return true;
}

bool Life::CleanUp()
{
	//TODO ERIC O GUILLEM: AQUI METER QUE SE DESTRUYA EL ITEM
	pbody->~PhysBody();

	return true;
}
