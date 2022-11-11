#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Victoria.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

#include "Player.h"

#include "Point.h"
#include "Animation.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Victory::Victory() : Module(), mapLoaded(false)
{
    name.Create("victory");
    Main.PushBack({ 1024, 0, 1024, 768 });
    Main.PushBack({ 1024, 0, 1024, 768 });
    Main.loop = true;
    Main.speed = 0.1f;
}

// Destructor
Victory::~Victory()
{}

// Called before render is available
bool Victory::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    texturePath = config.child("intro").attribute("texturepath").as_string();
    texturePath2 = config.child("intro").attribute("texturepath2").as_string();
    
    
    return ret;
}

bool Victory::Start()
{
    texture2 = app->tex->Load(texturePath2);
    texture = app->tex->Load(texturePath);
    currentAnimation2 = &Main;
    return true;
}

bool Victory::Update()
{
    currentAnimation2 = &Main;
    currentAnimation2->Update();
    SDL_Rect rect2 = currentAnimation2->GetCurrentFrame();
    app->render->DrawTexture(texture2, 0, 0, &rect2);
    return true;
}

void Victory::Draw()
{
    if (texturePath == false)
        return;
   if (app->scene->actualScene==app->scene->WIN)
   {
       app->render->DrawTexture(texture,0,0);
   }             
}

// Called before quitting
bool Victory::CleanUp()
{
    app->tex->UnLoad(texture);
    app->tex->UnLoad(texture);
    //RELEASE(texture);

    return true;
}








