
#include "TmxJsonParser.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include "cJSON.h"
#include <s3eFile.h>

namespace tmxparser
{

_STL::string GetChildValue_string(cJSON* parent, const char* childName)
{
	cJSON* child = cJSON_GetObjectItem(parent,childName);
	if (child)
		return child->valuestring;
	return "";
}

int GetChildValue_int(cJSON* parent, const char* childName, int defaultValue)
{
	cJSON* child = cJSON_GetObjectItem(parent,childName);
	if (child)
		return child->valueint;
	return defaultValue;
}

double GetChildValue_double(cJSON* parent, const char* childName, double defaultValue)
{
	cJSON* child = cJSON_GetObjectItem(parent,childName);
	if (child)
		return child->valuedouble;
	return defaultValue;
}

void json_parsePropertyNode(cJSON *parent, PropertyMap_t* outPropertyMap)
{
	cJSON* propList = cJSON_GetObjectItem(parent, "properties");
	if (propList==NULL)
		return;

	cJSON* item = cJSON_GetObjectItem(propList, "property");
	while(item)
	{
		if (strcmp(item->string, "property") == 0)
		{
			cJSON* name = cJSON_GetObjectItem(item,"name");
			cJSON* value = cJSON_GetObjectItem(item,"value");
			if (name!=NULL && value!= NULL)
			{
				(*outPropertyMap)[name->valuestring] = value->valuestring;
			}
		}
		item = item->next;
	}
}

void json_parsePointsArray(cJSON *item, PointCollection_t* outPoints)
{
	cJSON* point = item->child;
	
	while(point)
	{
		Point_t p(	 cJSON_GetObjectItem(point, "x")->valueint
					,cJSON_GetObjectItem(point, "y")->valueint );
		
		outPoints->push_back( p );

		point = point->next;
	}
}

void json_parseObjectNode(cJSON *item, Object_t* outObj)
{
	outObj->name = GetChildValue_string(item, "name");;
	outObj->type = GetChildValue_string(item, "type");

	outObj->x = GetChildValue_int(item, "x", 0);
	outObj->y = GetChildValue_int(item, "y", 0);
	outObj->width		= GetChildValue_int(item, "width", 0);
	outObj->height		= GetChildValue_int(item, "height", 0);
	outObj->rotation	= GetChildValue_double(item, "rotation", 0.0);
	outObj->visible		= GetChildValue_int(item, "visible", 1);
	outObj->referenceGid = GetChildValue_int(item, "gid", 0);

	json_parsePropertyNode(item, &outObj->propertyMap);

	cJSON* polygon = cJSON_GetObjectItem(item, "polygon");
	cJSON* polyline = cJSON_GetObjectItem(item, "polyline");

	if (cJSON_GetObjectItem(item, "ellipse")!=NULL)
		outObj->shapeType = kEllipse;
	else if (polygon!=NULL)
	{
		outObj->shapeType = kPolygon;
		json_parsePointsArray(polygon, &outObj->shapePoints);
	}
	else if (polyline!=NULL)
	{
		outObj->shapeType = kPolyline;
		json_parsePointsArray(polyline, &outObj->shapePoints);
	}
	else if (outObj->referenceGid>0)
		outObj->shapeType = kSprite;
	else	outObj->shapeType = kSquare;
}

void json_parseLayer(cJSON *item, Layer_t* outLayer)
{
	outLayer->name = cJSON_GetObjectItem(item, "name")->valuestring;
	outLayer->opacity = GetChildValue_double(item, "opacity", 0.0);
	outLayer->visible = GetChildValue_int(item, "visible", 1);
	outLayer->x = GetChildValue_int(item, "x", 0);
	outLayer->y = GetChildValue_int(item, "y", 0);
	outLayer->width = GetChildValue_int(item, "width", 0);
	outLayer->height = GetChildValue_int(item, "height", 0);

	json_parsePropertyNode(item, &outLayer->propertyMap);
}

void json_parseLayerTiles(cJSON *item, Layer_t* outLayer)
{
	cJSON* dataElement = cJSON_GetObjectItem(item, "data");
	if (dataElement != NULL)
	{
		cJSON* tile = dataElement->child;
		while(tile)
		{
			outLayer->tiles.push_back( tile->valueint );
			tile = tile->next;
		}
	}
}

void json_parseLayerObjects(cJSON *item, Layer_t* outLayer)
{
	cJSON* objectsArray = cJSON_GetObjectItem(item, "objects");
	cJSON* objNode = objectsArray->child;
	while(objNode)
	{
		Object_t obj;
		json_parseObjectNode(objNode, &obj);
		outLayer->objects.push_back(obj);

		objNode = objNode->next;
	}
}

void json_parseTilesetNode(cJSON *item, Tileset_t* outTileset)
{
	outTileset->firstgid = GetChildValue_int(item, "firstgid", -1);
	outTileset->tileWidth = GetChildValue_int(item, "tilewidth", 0);
	outTileset->tileHeight = GetChildValue_int(item, "tileheight", 0);
	outTileset->imageWidth = GetChildValue_int(item,"imagewidth", 0);
	outTileset->imageHeight = GetChildValue_int(item,"imageheight", 0);
	outTileset->tileSpacingInImage = GetChildValue_int(item, "spacing", 0);
	outTileset->tileMarginInImage = GetChildValue_int(item, "margin", 0);

	outTileset->name = GetChildValue_string(item, "name");
	outTileset->imagePath = GetChildValue_string(item, "image");
}


void json_parseMapNode(cJSON *item, Map_t* outMap)
{
	if (item == NULL)		return;

	outMap->version = GetChildValue_int(item,"version", 0);
	outMap->orientation = GetChildValue_string(item,"orientation");
	outMap->width = cJSON_GetObjectItem(item,"width")->valueint;
	outMap->height = cJSON_GetObjectItem(item,"height")->valueint;
	outMap->tileWidth = cJSON_GetObjectItem(item,"tilewidth")->valueint;
	outMap->tileHeight = cJSON_GetObjectItem(item,"tileheight")->valueint;
	outMap->backgroundColor = GetChildValue_string(item,"backgroundcolor");

	json_parsePropertyNode(item, &outMap->propertyMap);

	cJSON* tilesets = cJSON_GetObjectItem(item,"tilesets");
	if (tilesets)
	{
		cJSON* tileset = tilesets->child;
		while(tileset)
		{
			Tileset_t set;
			json_parseTilesetNode(tileset, &set);
			outMap->tilesets.push_back(set);
			tileset = tileset->next;
		}
	}

	cJSON* layers = cJSON_GetObjectItem(item,"layers");
	if (layers)
	{
		cJSON* layer  = layers->child;
		while(layer)
		{
			Layer_t tmxLayer;
			json_parseLayer(layer, &tmxLayer);

			_STL::string typeStr = GetChildValue_string(layer, "type");
			if (typeStr=="tilelayer")				json_parseLayerTiles(layer, &tmxLayer);
			else if (typeStr=="objectgroup")		json_parseLayerObjects(layer, &tmxLayer);

			outMap->layers.push_back(tmxLayer);
			layer = layer->next;
		}
	}
}

void parseTmxFromJSON_file_memory(const char* stringData, size_t length, Map_t* outMap)
{
	cJSON *mapNodes = cJSON_Parse(stringData);

    if (mapNodes!=NULL)
		json_parseMapNode(mapNodes, outMap);
	cJSON_Delete(mapNodes);
}

void parseTmxFromJSON_file(const std::string& filename, Map_t* outMap)
{
	s3eFile* file = s3eFileOpen(filename.c_str(), "r");
	if (file)
	{
		int32 size = s3eFileGetSize(file);
		char* buffer = new char[size+1];

		uint32 bytes = s3eFileRead( buffer, 1, size, file);
		buffer[bytes] = 0; // null terminate string
	
		parseTmxFromJSON_file_memory(buffer, size, outMap);
		delete buffer;
		s3eFileClose(file);
	}
}


}
