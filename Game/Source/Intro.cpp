#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Intro.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

#include "Player.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Intro::Intro() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Intro::~Intro()
{}

// Called before render is available
bool Intro::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    texturePath = config.child("intro").attribute("texturepath").as_string();
    
    
    return ret;
}

void Intro::Draw()
{
    if (texturePath == false)
        return;
    texture = app->tex->Load(texturePath);
   if (app->scene->actualScene==app->scene->INTRO)
   { 
     
      app->render->DrawTexture(texture,0,0);
   }             

     



}

// Called before quitting
bool Intro::CleanUp()
{
    RELEASE(texture);

    return true;
}








