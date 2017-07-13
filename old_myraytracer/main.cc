//
// 2009 jesssoft
//
// main.cc: Defines the entry point for the application.
//
#include <iostream>
#include "main.h"
#include "surface.h"
#include "typedefs.h"
#include "engine.h"
#include "util_imgfile.h"

using namespace std;

//
// Define the const values of screen resolution.
//

// Test
/*
const int kScreenWidth = 320;
const int kScreenHeight = 200;
*/
const int kScreenWidth = 800;
const int kScreenHeight = 600;

// FHD
/*
const int kScreenWidth = 1920;
const int kScreenHeight = 1080;
*/

int 
main(int arc, const char *argv[])
{
	Engine *engine = new Engine();
	
	if (!engine->Init(kScreenWidth, kScreenHeight)) {
		cerr << "Init() failed" << endl;
		delete engine;
		return -1;
	}

	Surface *surface = engine->GetSurface();

	//
	// Set up camera position
	//
	Vector3 pos(0, 0, -18);
	Vector3 target(0, 0.0, 15);
	engine->InitRender(pos, target);

	//
	// Start rendering
	//
	cout << "Rendering gets started." << endl;
	engine->Render();
	cout << "Rendering finished." << endl;

	//
	// Save the image into a tga file
	//
	imgfile_write_tgafile("screen_shot.tga", 
	    surface->GetWidth(),
	    -surface->GetHeight(),
	    reinterpret_cast<unsigned char *>(surface->GetBuffer()));
	cout << "Saved a tga file." << endl;

	//
	// Save the image into a bmp file
	//
	imgfile_write_bmpfile("screen_shot.bmp",
	    surface->GetWidth(),
	    -surface->GetHeight(),
	    reinterpret_cast<unsigned char *>(surface->GetBuffer()));
	cout << "Saved a bmp file." << endl;

	//
	// Finished
	//
	cout << "Finished." << endl;

	//
	// Cleanup
	//
	delete engine;

	return 0;
}
