#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

#include "Map.h"
#include "EntityManager.h"
#include "Pathfinding.h"

#include "Point.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("enemy");
}

Enemy::~Enemy() {}

bool Enemy::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	fly = parameters.attribute("fly").as_bool();

	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	
	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	 
	return true;
}

bool Enemy::Reset()

{
	//TODO:SWITCH DE NIVELES
	
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
	return ret;
}

bool Enemy::Update()
{
	//If not GameMap no print ni fisica
	if (app->scene->actualScene != app->scene->GAMEMAP)
	{
		return true;
	}

	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	tilePos = app->map->WorldToMap(position.x,position.y);
	tileObjective = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

	

	switch (actualState)
	{
	case Enemy::IDLE:

		//Moverse de lado a lado
		pbody->body->SetLinearVelocity(b2Vec2(direction*speed, -GRAVITY_Y));
		if (app->pathfinding->IsWalkable(iPoint(tilePos.x+direction,tilePos.y)))
			(direction*=-1);		
		
		//Detectar si el player esta cerca 
		if (position.DistanceTo(app->scene->player->position)<200) //Pugi Mejor o asi esta bien?
		{
			LOG("DETECTED");
			actualState = Enemy::CHASE;
		}



		//Show Alert Mode
		if (ShowVectors)
		{
			app->render->DrawCircle(position.x, position.y, 200, 255, 255, 0);
			SDL_Rect tileEnemigo;
			int tileSM = app->map->mapData.tileWidth;
			tileEnemigo.x = tilePos.x * tileSM;
			tileEnemigo.y = tilePos.y * tileSM;
			tileEnemigo.h = tileEnemigo.w = tileSM;
			app->render->DrawRectangle(tileEnemigo, 0, 0, 255,122);
			
				
		}

		break;
	case Enemy::CHASE:

		if (position.DistanceTo(app->scene->player->position) > 450) //Pugi Mejor o asi esta bien?
		{
			LOG("DETECTED");
			actualState = Enemy::IDLE;
		}

		//Create path towards player

		

 		app->pathfinding->CreatePath(tilePos, tileObjective);


		//Show Alert Mode
		if (ShowVectors)
		{
			app->render->DrawCircle(position.x, position.y, 450, 255, 0, 255);
			
			SDL_Rect tileEnemigo;
			int tileSM = app->map->mapData.tileWidth;
			tileEnemigo.x = tilePos.x * tileSM;
			tileEnemigo.y = tilePos.y * tileSM;
			tileEnemigo.h = tileEnemigo.w = tileSM;
			app->render->DrawRectangle(tileEnemigo, 0, 0, 255, 122);
			
			SDL_Rect tileObj;
			tileObj.x = tileObjective.x * tileSM;
			tileObj.y = tileObjective.y * tileSM;
			tileObj.w = tileObj.h = tileSM;
			app->render->DrawRectangle(tileObj, 122, 0, 255);
		
		}


		break;
	case Enemy::DEAD:
		break;
	default:
		break;
	}


	//Debug

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		ShowVectors = !ShowVectors;

	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		

		//physB->listener->Disable();
		//TODO ERIC:Arreglar esto


		app->entityManager->DestroyEntity(physB->listener);

		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		
		break;
	case ColliderType::WALL:
		LOG("Collison WALL");
		break;
	case ColliderType::DEAD:
	
		physA->body->DestroyFixture(physA->body->GetFixtureList());

		break;
	case ColliderType::WIN:
		
		break;
	case ColliderType::ENEMY:
		
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}



}