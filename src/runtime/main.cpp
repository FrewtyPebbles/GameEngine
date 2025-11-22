//*****************************************
// This is the entry point for the runtime
//*****************************************

#include <iostream>
#include <iomanip>
#include "Engine/render_backends/progressive/progressive_render_backend.h"

using std::cout, std::endl;

int main(int argc, char** argv)
{
	cout << "Runtime starting..." << endl;

	// TODO: Replace this code once game-data loading logic is available.
	// For now just create a test window.
	// Game-data loading logic will choose the render backend and load in the initial scene and node state.

	ProgressiveRenderBackend backend = ProgressiveRenderBackend();
	
	backend.start_window("Test Window", 600, 600);

	return 0;
}
