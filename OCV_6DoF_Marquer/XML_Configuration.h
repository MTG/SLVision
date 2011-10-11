/*
 * Daniel Gallardo Grassot daniel.gallardo@upf.edu
 * Barcelona 2011
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

