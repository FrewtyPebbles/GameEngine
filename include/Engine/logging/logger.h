#pragma once
// This class manages and directs log messages to different
// `LogPipe`s.

#include "Engine/logging/log_pipe.h"
#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

namespace Log {
	enum Domain {
		PHYSICS = 0,
		USER = 1,
		RENDERING = 2
	};

	enum Severity {
		INFO = 0,
		WARNING = 1,
		ERROR = 2,
		FATAL = 3
	};
}

class Logger {
public:
	Logger();
	/// Default pipes

	log(string message, string pipe, Log::Domain domain, Log::Severity)
private:
	unordered_map<string, LogPipe> pipe_map;
};