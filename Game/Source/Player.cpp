#include "Player.h"
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

#include "Item.h"
#include "EntityManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//pos = position;
	//texturePath = "Assets/Textures/player/idle1.png";

	//L02: DONE 5: Get Player parameters from XML
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	
	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 TODO 5: Add physics to the player - initialize physics body
	//playerBody = app->physics->CreateRectangle(position.x + 32 / 2, position.y + 32 / 2, 16, 32, bodyType::DYNAMIC); //MEJOR ESFERA YA QUE EL RECTANGULO ROTA Y DA PROBLEMAS
	playerBody = app->physics->CreateCircle(position.x + 32, position.y + 32, 32 / 2, bodyType::DYNAMIC);
	playerBody->listener = this;
	return true;
	
}

bool Player::Update()
{
	if (app->scene->actualScene!=app->scene->GAMEMAP)
	{
		return true;
	}
	
	// L07 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity=b2Vec2(0,0);
	
	//En cada iteracion jump y movimiento se hacen un poco mas pequeñas, dando la impersion de que desacelera el player
	if (jump<0)
	{
		velocity.y = jump;
		jump += 2;
	}
	else
	{
		velocity.y = -GRAVITY_Y;
	}

	if (movement>0)
	{
		velocity.x = movement;
		--movement;
	}
	else if(movement<0)
	{
		velocity.x = movement;
		++movement;
	}
	else
	{
		velocity.x = GRAVITY_X;
	}

	//Mover la camara
	if (app->scene->maxCameraPosLeft >= app->render->camera.x && -position.x+app->scene->cameraMargin*app->map->mapData.tileWidth >= app->render->camera.x && movement<0)
	{
		app->render->camera.x += -movement; //TODO ERIC: MOVER LA CAMARA TANTO COMO EL PLAYER
	}
	if (app->scene->maxCameraPosRigth <= app->render->camera.x && position.x + app->scene->cameraMargin * app->map->mapData.tileWidth >= -app->render->camera.x+app->render->camera.w /*&& movement> 0*/)
	{
		app->render->camera.x += -movement; //TODO ERIC: MOVER LA CAMARA TANTO COMO EL PLAYER
	}
	


	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && CanJump==true)
	{
		jump=-30;
		CanJump = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		position.y += 1;

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		movement = -10;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && CanJump == true)
		{
			jump = -30;
			CanJump = false;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		movement=10;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && CanJump == true)
		{
			jump = -30;
			CanJump = false;
		}
	}
		
	
	


	playerBody->body->SetLinearVelocity(velocity);
	
	b2Transform transform = playerBody->body->GetTransform();
	b2Vec2 vec=transform.p;

	position.x = METERS_TO_PIXELS(vec.x)-32/2;
	position.y = METERS_TO_PIXELS(vec.y)-32/2;

	app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Player::CleanUp()
{

	return true;
}

// L07 DONE 6: Define OnCollision function for the player. Check the virtual function on Entity class
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	// L07 DONE 7: Detect the type of collision

	switch (physB->ctype)
	{
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		physB->~PhysBody();
		//app->entityManager->DestroyEntity();
		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		if (CanJump == false) { CanJump = true; app->audio->PlayFx(pickCoinFxId);
		}
		break;
	case ColliderType::WALL:
		LOG("Collison WALL");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}



}