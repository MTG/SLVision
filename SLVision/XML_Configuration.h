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

