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

	pbody->body->ApplyForceToCenter(b2Vec2(1, 0), 10);

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