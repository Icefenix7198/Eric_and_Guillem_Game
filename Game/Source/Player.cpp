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

#include <math.h>

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

	/*ListItem<rects>*/
	Run.PushBack({ 0, 0, 27, 36 });
	Run.PushBack({ 27, 0, 27, 36 });
	Run.PushBack({ 54, 0, 27, 36 });
	Run.loop = true;
	Run.speed = 0.1f;

	Jump.PushBack({ 0, 72, 27, 36 });
	Jump.PushBack({ 27, 72, 27, 36 });
	Jump.PushBack({ 54, 72, 27, 36 });
	Jump.loop = true;
	Jump.speed = 0.1f;	

	JumpIdle.PushBack({ 0, 36, 27, 36 });
	JumpIdle.PushBack({ 27, 36, 27, 36 });
	JumpIdle.loop = true;
	JumpIdle.speed = 0.1f;

	Weapon.PushBack({ 54, 36, 27, 36 });
	Weapon.PushBack({ 54, 36, 27, 36 });
	Weapon.loop = true;
	Weapon.speed = 0.1f;

	Nulle.PushBack({ 0, 0, 2, 2 });

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
	FxJump = parameters.attribute("FxJump").as_string();
	FxWin = parameters.attribute("FxWin").as_string();
	FxLose = parameters.attribute("FxLose").as_string();
	pathFullHP = parameters.attribute("containerFull").as_string();
	pathNoneHP = parameters.attribute("containerEmpty").as_string();
	
	/*for(nodoDePugi = parameters.child("AnimationRun"); nodoDePugi !=NULL; nodoDePugi= parameters.child("AnimationRun").next_sibling();)
	{
		rect.x = nodoDePugi.attribute("x");
		rect.y = nodoDePugi.attribute("7");
		rect.h = nodoDePugi.attribute("h");
		rect.w = nodoDePugi.attribute("w");
	}*/
	
	////initilize textures
	//texture = app->tex->Load(texturePath);

	//initializate sound
	JumpSound = app->audio->LoadFx(FxJump);
	WinSound = app->audio->LoadFx(FxWin);
	DeadSound = app->audio->LoadFx(FxLose);
	

	return true;
}

bool Player::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	fullHP = app->tex->Load(pathFullHP);
	voidHP = app->tex->Load(pathNoneHP);


	currentAnimation = &Run;
	currentAnimation2 = &Nulle;
	// L07 TODO 5: Add physics to the player - initialize physics body
	//playerBody = app->physics->CreateRectangle(position.x + 32 / 2, position.y + 32 / 2, 16, 32, bodyType::DYNAMIC); //MEJOR ESFERA YA QUE EL RECTANGULO ROTA Y DA PROBLEMAS
	playerBody = app->physics->CreateCircle(position.x, position.y - 45, 32 / 2, bodyType::DYNAMIC);
	playerBody->listener = this;
	swordExist = false;
	

	return true;
	
}

bool Player::Reset() 
{
	bool ret = true;
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	if (playerBody==nullptr)
	{
		playerBody = app->physics->CreateCircle(position.x, position.y - 45, 32 / 2, bodyType::DYNAMIC);
	}
	else 
	{
		playerBody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y)), 0);
			
	}
	app->render->camera.x = 0;
	GodMode = false;
	inmune = false;
	lives = 4;
	coinsPicked = 0;
	return ret;
}

bool Player::Update()
{
	if (app->scene->actualScene!=app->scene->GAMEMAP )
	{
		return true;
	}

	//Print actual lives
	for(int i=0;lives>i;i++)
	{
		app->render->DrawTexture(fullHP, -app->render->camera.x+30+i*30, -app->render->camera.y);

	}
	for (int i = lives; 4 > i; i++)
	{
		app->render->DrawTexture(voidHP, -app->render->camera.x + 30 + i * 30, -app->render->camera.y);

	}

	//Print coins
	SString coinToText;
	coinToText.Create("COINS %d", coinsPicked);
	SDL_Color colorLetras;
	colorLetras.r = 239;
	colorLetras.g = 164;
	colorLetras.b = 52;
	colorLetras.a = 255;
	app->render->DrawText(coinToText.GetString(), 500, 5, 70, 20, colorLetras); 

	

	if (app->scene->pause)
	{
		playerBody->body->SetLinearVelocity(b2Vec2(0, 0.00f));
		playerBody->body->SetGravityScale(0);
		SDL_Rect rect = currentAnimation->GetCurrentFrame();
		app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, Invert);
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
		jump += 1;
		}
		else 
		{
			jump -= 1;
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
	{
		if (app->scene->maxCameraPosLeft >= app->render->camera.x && -position.x+app->scene->cameraMargin*app->map->mapData.tileWidth >= app->render->camera.x && movement<0)
		{
			app->render->camera.x += -movement; //TODO ERIC: MOVER LA CAMARA TANTO COMO EL PLAYER
		}
		if (app->scene->maxCameraPosRigth <= app->render->camera.x && position.x + app->scene->cameraMargin * app->map->mapData.tileWidth >= -app->render->camera.x+app->render->camera.w && movement> 0)
		{
			app->render->camera.x += -movement; //TODO ERIC: MOVER LA CAMARA TANTO COMO EL PLAYER
		}
		if (app->scene->maxCameraPosUp >= app->render->camera.y && -position.y + app->scene->cameraMargin * app->map->mapData.tileHeight >= app->render->camera.y)
		{
			app->render->camera.y += 3; //TODO ERIC: MOVER LA CAMARA TANTO COMO EL PLAYER
		}
		if (app->scene->maxCameraPosDown <= app->render->camera.y && position.y + app->scene->cameraMargin * app->map->mapData.tileHeight >= -app->render->camera.y + app->render->camera.h)
		{
			app->render->camera.y += -3; //TODO ERIC: MOVER LA CAMARA TANTO COMO EL PLAYER
		}
	}
	
	//DEBUGGING
	{
		if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
			app->entityManager->Reset();
	
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
	}

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && CanJump>0)
	{
		jump=-16;
		CanJump -=1;
		app->audio->PlayFx(JumpSound);

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			Invert = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
			currentAnimation = &Jump;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			Invert = SDL_RendererFlip::SDL_FLIP_NONE;
			currentAnimation = &Jump;
		}
		else
		{
			currentAnimation = &JumpIdle;

		}
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
		direction = -1;
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			currentAnimation = &Jump;
		}
		else
		{
			currentAnimation = &Run;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		movement=5;
		Invert = SDL_RendererFlip::SDL_FLIP_NONE;

		direction = 1;
		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			currentAnimation = &Jump;
		}
		else
		{
			currentAnimation = &Run;
		}
	}

	currentAnimation->Update();	
	currentAnimation2->Update();	
	SDL_Rect rect = currentAnimation->GetCurrentFrame();
	SDL_Rect rect2 = currentAnimation2->GetCurrentFrame();

	

	playerBody->body->SetLinearVelocity(velocity);
	
	b2Transform transform = playerBody->body->GetTransform();
	b2Vec2 vec=transform.p;

	position.x = METERS_TO_PIXELS(vec.x)-32/2;
	position.y = METERS_TO_PIXELS(vec.y)-32/2;
	
	if (inmune == true )
	{
		if (blinkingFrames > 8) { blinkingFrames -= 20; }
		++blinkingFrames;
		if (blinkingFrames<0)
		{
		app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, Invert);
		}
	}
	else
	{
		app->render->DrawTexture(texture, position.x, position.y, &rect, 1.0f, NULL, NULL, NULL, Invert);
	}

	//Tras 3 segundos quitar inmunidad
	if (inmunityFrames.ReadSec() > 2) { inmune = false;}

	/*if (app->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
	{
		app->map->Load();
	}*/
	
	//Ataque magico
	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN  && swordExist==false)
	{
		weapon = app->physics->CreateRectangleSensor(position.x, position.y + 40, 32, 16, STATIC);
		weapon->ctype = ColliderType::WEAPON;
		//weapon->body->SetTransform({ PIXEL_TO_METERS(position.x),PIXEL_TO_METERS(position.y) }, 0);
		
		posFlameX = position.x;
		posFlameY = position.y + 16;
		swordExist = true;
		swordDuration.Start();
		dirToGo = direction;
		
	}
	if (weapon!=nullptr && swordDuration.ReadSec()<1 && swordExist==true && app->scene->pause==false)
	{
		
		posFlameX += 2 * dirToGo;
		
		weapon->body->SetTransform({ PIXEL_TO_METERS(posFlameX),PIXEL_TO_METERS(posFlameY) }, 1.57079632679);
		if(dirToGo >0){Invert = SDL_RendererFlip::SDL_FLIP_NONE;}
		if (dirToGo < 0) { Invert = SDL_RendererFlip::SDL_FLIP_HORIZONTAL; }
		currentAnimation2 = &Weapon;
		app->render->DrawTexture(texture, posFlameX, posFlameY-16, &rect2, 1.0f, 0, NULL, NULL, Invert);
		
		weapon->ctype = ColliderType::WEAPON;
	}
	if (swordDuration.ReadSec() > 1 && swordExist==true)
	{
		weapon->body->DestroyFixture(weapon->body->GetFixtureList());
	  	weapon->body->GetWorld()->DestroyBody(weapon->body);
		app->entityManager->DestroyEntity(weapon->listener);
		swordExist = false;
	}

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

 		//app->entityManager->DestroyEntity(physB->listener);
		//physB->listener->Disable();
		//TODO ERIC:Arreglar esto
		++coinsPicked;
		physB->ctype = ColliderType::UNKNOWN;
		

		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		if (CanJump == 0 && jump ==0) { CanJump = 2;}
		break;
	case ColliderType::WALL:
		LOG("Collison WALL");
		break;
	case ColliderType::DEAD:
		app->scene->actualScene = app->scene->LOSE;
		app->audio->PlayFx(DeadSound);
		
		break;
	case ColliderType::WIN:
		app->scene->actualScene = app->scene->WIN;
		if (physB->ctype == ColliderType::WIN)
		{
			app->audio->PlayFx(WinSound);
		}
		break;
	case ColliderType::ENEMY:
		if (GodMode==false && inmune==false)
		{
			if (lives>1)
			{
				--lives;
				inmunityFrames.Start();
				inmune = true;
			}
			else
			{
				app->scene->actualScene = app->scene->LOSE;
				app->audio->PlayFx(DeadSound); //TODO: QUIZA HACER MODO DE VIDAS
			}
			
		
		}
		
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::WEAPON:
		LOG("Collision WEAPON");
	case ColliderType::LIVE:
		
		if (lives<4)
		{
		++lives;
		physB->ctype = ColliderType::UNKNOWN;
		}
		LOG("Collision WEAPON");
		
		break;
	}



}