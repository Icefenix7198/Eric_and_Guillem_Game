#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Intro.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

#include "Player.h"

#include "Point.h"
#include "Animation.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Intro::Intro() : Module(), mapLoaded(false)
{
    name.Create("intro");
    
}

// Destructor
Intro::~Intro()
{}

// Called before render is available
bool Intro::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    texturePath = config.child("start1").attribute("texturepath").as_string();
    texturePath2 = config.child("start2").attribute("texturepath2").as_string();
    
    
    return ret;
}

bool Intro::Start()
{
    texture2 = app->tex->Load(texturePath2);
    texture = app->tex->Load(texturePath);
    change = 30;
    return true;
}

bool Intro::Update()
{
    
    return true;
}

void Intro::Draw()
{
    if (texturePath == false)
        return;
   if (app->scene->actualScene==app->scene->INTRO)
   {
       
       if (change>=0)
       {
        app->render->DrawTexture(texture,0,0);
        --change;
        if (change == 0)
            change = -30;
       }
       if (change <= 0)
       {
           app->render->DrawTexture(texture2, 0, 0);
           ++change;
           if (change == 0)
               change = 30;
       }
       
   }             
}

// Called before quitting
bool Intro::CleanUp()
{
    app->tex->UnLoad(texture);
    app->tex->UnLoad(texture);
    //RELEASE(texture);

    return true;
}








