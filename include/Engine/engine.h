#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class RenderBackend;
class Logger;

namespace Tritium {

	class Engine {
	public:
		Engine(
			RenderBackend* render_backend,
			Logger* logger,
			string application_name,
			string application_description,
			vector<string> application_authors,
			int application_version_major,
			int application_version_minor,
			int application_version_patch,
			string application_version_identifier
		);

		// Functions

		void start_window(string window_title, uint32_t window_width, uint32_t window_height);

		// METADATA
		RenderBackend* render_backend;
		Logger* logger;
		string application_name;
		string application_description;
		vector<string> application_authors;
		int application_version_major;
		int application_version_minor;
		int application_version_patch;
		string application_version_identifier;
	};
}