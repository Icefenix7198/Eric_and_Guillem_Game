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
	speed= parameters.attribute("speed").as_float();
	alarmRange= parameters.attribute("alarmRange").as_int();
	loseRange= parameters.attribute("loseRange").as_int();
	
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);
	
	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	if (fly==true)
	{
		pbody->body->SetGravityScale(0);
	}
	else 
	{
		pbody->body->SetGravityScale(1);
	}
	isDead = false;

	pbody->listener = this;

	return true;
}

bool Enemy::Reset()

{
	//TODO:SWITCH DE NIVELES
	
	bool ret = true;
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	if (actualState==state::DEAD)
	{
		pbody = app->physics->CreateCircle(position.x+16, position.y+16 - 45, 32 / 2, bodyType::DYNAMIC);
		//pbody->listener = this;
	}
	else
	{
		pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y)), 0);

	}
	actualState = IDLE;
	isDead = false;
	app->pathfinding->ClearLastPath();
	return ret;
}

bool Enemy::Update()
{
	//If not GameMap no print ni fisica
	if (app->scene->actualScene != app->scene->GAMEMAP)
	{
		return true;
	}


	//Pause
	if (app->scene->pause)
	{
		pbody->body->SetLinearVelocity(b2Vec2(0, 0.00f));
		//playerBody->body->SetGravityScale(0); ////Lo dejo para cuando se le meta animación
		//SDL_Rect rect = currentAnimation->GetCurrentFrame();
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = rect.h = 32;
		app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL/*, Invert*/);
		return true;
	}

	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  
	position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x) - 16;
	position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y) - 16;

	tilePos = app->map->WorldToMap(position.x,position.y);
	tileObjective = app->map->WorldToMap(app->scene->player->position.x, app->scene->player->position.y);

	if (actualState!=state::DEAD)
	{
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = rect.h = 32;
		if(directionX<0)
			app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
		if (directionX > 0)
			app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
	}

		

	switch (actualState)
	{
	case Enemy::IDLE:

		//Moverse de lado a lado
		if (fly == true)
		{
			pbody->body->SetLinearVelocity(b2Vec2(0,0));

		}

		if (fly==false)
		{
			pbody->body->SetLinearVelocity(b2Vec2(directionX*speed,0));

			//directionY = 0;
			if (!app->pathfinding->IsWalkable(iPoint(tilePos.x+directionX,tilePos.y),fly))
				(directionX*=-1); 
		}

		
				
		
		//Detectar si el player esta cerca 
		if (position.DistanceTo(app->scene->player->position)< alarmRange) //Pugi Mejor o asi esta bien?
		{
			LOG("DETECTED");
			actualState = Enemy::CHASE;
		}

		

		//Show Alert Mode
		if (ShowVectors)
		{
			app->render->DrawCircle(position.x, position.y, alarmRange, 255, 255, 0);
			SDL_Rect tileEnemigo;
			int tileSM = app->map->mapData.tileWidth;
			tileEnemigo.x = tilePos.x * tileSM;
			tileEnemigo.y = tilePos.y * tileSM;
			tileEnemigo.h = tileEnemigo.w = tileSM;
			app->render->DrawRectangle(tileEnemigo, 0, 0, 255,122);
			
				
		}

		break;

	case Enemy::CHASE:

		if (position.DistanceTo(app->scene->player->position) > loseRange) 
		{
			LOG("LOST TRACK");
			actualState = Enemy::IDLE;
		}

		//Create path towards player

		app->pathfinding->CreatePath(tilePos, tileObjective, fly);
		
		if (fly==true)
		{
			if (app->pathfinding->GetLastPath()->Count() !=0 )
			{
				const DynArray<iPoint>* path = app->pathfinding->GetLastPath();

				pathStep = 1;

				posNextStep = app->map->MapToWorld(path->At(pathStep)->x, path->At(pathStep)->y);
				
				if (posNextStep.x > position.x)
				{
					directionX = 1;
				}
				if (posNextStep.x < position.x)
				{
					directionX = -1;
				}

				if (posNextStep.y > position.y)
				{
					directionY = 1;
				}
				if (posNextStep.y < position.y)
				{
					directionY = -1;
				}

				pbody->body->SetLinearVelocity(b2Vec2(directionX * 2 * speed, directionY*2*speed));
			}
		}

		if (fly==false)
		{
			if (app->pathfinding->GetLastPath()->Count() > 0 )
			{
				const DynArray<iPoint>* path = app->pathfinding->GetLastPath();

				pathStep = 1;
				
				if (path->Count()>1)
				{
					posNextStep = app->map->MapToWorld(path->At(pathStep)->x, path->At(pathStep)->y);
					if (posNextStep.x > position.x)
					{
						directionX = 1;
					}
					else if (posNextStep.x < position.x)
					{
						directionX = -1;
					}
				}
				else
				{
					LOG("mUY CERCA");
					pathStep = 0;
				}

					
				
				
				pbody->body->SetLinearVelocity(b2Vec2(directionX * 2* speed, -GRAVITY_Y));
			}
			else
			{
				pbody->body->SetLinearVelocity(b2Vec2(directionX* speed, 0));

				//directionY = 0;
				if (!app->pathfinding->IsWalkable(iPoint(tilePos.x + directionX, tilePos.y), fly))
					(directionX *= -1);
			}
		}
		

		
		
			
		


		//Show Alert Mode
		if (ShowVectors)
		{
			app->render->DrawCircle(position.x, position.y, loseRange, 255, 0, 255);
			
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

			app->render->DrawLine(position.x+16, position.y+16, position.x + directionX * 12 +16, position.y + directionY * 12+16 , 0, 255, 255);

			const DynArray<iPoint>* path = app->pathfinding->GetLastPath();

			for (uint i = 0; i < path->Count(); ++i)
			{
				iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				SDL_Rect rectangulo;
				rectangulo.x = pos.x;
				rectangulo.y = pos.y;
				rectangulo.h = rectangulo.w = 32;
				app->render->DrawRectangle(rectangulo,255-i*10,0,i*10,100);
			}
		
		}


		break;
	case Enemy::DEAD:

		if (isDead==false)
		{
		pbody->body->DestroyFixture(pbody->body->GetFixtureList());
		pbody->body->GetWorld()->DestroyBody(pbody->body);
		//app->entityManager->DestroyEntity(pbody->listener);
		isDead = true;
		}
		

		break;
	default:
		break;
	}


	app->pathfinding->ClearLastPath();
	//Debug

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		ShowVectors = !ShowVectors;

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		ShowVectors = !ShowVectors;

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		ShowVectors = !ShowVectors;

	//app->render->DrawTexture(texture, position.x, position.y);

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

		actualState = state::DEAD;

		break;
	case ColliderType::WIN:

		break;
	case ColliderType::ENEMY:

		break;

	case ColliderType::WEAPON:

		actualState = state::DEAD;
		
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

bool Enemy::LoadState(pugi::xml_node& data)
{
	Reset();
	//Damos como data uno de los enemy directamente (habiendo uno por cada enemigo)
	float x = data.attribute("x").as_int();
	float y = data.attribute("y").as_int();

	pbody->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

	actualState= data.attribute("state").as_int();
	directionX = data.attribute("directionX").as_int();
	directionY = data.attribute("directionY").as_int();

	return true;
}

bool Enemy::SaveState(pugi::xml_node& data)
{
	pugi::xml_node enemy = data.append_child("enemy");

	enemy.append_attribute("x") = position.x;
	enemy.append_attribute("y") = position.y;
	enemy.append_attribute("state") = actualState;
	enemy.append_attribute("directionX") = directionX;
	enemy.append_attribute("directionY") = directionY;

	return true;
}