#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Muerte.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

#include "Player.h"

#include "Point.h"
#include "Animation.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Lose::Lose() : Module(), mapLoaded(false)
{
    name.Create("lose");
    
}

// Destructor
Lose::~Lose()
{}

// Called before render is available
bool Lose::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    texturePath = config.child("lose").attribute("texturepath").as_string();
    
    
    
    return ret;
}

bool Lose::Start()
{
    texture = app->tex->Load(texturePath);
    
    return true;
}

bool Lose::Update()
{

   
    return true;
}

void Lose::Draw()
{
    if (texturePath == false)
        return;
   if (app->scene->actualScene==app->scene->LOSE)
   {
       app->render->DrawTexture(texture,0,0);
   }             
}

// Called before quitting
bool Lose::CleanUp()
{
    app->tex->UnLoad(texture);
    app->tex->UnLoad(texture);
    //RELEASE(texture);

    return true;
}








