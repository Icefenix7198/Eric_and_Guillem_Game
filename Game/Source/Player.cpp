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

#include "Animation.h"

#include "Item.h"
#include "EntityManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	Anim.PushBack({ 0, 36, 27, 36 });
	Anim.PushBack({ 256, 64, 27, 36 });
	Anim.PushBack({ 288, 64, 27, 36 });
	Anim.loop = true;
	Anim.speed = 0.1f;
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
	
	currentAnimation = &Anim;

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);

	// L07 TODO 5: Add physics to the player - initialize physics body
	//playerBody = app->physics->CreateRectangle(position.x + 32 / 2, position.y + 32 / 2, 16, 32, bodyType::DYNAMIC); //MEJOR ESFERA YA QUE EL RECTANGULO ROTA Y DA PROBLEMAS
	playerBody = app->physics->CreateCircle(position.x, position.y - 45, 32 / 2, bodyType::DYNAMIC);
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
	if (jump!=0)
	{
		velocity.y = jump;
		//For god mode proporses, if jump is positive (goes downwards) the jump variable gets added to become 0
		if(jump<0)
		{
		jump += 2;
		}
		else 
		{
			jump -= 2;
		}   
	}
	else
	{
		velocity.y = -GRAVITY_Y*(GodMode==false); //Si God Mode == true es falso por lo tanto gravedad = 0
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
	if (app->scene->maxCameraPosRigth <= app->render->camera.x && position.x + app->scene->cameraMargin * app->map->mapData.tileWidth >= -app->render->camera.x+app->render->camera.w && movement> 0)
	{
		app->render->camera.x += -movement; //TODO ERIC: MOVER LA CAMARA TANTO COMO EL PLAYER
	}
	
	//DEBUGGING

	//Show Vectors
	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		ShowVectors = !ShowVectors;

	//God Mode
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) 
	{
	if (GodMode==true)
	{
		GodMode = false;
	}
	else
	{
		GodMode = true;
	}
	}

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && CanJump>0)
	{
		jump=-24;
		CanJump -=1;
	}

	//Fly Up and Down during God Mode
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && GodMode==true)
	{
		jump=4;
	}
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && GodMode == true)
	{
		jump = -4;
	}
		

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		movement = -5;
		Invert = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		currentAnimation = &Anim;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		movement=5;
		Invert = SDL_RendererFlip::SDL_FLIP_NONE;
		currentAnimation = &Anim;
	}
		
	currentAnimation->Update();	
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	


	playerBody->body->SetLinearVelocity(velocity);
	
	b2Transform transform = playerBody->body->GetTransform();
	b2Vec2 vec=transform.p;

	position.x = METERS_TO_PIXELS(vec.x)-32/2;
	position.y = METERS_TO_PIXELS(vec.y)-32/2;

	
	
	app->render->DrawTexture(texture, position.x, position.y/*, &rect, 1.0f, NULL, NULL, NULL, Invert*/);
	 
	//Print player movement vector
	if (ShowVectors==true)
	{
		//+16 para que este centrado, es el tamaño del collider del player
		app->render->DrawLine(position.x+16, position.y+16, position.x + METERS_TO_PIXELS(velocity.x/8)+16,position.y + METERS_TO_PIXELS(velocity.y/8)+16, 0, 255, 0);
	}
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
		if (CanJump == 0) { CanJump = 2; app->audio->PlayFx(pickCoinFxId);
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