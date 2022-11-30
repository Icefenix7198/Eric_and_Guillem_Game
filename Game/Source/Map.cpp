
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"

#include "Defs.h"
#include "Log.h"

#include "Player.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    mapFileName = config.child("mapfile").attribute("path").as_string();
    mapFolder = config.child("mapfolder").attribute("path").as_string();
    return ret;
}

void Map::Draw()
{
    if(mapLoaded == false)
        return;

    


    //SWITCH DE QUE PANTALLA SE PRINTA
    if (app->scene->actualScene == app->scene->Scenes::GAMEMAP)
    {


        ListItem<MapLayer*>* mapLayerItem;
        mapLayerItem = mapData.maplayers.start;

        while (mapLayerItem != NULL) 
        {

            //L06: DONE 7: use GetProperty method to ask each layer if your “Draw” property is true.
            if (mapLayerItem->data->properties.GetProperty("Draw") != NULL && mapLayerItem->data->properties.GetProperty("Draw")->value) 
            {

                

                for (int x = 0; x < mapLayerItem->data->width; x++)
                {
                    for (int y = 0; y < mapLayerItem->data->height; y++)
                    {
                        // L05: DONE 9: Complete the draw function
                        int gid = mapLayerItem->data->Get(x, y);

                        //L06: DONE 3: Obtain the tile set using GetTilesetFromTileId
                        TileSet* tileset = GetTilesetFromTileId(gid);

                        SDL_Rect r = tileset->GetTileRect(gid);
                        iPoint pos = MapToWorld(x, y);

                        app->render->DrawTexture(tileset->texture,
                            pos.x,
                            pos.y,
                            &r);
                    }
                }
            }

            

            mapLayerItem = mapLayerItem->next;

        }
    }   



    
}

// L05: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tileWidth;
    ret.y = y * mapData.tileHeight;

    return ret;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int gid) const
{
    SDL_Rect rect = { 0 };
    int relativeIndex = gid - firstgid;

    // L05: DONE 7: Get relative Tile rectangle
    rect.w = tileWidth;
    rect.h = tileHeight;
    rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
    rect.y = margin + (tileWidth + spacing) * (relativeIndex / columns);

    return rect;
}


// L06: DONE 2: Pick the right Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    ListItem<TileSet*>* item = mapData.tilesets.start;
    TileSet* set = NULL;
    while (item)
    {
        set = item->data;
        if (gid < (item->data->firstgid + item->data->tilecount))
        {
            break;
        }
        item = item->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L04: DONE 2: Make sure you clean up any memory allocated from tilesets/map
	ListItem<TileSet*>* item;
	item = mapData.tilesets.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	mapData.tilesets.Clear();

    // L05: DONE 2: clean up all layer data
    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.maplayers.start;

    while (layerItem != NULL)
    {
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    return true;
}

// Load new map
bool Map::Load()
{
    bool ret = true;

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }

    if(ret == true)
    {
        ret = LoadMap(mapFileXML);
    }

    if (ret == true)
    {
        ret = LoadTileSet(mapFileXML);
    }

    // L05: DONE 4: Iterate all layers and load each of them
    if (ret == true)
    {
        ret = LoadAllLayers(mapFileXML.child("map"));
    }
    
    // L07 DONE 3: Create colliders
    // Later you can create a function here to load and create the colliders from the map
    

    ListItem<MapLayer*>* layerCollision; //AYUDA PEDRO!!!
    layerCollision = mapData.maplayers.end;

    //Crear colliders (?)
    while (layerCollision != NULL)
    {
        if (layerCollision->data->properties.GetProperty("Colliders") != NULL && layerCollision->data->properties.GetProperty("Colliders")->value == true)
        {
            for (int x1 = 0; x1 < layerCollision->data->width; x1++) //Preguntar a Pedro porque explota
            {
                for (int y1 = 0; y1 < layerCollision->data->height; y1++)
                {
                    // L05: DONE 9: Complete the draw function
                    int gid = layerCollision->data->Get(x1, y1);
                    

                    //L06: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    TileSet* tileset = GetTilesetFromTileId(gid);

                    SDL_Rect r = tileset->GetTileRect(gid);
                    iPoint pos = MapToWorld(x1, y1);

                    switch (gid)
                    {
                        case 111:
                            
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co = new PhysBody;
                            co = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co);
                        }
                            break;

                        case 112:
                        {
                            int w, h;
                            w = 48;
                            h = 48;

                            PhysBody* co1 = new PhysBody;
                            co1 = app->physics->CreateRectangle(pos.x + w / 6, pos.y + h / 6, w, h, STATIC);
                            collisionadores.Add(co1);
                        }
                            break;

                        case 113:
                        {
                            int w, h;
                            w = 48;
                            h = 48;

                            PhysBody* co2 = new PhysBody;
                            co2 = app->physics->CreateRectangle(pos.x + w / 2 , pos.y + h / 6, w, h, STATIC);
                            collisionadores.Add(co2);
                        }

                            break;

                        case 114:
                        {
                            int w, h;
                            w = 48;
                            h = 48;

                            PhysBody* co3 = new PhysBody;
                            co3 = app->physics->CreateRectangle(pos.x + w / 6, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co3);
                        }
                            break;

                        case 115:
                        {
                            int w, h;
                            w = 48;
                            h = 48;

                            PhysBody* co4 = new PhysBody;
                            co4 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co4);
                        }
                            break;

                        case 116:
                        {
                            int w, h;
                            w = 64;
                            h = 48;

                            PhysBody* co5 = new PhysBody;
                            co5 = app->physics->CreateRectangle(pos.x + w / 4, pos.y + h / 6, w, h, STATIC);
                            collisionadores.Add(co5);
                        }
                            break;

                        case 117:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co6 = new PhysBody;
                            co6 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co6);
                        }
                            break;

                        case 118:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co7 = new PhysBody;
                            co7 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co7);
                    }
                            break;

                        case 119:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co8 = new PhysBody;
                            co8 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co8);
                        }
                            break;

                        case 120:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co9 = new PhysBody;
                            co9 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co9);
                        }
                            break;

                        case 121:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co10 = new PhysBody;
                            co10 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co10);
                        }
                            break;

                        case 122:
                        {
                            int w, h;
                            w = 32;
                            h = 48;

                            PhysBody* co11 = new PhysBody;
                            co11 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 6, w, h, STATIC);
                            collisionadores.Add(co11);
                        }
                            break;

                        case 123:
                        {
                            int w, h;
                            w = 48;
                            h = 32;

                            PhysBody* co12 = new PhysBody;
                            co12 = app->physics->CreateRectangle(pos.x + w / 6, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co12);
                        }
                            break;

                        case 124:
                        {
                            int w, h;
                            w = 32;
                            h = 48;

                            PhysBody* co13 = new PhysBody;
                            co13 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co13);
                        }
                            break;

                        case 125:
                        {
                            int w, h;
                            w = 48;
                            h = 32;

                            PhysBody* co14 = new PhysBody;
                            co14 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co14);
                        }
                            break;

                        case 126:
                        {
                            int w, h;
                            w = 64;
                            h = 48;

                            PhysBody* co15 = new PhysBody;
                            co15 = app->physics->CreateRectangle(pos.x + w / 4, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co15);
                        }
                            break;

                        case 127:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co16 = new PhysBody;
                            co16 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co16);
                        }
                            break;

                        case 128:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co17 = new PhysBody;
                            co17 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co17);
                        }
                            break;

                        case 129:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co18 = new PhysBody;
                            co18 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co18);
                        }
                            break;

                        case 130:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co19 = new PhysBody;
                            co19 = app->physics->CreateRectangle(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            collisionadores.Add(co19);
                        }
                            break;             
                    }
                        


                }
            }
        }

        layerCollision = layerCollision->next;

    }

 // Collider 1
    {
        int x, y, w, h; x = 0; y = 656; w = 272; h = 208;
        PhysBody* c1 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c1->ctype = ColliderType::PLATFORM;
    }

    // Collider 2 (PARED MAPA)
    {
        int x, y, w, h; x = 0; y = 0; w = 2; h = 768;
        PhysBody* c2 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c2->ctype = ColliderType::UNKNOWN;
    }
    // Collider 3
    {
        int x, y, w, h; x = 272; y = 562; w = 160; h = 272;
        PhysBody* c3 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c3->ctype = ColliderType::PLATFORM;
    }
    // Collider 4
    {
        int x, y, w, h; x = 528; y = 562; w = 160; h = 272;
        PhysBody* c4 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c4->ctype = ColliderType::PLATFORM;
    }
    // Collider 5
    {
        int x, y, w, h; x = 688; y = 688; w = 320; h = 80;
        PhysBody* c5 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c5->ctype = ColliderType::PLATFORM;
    }
    // Collider 6
    {
        int x, y, w, h; x = 1104; y = 656; w = 336; h = 208;
        PhysBody* c6 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c6->ctype = ColliderType::PLATFORM;
    }
    // Collider 7 (PARED MAPA)
    {
        int x, y, w, h; x = 1440; y = 0; w = 2; h = 768;
        PhysBody* c7 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c7->ctype = ColliderType::UNKNOWN;
    }
    // Collider 8 (plataforma compuesta P1 Derecha)
    {
        int x, y, w, h; x = 1168; y = 496; w = 128; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c8 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c8->ctype = ColliderType::PLATFORM;
    }
    // Collider 9 (plataforma compuesta P1 Derecha)
    {
        int x, y, w, h; x = 1104; y = 464; w = 96; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c9 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c9->ctype = ColliderType::PLATFORM;
    }
    // Collider 10 (plataforma compuesta P1 Derecha-1)
    {
        int x, y, w, h; x = 976; y = 464; w = 64; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c10 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c10->ctype = ColliderType::PLATFORM;
    }
    // Collider 11 (plataforma compuesta P1 Derecha-1)
    {
        int x, y, w, h; x = 912; y = 432; w = 96; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c11 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c11->ctype = ColliderType::PLATFORM;
    }
    // Collider 12 (plataforma compuesta P1 Derecha-2)
    {
        int x, y, w, h; x = 752; y = 432; w = 96; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c12 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c12->ctype = ColliderType::PLATFORM;
    }
    // Collider 13 (plataforma compuesta P1 Derecha-2)
    {
        int x, y, w, h; x = 624; y = 400; w = 160; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c13 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c13->ctype = ColliderType::PLATFORM;
    }
    // Collider 14 (plataforma compuesta P1 Derecha-2)
    {
        int x, y, w, h; x = 432; y = 368; w = 224; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c14 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c14->ctype = ColliderType::PLATFORM;
    }
    // Collider 15 (plataforma compuesta P1 Derecha-2)
    {
        int x, y, w, h; x = 240; y = 336; w = 224; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c15 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c15->ctype = ColliderType::PLATFORM;
    }
    // Collider 16 (plataforma compuesta P1 Derecha-2)
    {
        int x, y, w, h; x = 144; y = 304; w = 128; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c16 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c16->ctype = ColliderType::PLATFORM;
    }
    // Collider 17 (plataforma compuesta P2 izquierda)
    {
        int x, y, w, h; x = 304; y = 176; w = 160; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c17 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c17->ctype = ColliderType::PLATFORM;
    }
    // Collider 18 (plataforma compuesta P2 izquierda)
    {
        int x, y, w, h; x = 464; y = 144; w = 64; h = 88; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c18 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c18->ctype = ColliderType::PLATFORM;
    }
    // Collider 19 (plataforma compuesta P2 izquierda+1)
    {
        int x, y, w, h; x = 592; y = 176; w = 96; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c19 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c19->ctype = ColliderType::PLATFORM;
    }
    // Collider 20 (plataforma compuesta P2 izquierda+2)
    {
        int x, y, w, h; x = 752; y = 144; w = 64; h = 88; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c20 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c20->ctype = ColliderType::PLATFORM;
    }
    // Collider 21 (plataforma compuesta P2 izquierda+2)
    {
        int x, y, w, h; x = 816; y = 176; w = 160; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c21 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c21->ctype = ColliderType::PLATFORM;
    }
    // Collider 22 (plataforma compuesta P2 izquierda+2)
    {
        int x, y, w, h; x = 944; y = 208; w = 352; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c22 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c22->ctype = ColliderType::PLATFORM;
    }
    // Collider 23 (plataforma compuesta P2 izquierda+2)
    {
        int x, y, w, h; x = 1264; y = 240; w = 352; h = 56; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c23 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c23->ctype = ColliderType::PLATFORM;
    }
    // Collider 24 (Techo)
    {
        int x, y, w, h; x = 0; y = 0; w = 1440; h = 72; //h no divisor de 16 porque tiene unos pocos pixeles mas el reborde de abajo
        PhysBody* c24 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
        // L07 DONE 7: Assign collider type
        c24->ctype = ColliderType::UNKNOWN;
    }
    //Collider 25 (pozos de muerte)
    {
       int x, y, w, h; x = 0; y = 800; w = 1440; h = 8;
       PhysBody* c25 = app->physics->CreateRectangleSensor(x + w / 2, y + h / 2, w, h, STATIC);
       c25->ctype = ColliderType::DEAD;
    }
    //Collider 26 (Puerta salida)
    {
        int x, y, w, h; x = 1396; y = 192; w = 32; h = 48;
        PhysBody* c25 = app->physics->CreateRectangleSensor(x + w / 2, y + h / 2, w, h, STATIC);
        c25->ctype = ColliderType::WIN;
    }
    

    if(ret == true)
    {
        // L04: DONE 5: LOG all the data loaded iterate all tilesets and LOG everything
       
        LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
        LOG("width : %d height : %d",mapData.width,mapData.height);
        LOG("tile_width : %d tile_height : %d",mapData.tileWidth, mapData.tileHeight);
        
        LOG("Tilesets----");

        ListItem<TileSet*>* tileset;
        tileset = mapData.tilesets.start;

        while (tileset != NULL) {
            LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
            LOG("tile width : %d tile height : %d", tileset->data->tileWidth, tileset->data->tileHeight);
            LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
            tileset = tileset->next;
        }

        // L05: DONE 4: LOG the info for each loaded layer
        ListItem<MapLayer*>* mapLayer;
        mapLayer = mapData.maplayers.start;

        while (mapLayer != NULL) {
            LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
            LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
            mapLayer = mapLayer->next;
        }
    }

    if(mapFileXML) mapFileXML.reset();

    mapLoaded = ret;

    return ret;
}

// L04: DONE 3: Implement LoadMap to load the map properties
bool Map::LoadMap(pugi::xml_node mapFile)
{
    bool ret = true;
    pugi::xml_node map = mapFile.child("map");

    if (map == NULL)
    {
        LOG("Error parsing map xml file: Cannot find 'map' tag.");
        ret = false;
    }
    else
    {
        //Load map general properties
        mapData.height = map.attribute("height").as_int();
        mapData.width = map.attribute("width").as_int();
        mapData.tileHeight = map.attribute("tileheight").as_int();
        mapData.tileWidth = map.attribute("tilewidth").as_int();
    }

    return ret;
}

// L04: DONE 4: Implement the LoadTileSet function to load the tileset properties
bool Map::LoadTileSet(pugi::xml_node mapFile){

    bool ret = true; 

    pugi::xml_node tileset;
    for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();

        // L04: DONE 4: Load Tileset attributes
        set->name = tileset.attribute("name").as_string();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->margin = tileset.attribute("margin").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->tileWidth = tileset.attribute("tilewidth").as_int();
        set->tileHeight = tileset.attribute("tileheight").as_int();
        set->columns = tileset.attribute("columns").as_int();
        set->tilecount = tileset.attribute("tilecount").as_int();

        // L04: DONE 4: Load Tileset image
        SString tmp("%s%s", mapFolder.GetString(), tileset.child("image").attribute("source").as_string());
        set->texture = app->tex->Load(tmp.GetString());

        mapData.tilesets.Add(set);
    }

    return ret;
}

// L05: DONE 3: Implement a function that loads a single layer layer
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
    bool ret = true;

    //Load the attributes
    layer->id = node.attribute("id").as_int();
    layer->name = node.attribute("name").as_string();
    layer->width = node.attribute("width").as_int();
    layer->height = node.attribute("height").as_int();

    //L06: DONE 6 Call Load Propoerties
    LoadProperties(node, layer->properties);

    //Reserve the memory for the data 
    layer->data = new uint[layer->width * layer->height];
    memset(layer->data, 0, layer->width * layer->height);

    //Iterate over all the tiles and assign the values
    pugi::xml_node tile;
    int i = 0;
    for (tile = node.child("data").child("tile"); tile && ret; tile = tile.next_sibling("tile"))
    {
        layer->data[i] = tile.attribute("gid").as_int();
        i++;
    }

    return ret;
}

// L05: DONE 4: Iterate all layers and load each of them
bool Map::LoadAllLayers(pugi::xml_node mapNode) {
    bool ret = true;

    for (pugi::xml_node layerNode = mapNode.child("layer"); layerNode && ret; layerNode = layerNode.next_sibling("layer"))
    {
        //Load the layer
        MapLayer* mapLayer = new MapLayer();
        ret = LoadLayer(layerNode, mapLayer);

        //add the layer to the map
        mapData.maplayers.Add(mapLayer);
    }

    return ret;
}

// L06: DONE 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.list.Add(p);
    }

    return ret;
}


// L06: DONE 7: Ask for the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* item = list.start;
    Property* p = NULL;

    while (item)
    {
        if (item->data->name == name) {
            p = item->data;
            break;
        }
        item = item->next;
    }

    return p;
}


