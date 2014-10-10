#include "s3e.h"
#include "IwDebug.h"
#include "IwGx.h"
#include "Iw2D.h"
#include "TmxJsonParser.h"
#include "TmxRenderer.h"



void Initialize()
{
    //Initialise graphics system(s)
    IwGxInit();
	Iw2DInit();
}

void Terminate()
{
	Iw2DTerminate();
    IwGxTerminate();
}


int main()
{
	Initialize();

	// --HowTo: Load the tmx map from the json file
	tmxparser::Map_t *map = new tmxparser::Map_t;
	tmxparser::parseTmxFromJSON_file("testLevel.json", map);
	// --HowTo: Create a renderer
	tmxparser::TmxRenderer *renderer = new tmxparser::TmxRenderer(map);
	// an offset to use for scrolling the map
	CIwFVec2 offset(0,0);
	
	IwGxSetColClear(0x2f, 0x3f, 0x3f, 0xff);
    
    // Loop forever, until the user or the OS performs some action to quit the app
    while (!s3eDeviceCheckQuitRequest())
    {
        //Update the input systems
        s3eKeyboardUpdate();
        s3ePointerUpdate();

        // Clear the surface
        IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

		// --HowTo: render all layers, at original pixel size:
		//renderer->Render(offset); 

		// --HowTo: render all layers at reduced/scaled tile size
		renderer->Render(offset, CIwFVec2(64.0f, 64.0f)); 
		
		// --HowTo: render only one layer at original pixel size
		//renderer->RenderLayer(0, offset, CIwFVec2(0.0f, 0.0f));

		// --HowTo: render only one layer at scaled pixel size
		//renderer->RenderLayer(0, offset, CIwFVec2(64.0f, 64.0f));

		// advance offset
		offset.x += 3;
		if (offset.x>1900)
			offset.x =0;

        // Standard EGL-style flush of drawing to the surface
        IwGxFlush();
        IwGxSwapBuffers();
        s3eDeviceYield(0);
    }

	delete renderer;
	delete map;

	Terminate();    
    // Return
    return 0;
}
