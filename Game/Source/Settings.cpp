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

    button1_1 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "   MUSIC +   ", { 415,400,200,40 }, this);
    button1_2 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "   MUSIC -   ", { 415,450,200,40 }, this);
    button1_3 = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "   BACK   ", { 800,660,200,40 }, this);

    button2_1 = (GuiToggle*)app->guiManager->CreateGuiControl(GuiControlType::TOGGLE, 3, "  CHEK 1  ", { 415,500,200,40 }, this);
    button2_2 = (GuiToggle*)app->guiManager->CreateGuiControl(GuiControlType::TOGGLE, 4, "  CHEK 2  ", { 415,550,200,40 }, this);
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
    if (app->scene->actualScene == app->scene->SETTINGS)
    {
        app->render->DrawTexture(texture, 0, 0);
        button1_1->Draw(app->render);
        button1_2->Draw(app->render);
        button2_1->Draw(app->render);
        button2_2->Draw(app->render);
        button1_3->Draw(app->render);
        
    }
}

bool Settings::OnGuiMouseClickEvent(GuiControl* control)
{
    // L15: DONE 5: Implement the OnGuiMouseClickEvent method
    LOG("Event by %d ", control->id);

    switch (control->id)
    {
    case 1:
        LOG("Button 1 click");
        break;
    case 2:
        LOG("Button 2 click");
        break;
    case 3:
        LOG("Button 1 click");
        break;
    case 4:
        LOG("Button 2 click");
        break;
    case 5:
        LOG("Button 2 click");
        app->scene->actualScene = app->scene->Scenes::INTRO;
        break;
    }

    return true;
}

// Called before quitting
bool Settings::CleanUp()
{
    app->tex->UnLoad(texture);
    app->tex->UnLoad(texture);
    //RELEASE(texture);

    return true;
}








