//*****************************************
// This is the entry point for the runtime
//*****************************************

#include <iostream>
#include <iomanip>

#include "Engine/constants.h"
#include "Engine/engine.h"
#include <stdexcept>
#include "Engine/logging/logger.h"

#ifdef RENDER_BACKEND_PROGRESSIVE
#include "Engine/render_backends/progressive/progressive_render_backend.h"
#endif // RENDER_BACKEND_PROGRESSIVE

#ifdef RENDER_BACKEND_COMPATIBILITY
#include "Engine/render_backends/compatibility/compatibility_render_backend.h"
#endif // RENDER_BACKEND_COMPATIBILITY

using std::cout, std::endl;

int main(int argc, char** argv)
{
	cout << "Runtime starting...  " << ENGINE_NAME << " Engine v" << GET_ENGINE_VERSION() << endl;

	// TODO: Replace this code once game-data loading logic is available.
	// For now just create a test window.
	// Game-data loading logic will choose the render backend and load in the initial scene and node state.
	try {
		// Log pipes must be added to vector this way to avoid copying
		// since the file pointer in log pipes cant be copied
		vector<LogPipe*> logPipes;
		logPipes.reserve(2);

		LogPipe logPipeRendering("./.logs/rendering.log", "rendering");
		LogPipe logPipeUser("./.logs/user.log", "user");

		logPipes.push_back(&logPipeRendering);
		logPipes.push_back(&logPipeUser);

		Logger logger = Logger(logPipes);

		// Create/start backend
#ifdef RENDER_BACKEND_PROGRESSIVE
		ProgressiveRenderBackend renderBackend = ProgressiveRenderBackend(
			nullptr
		);
#endif // RENDER_BACKEND_PROGRESSIVE

#ifdef RENDER_BACKEND_COMPATIBILITY
		CompatibilityRenderBackend renderBackend = CompatibilityRenderBackend(
			nullptr
		);
#endif // RENDER_BACKEND_COMPATIBILITY

		// Create an engine instance:

		Tritium::Engine engine = Tritium::Engine(
			&renderBackend,
			&logger,
			"TestApp",
			"This is a test app.",
			{ "Jane Doe" },
			0,
			0,
			0,
			"dev"
		);

		engine.start_window(engine.application_name, 600, 600);

		engine.logger->flush();
	}
	catch (std::runtime_error error) {
		std::cerr << "Fatal Error: " << error.what() << "\n";
	}

	return 0;
}
