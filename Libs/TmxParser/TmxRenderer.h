#ifndef _MARMALADE_TMX_RENDERER_H_
#define _MARMALADE_TMX_RENDERER_H_
#include "Iw2D.h"
#include "TmxJsonParser.h"


namespace tmxparser
{


typedef struct TileMappingInfo_t
{
public:
	TileMappingInfo_t(int gid, int tx, int ty) { tilesetIndex=gid; srcOffset.x= tx; srcOffset.y = ty; }

	int			tilesetIndex;
	CIwFVec2	srcOffset;
} TileMappingInfo_t;

typedef std::vector<TileMappingInfo_t>		TileMappingCollection_t;


class TmxRenderer
{
public:
	TmxRenderer(Map_t* map);
	~TmxRenderer();
	
	// render without resizing tiles
	virtual void Render(CIwFVec2 offset);

	// render each tile, scaled to a new cell size
	virtual void Render(CIwFVec2 offset, CIwFVec2 destCellSize);

	// 0,0 destCellSize renders at map tile size
	virtual void RenderLayer(int layerIdx, CIwFVec2 offset, CIwFVec2 destCellSize);

private:
	TmxRenderer() {}

	void LoadResources();
	void DestroyResources();
	void Precalculate();
	bool isTileVisible(CIwFVec2 destination);

	TileMappingCollection_t		gidTable; // to translate gid's to tileIndex,tile Location pairs,

	CIw2DImage** TilesetImages;
	int numSets;

	int displayWidth;
	int displayHeight;
	
	CIwFVec2 displayTileSize;

	Map_t* map;
};

}
#endif 
