
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

// L12: Create walkability map for pathfinding
bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;
    ListItem<MapLayer*>* item;
    item = mapData.maplayers.start;

    for (item = mapData.maplayers.start; item != NULL; item = item->next)
    {
        MapLayer* layer = item->data;

        if (!layer->properties.GetProperty("Navigation")->value)
            continue;

        uchar* map = new uchar[layer->width * layer->height];
        memset(map, 1, layer->width * layer->height);

        for (int y = 0; y < mapData.height; ++y)
        {
            for (int x = 0; x < mapData.width; ++x)
            {
                int i = (y * layer->width) + x;

                int tileId = layer->Get(x, y);
                TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL; //PEDRO AYUDA, ESTA LINEA DE CODIGO QUE HACE

                if (tileset != NULL)
                {
                    map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
                }
            }
        }

        *buffer = map;
        width = mapData.width;
        height = mapData.height;
        ret = true;

        break;
    }

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

iPoint Map::WorldToMap(int x, int y)
{
    iPoint ret(0, 0);    
    
        ret.x = x / mapData.tileWidth;
        ret.y = y / mapData.tileHeight;
    
    

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
    

    ListItem<MapLayer*>* layerCollision; 
    layerCollision = mapData.maplayers.end->prev; //PEDRO AYUDA, COMO HALLAR LAYER COLLIDER

    //Crear colliders
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
                            co->ctype = ColliderType::PLATFORM;
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
                            co1->ctype = ColliderType::PLATFORM;
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
                            co2->ctype = ColliderType::PLATFORM;
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
                            co5->ctype = ColliderType::PLATFORM;
                            collisionadores.Add(co5);
                        }
                            break;

                        case 117:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co6 = new PhysBody;
                            co6 = app->physics->CreateRectangleSensor(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            co6->ctype = ColliderType::DEAD;
                            collisionadores.Add(co6);
                        }
                            break;

                        case 118:
                        {
                            int w, h;
                            w = 32;
                            h = 32;

                            PhysBody* co7 = new PhysBody;
                            co7 = app->physics->CreateRectangleSensor(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            co7->ctype = ColliderType::DEAD;
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
                            co11->ctype = ColliderType::PLATFORM;
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
                            co17 = app->physics->CreateRectangleSensor(pos.x + w / 2, pos.y + h / 2, w, h, STATIC);
                            co17->ctype = ColliderType::DEAD;
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


