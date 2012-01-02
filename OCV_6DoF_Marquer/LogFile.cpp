/*
	Daniel Gallardo Grassot
	daniel.gallardo@upf.edu
	Barcelona 2011

	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
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