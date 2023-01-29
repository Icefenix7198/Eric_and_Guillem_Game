#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Settings.h"
#include "Scene.h"
#include "GuiToggle.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

#include "Player.h"

#include "Point.h"
#include "Animation.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Settings::Settings() : Module(), mapLoaded(false)
{
    name.Create("victory");

}

// Destructor
Settings::~Settings()
{}

// Called before render is available
bool Settings::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    texturePath = config.child("win").attribute("texturepath").as_string();



    return ret;
}

bool Settings::Start()
{
    texture = app->tex->Load(texturePath);

    button1_1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "    PLAY    ", { 240,510,200,40 }, this);
    button1_2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "CONTINUE", { 240,560,200,40 }, this);
    button2_1 = (GuiToggle*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "SETTINGS", { 240,610,200,40 }, this);
    button2_2 = (GuiToggle*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "    EXIT    ", { 240,660,200,40 }, this);
    return true;
}

bool Settings::Update()
{
    return true;
}

void Settings::Draw()
{
    if (texturePath == false)
        return;
    if (app->scene->actualScene == app->scene->WIN)
    {
        app->render->DrawTexture(texture, 0, 0);
    }
}

// Called before quitting
bool Settings::CleanUp()
{
    app->tex->UnLoad(texture);
    app->tex->UnLoad(texture);
    //RELEASE(texture);

    return true;
}








