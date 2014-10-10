#ifndef _LIB_TMX_PARSER_H_
#define _LIB_TMX_PARSER_H_

#include <string>
#include <map>
#include <vector>


namespace tmxparser
{

typedef enum {	kOrthogonal, kIsometric, kStaggered } Orientation_e;

typedef enum { kPolygon, kPolyline,	kEllipse, kSquare, kSprite } ShapeType_e;

typedef std::map<std::string, std::string> PropertyMap_t;

typedef struct
{
	std::string name;
	std::string value;
} Property_t;


typedef struct
{
	unsigned int firstgid;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int imageWidth;
	unsigned int imageHeight;
	unsigned int tileSpacingInImage;
	unsigned int tileMarginInImage;

	std::string name;
	std::string imagePath;
} Tileset_t;

typedef std::vector<Tileset_t> TilesetCollection_t;


// Layers -----------------------
typedef struct Point_t
{
	Point_t(int px, int py)		{ x=px; y=py; }

	int x;
	int y;
} Point_t;

typedef std::vector<Point_t> PointCollection_t;


typedef struct
{
	std::string name;
	std::string type;
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	float rotation;
	unsigned int referenceGid;
	bool visible;

	ShapeType_e			shapeType;
	PointCollection_t	shapePoints;

	PropertyMap_t		propertyMap;
} Object_t;


typedef std::vector<Object_t> ObjectCollection_t;


typedef std::vector<unsigned int> TileArray_t;

typedef struct
{
	std::string name;
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	float opacity;
	bool visible;

	TileArray_t tiles;
	ObjectCollection_t objects;

	PropertyMap_t propertyMap;
} Layer_t;


typedef std::vector<Layer_t> LayerCollection_t;


typedef struct
{
	unsigned int version; // tilemap saves it as 1 , not as "1" , so its not a string
	std::string orientation;
	unsigned int width;
	unsigned int height;
	unsigned int tileWidth;
	unsigned int tileHeight;
	std::string backgroundColor;

	TilesetCollection_t tilesets;
	LayerCollection_t	layers;

	PropertyMap_t propertyMap;
} Map_t;

void parseTmxFromJSON_file(const std::string& filename, Map_t* outMap);
void parseTmxFromJSON_file_memory(const char* stringData, size_t length, Map_t* outMap);

}
#endif /* _LIB_TMX_PARSER_H_ */
