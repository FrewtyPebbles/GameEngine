//*****************************************
// This is the entry point for the runtime
//*****************************************

#include <iostream>
#include <iomanip>
#include "Engine/render_backends/progressive/progressive_render_backend.h"
#include "Engine/constants.h"
#include <stdexcept>

using std::cout, std::endl;

int main(int argc, char** argv)
{
	cout << "Runtime starting...   v" << GET_ENGINE_VERSION() << endl;

	// TODO: Replace this code once game-data loading logic is available.
	// For now just create a test window.
	// Game-data loading logic will choose the render backend and load in the initial scene and node state.
	try {
		ProgressiveRenderBackend backend = ProgressiveRenderBackend(
			"TestApp",
			"This is a test app.",
			{ "Jane Doe" },
			0,
			0,
			0,
			"dev"
		);

		backend.start_window("Test Window", 600, 600);
	}
	catch (std::runtime_error error) {
		std::cerr << "Fatal Error: " << error.what() << "\n";
	}

	return 0;
}
