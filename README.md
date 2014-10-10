Marmalade-TMX-json-parser-renderer
==================================

This library will parse tmx map files exported in Json. A renderer is also included for marmalade, using Iw2d.

It uses the cJson library from Dave Gamble ( http://sourceforge.net/projects/cjson/ )

Json was selected instead of the standard xml-based Tmx file because it results in shorter files (when compared to tmx format without zlib/base64 encoding. So the zlib dependency is also redundant when using json.
If you are using Derbh in Marmalade, then the json file will automaticaly  get compressed anyways.


Simple Usage:

1. Load the map:

	tmxparser::Map_t *map = new tmxparser::Map_t;

	tmxparser::parseTmxFromJSON_file("testLevel.json", map);
 
2. Create a renderer (for marmalade):

	tmxparser::TmxRenderer *renderer = new tmxparser::TmxRenderer(map);

3. In the main loop call the renderer to draw the map (passing a given offset location)

  renderer->Render(offset);
  
  Or render at a scale:
  
  //renderer->Render(offset, CIwFVec2(64.0f, 64.0f)); 
  
  Or render only a layer of the map:
  
  //renderer->RenderLayer(0, offset, CIwFVec2(0.0f, 0.0f));
  
  
  The renderer clips toe tiles that are outside the display. Look at the IsTileVisible() method if you need to change the cliping area.
  
  The example provided uses fairly large Tiles (128x128) which consumes memory in marmalade.
  
  
  
