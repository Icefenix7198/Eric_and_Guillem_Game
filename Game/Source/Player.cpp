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
	return true;
}

bool Player::Update()
{
	// L07 TODO 5: Add physics to the player - updated player position using physics
	

	b2Vec2 velocity = b2Vec2(GRAVITY_X, -GRAVITY_Y);

	//L02: DONE 4: modify the position of the player using arrow keys and render the texture
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		position.y -= 1;

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		position.y += 1;

	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		velocity= b2Vec2(-5, -GRAVITY_Y);

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		velocity = b2Vec2(5, -GRAVITY_Y);;

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