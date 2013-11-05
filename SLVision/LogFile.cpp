/*
 * Copyright (C) 2011-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of SLVision
 *
 * SLVision is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/*
 *	Daniel Gallardo Grassot
 *	daniel.gallardo@upf.edu
 *	Barcelona 2011
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
