/*
 * Daniel Gallardo Grassot daniel.gallardo@upf.edu
 * Barcelona 2011
 */

#include "LogFile.h"
#include <fstream>
#include <time.h>

using namespace std;
using namespace datasaver;

bool LogFile::start = false;
time_t LogFile::rawtime = 0;

void LogFile::Add(string text)
{
	ofstream log;
	log.open(LOG_PATH,ios::out | ios::app);
	time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);

	if(!start)
	{
		log << "######################\n";
		log << "##" 
			<< timeinfo->tm_mday << "/" 
			<< timeinfo->tm_mon << "/"
			<< 1900+timeinfo->tm_year << "  " 
			<< timeinfo->tm_hour << ":"
			<< timeinfo->tm_min << ":"
			<< timeinfo->tm_sec << "##\n";
		log << "######################\n";
		start = true;
	}
	log << timeinfo->tm_mday << "/" 
		<< timeinfo->tm_mon << "/"
		<< 1900+timeinfo->tm_year << "  " 
		<< timeinfo->tm_hour << ":"
		<< timeinfo->tm_min << ":"
		<< timeinfo->tm_sec << "  --  ";
	log << text << "\n";
	log.close();
}