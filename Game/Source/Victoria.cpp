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
   
}

// Destructor
Victory::~Victory()
{}

// Called before render is available
bool Victory::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    texturePath = config.child("win").attribute("texturepath").as_string();
   
    
    
    return ret;
}

bool Victory::Start()
{ 
    texture = app->tex->Load(texturePath);
    return true;
}

bool Victory::Update()
{
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








