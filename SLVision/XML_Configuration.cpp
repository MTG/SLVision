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

#include "XML_Configuration.h"
#include "LogFile.h"

using namespace std;
using namespace datasaver;

typedef std::pair<std::string,std::string> string_pair;

XML_Configuration::XML_Configuration(void)
{
	values.clear();
	path_list.clear();
}

XML_Configuration::~XML_Configuration(void)
{
}

bool XML_Configuration::LoadXMLFile(std::string path)
{
	string line;
	ifstream file (path.c_str(), ifstream::in);
	if(file.is_open())
	{
		string temp;
		while(file.good())
		{
			getline(file,line);
			if(line.size() > 0)
			{
				int tag_begin = line.find('<');
				int tag_end = line.find('>');
				temp = line.substr(tag_begin+1, (tag_end-tag_begin)-1 );
				if(temp[0] == '/')
					path_list.remove(path_list.back());
				else
					path_list.push_back(temp);
			
				int tag_last = line.find('<',tag_begin+1);
				if(tag_last != string::npos)
				{
					temp = line.substr(tag_end+1, (tag_last-tag_end)-1);
					//cout << PathToString() << " -> " << temp << endl;
					//cout << PathToString() << " -> " << temp << endl;
					//cout << PathToString() << " -> " << temp << endl;
					values[PathToString()] = temp;
					tag_end = line.find('>',tag_last);
					path_list.remove(path_list.back());
				}
			}
		}
		file.close();
		return true;
	}
	else
		LogFile::Add("Unable to load configuration file from: "+path);
	return false;
}

std::string XML_Configuration::PathToString()
{
	string toreturn= "";
	for(string_list::iterator it = path_list.begin(); it != path_list.end();it++)
	{
		if(it == path_list.begin()) toreturn = (*it);
		else toreturn += ":"+(*it);
	}
	return toreturn;
}

bool compare_pair(string_pair first, string_pair second)
{
	unsigned int i = 0;

	while ( (i<first.first.length()) && (i<second.first.length()) )
	{
		if (tolower(first.first[i])<tolower(second.first[i])) return true;
		else if (tolower(first.first[i])>tolower(second.first[i])) return false;
		++i;
	}
	if (first.first.length()<second.first.length()) return true;
	else return false;
}

void XML_Configuration::SaveXMLFile(std::string path)
{
	ofstream file;
	file.open(path.c_str(),ios::out);
	if(!file.is_open())
	{
		LogFile::Add("Error opening "+path+" for saving");
		return;
	}

	list<string_pair> list_values;

	for(map_values::iterator it = values.begin(); it != values.end(); it++)
	{
		list_values.push_back(*it);
	}

	list_values.sort(compare_pair);
	path_list.clear();

	for(list<string_pair>::iterator it = list_values.begin(); it != list_values.end(); it++)
	{
		Wrtie(it->first, it->second,file);
	}
	WritePop(file, -1);

	file.close();
}

void XML_Configuration::Wrtie(std::string path, std::string value,  ofstream &file)
{
	string_list actual = GetPath(path);
	unsigned int i = 0;
	string_list::iterator it_path = path_list.begin();
	string_list::iterator it_actual = actual.begin();
	unsigned int max = path_list.size();
	
	if(max > actual.size()) 
		max = actual.size();

	for(; i < max; i++)
	{
		if( it_path->compare( *it_actual)== 0)
		{
			it_path++;
			it_actual++;
		}
		else
			break;
	}

	int q = path_list.size() -i;
	if(q > 0) WritePop(file, q);
	for(; it_actual != actual.end(); it_actual++)
	{
		WritePush(file, *it_actual);
	}

	file << value ;
	WritePop(file);
}

void XML_Configuration::WritePop(std::ofstream &file, int n)
{
	if( n == -1)
	{
		n = path_list.size();
		if(n==0) return;
	}
	for (int i = 0; i<n ; i++)
	{
		file << "\n";
		for(int k = 0; k < (int)path_list.size()-1; k++)
			file << "\t";
		file << "</" << path_list.back() <<">\n";
		path_list.pop_back();
	}
	if(n == 0)
	{
		file << "</" << path_list.back() <<">";
		path_list.pop_back();
	}
}

void XML_Configuration::WritePush(std::ofstream &file, std::string tag)
{
	path_list.push_back(tag);
	if(path_list.size()>1)file << "\n";
	for(int k = 0; k < (int)path_list.size()-1; k++)
		file << "\t";
	file << "<"<<tag <<">";
}

string_list XML_Configuration::GetPath(std::string data)
{
	string_list toreturn;
	int previous = 0;
	int checkpoint = data.find(':');
	while(checkpoint != string::npos)
	{
		if(toreturn.size() == 0)
			toreturn.push_back(data.substr(previous, checkpoint-previous));
		else 
			toreturn.push_back(data.substr(previous+1, (checkpoint-previous)-1 ));
		previous = checkpoint;
		checkpoint = data.find(':',checkpoint+1);
	}
	if(toreturn.size() == 0)
		toreturn.push_back(data.substr(previous, data.length()-previous));
	else 
		toreturn.push_back(data.substr(previous+1, data.length()-previous));
	return toreturn;
}

void XML_Configuration::SetValue(const std::string& tag, int value)
{
	char string_value[255];
	sprintf_s(string_value, "%i", value);
	values[tag] = string_value;
}

void XML_Configuration::SetValue(const std::string& tag, float value)
{
	char string_value[255];
	sprintf_s(string_value, "%f", value);
	values[tag] = string_value;
}

void XML_Configuration::SetValue(const std::string& tag, double value)
{
	char string_value[255];
	sprintf_s(string_value, "%d", value);
	values[tag] = string_value;
}

void XML_Configuration::SetValue(const std::string& tag, const std::string& value)
{
	values[tag] = value;
}

int XML_Configuration::GetValue(const std::string& tag, int defaultValue)
{
	if(values.find(tag) != values.end())
	{
		return strtol(values[tag].c_str(), NULL, 0);
	}
	else
		return defaultValue;
}

float XML_Configuration::GetValue(const std::string& tag, float defaultValue)
{
	if(values.find(tag) != values.end())
	{
		return (float)strtod(values[tag].c_str(), 0);
	}
	else
		return defaultValue;
}

double XML_Configuration::GetValue(const std::string& tag, double defaultValue)
{
	if(values.find(tag) != values.end())
	{
		return strtod(values[tag].c_str(), 0);
	}
	else
		return defaultValue;
}

std::string XML_Configuration::GetValue(const std::string& tag, const std::string defaultValue)
{
	if(values.find(tag) != values.end())
	{
		return values[tag];
	}
	else
		return defaultValue;
}