/*
 * Daniel Gallardo Grassot daniel.gallardo@upf.edu
 * Barcelona 2011
 */

#pragma once
#include <string>

namespace datasaver
{
	#define LOG_PATH "log.txt"

	class LogFile
	{
	private:
		static bool start;
		static time_t rawtime;
	public:
		static void Add(std::string text);
	};
}