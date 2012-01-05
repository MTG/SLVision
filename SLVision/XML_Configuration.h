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

#pragma once
#include <string>
#include <map>
#include <list>
#include <fstream>

namespace datasaver
{
	typedef std::map<std::string,std::string> map_values;
	typedef std::list<std::string> string_list;
	
	class XML_Configuration
	{
	private:
		map_values values;
		//std::string data_path;
		std::list<std::string> path_list;
		string_list GetPath(std::string data);
		std::string PathToString();
		void Wrtie(std::string path, std::string value, std::ofstream &file);
		void WritePop(std::ofstream &file, int n = 0);
		void WritePush(std::ofstream &file, std::string tag);
	public:
		XML_Configuration(void);
		~XML_Configuration(void);
		bool LoadXMLFile(std::string path);
		void SaveXMLFile(std::string path);
		void SetValue(const std::string& tag, int value);
		void SetValue(const std::string& tag, float value);
		void SetValue(const std::string& tag, double value);
		void SetValue(const std::string& tag, const std::string& value);
		int GetValue(const std::string& tag, int defaultValue);
		float GetValue(const std::string& tag, float defaultValue);
		double GetValue(const std::string& tag, double defaultValue);
		std::string GetValue(const std::string& tag, const std::string defaultValue);
	};
}

