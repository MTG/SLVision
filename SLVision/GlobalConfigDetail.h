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
#include <sstream>
#include <map>
#include "XML_Configuration.h"
#include "Singleton.h"
#include "LogFile.h"

#define XML_PATH "Global_config.xml"

namespace datasaver
{
	class GlobalConfigDetail : Singleton<GlobalConfigDetail>
	{

		class BaseContainer
		{
			public:
			virtual void save(){}
		};

		template<typename T>
		class Container : public BaseContainer
		{
			T value;
			std::string addr;
			public:
			Container(const std::string& k,const T & defaultvalue = T()):addr(k)
			{
				value = GlobalConfigDetail::get<T>(k,defaultvalue);
			}
			T & getRef()
			{
				return value;
			}
			void save()
			{
				GlobalConfigDetail::set<T>(addr,value);
			}
		};

		XML_Configuration XML;

		std::map<std::string,BaseContainer*> liveConfig;

		void save()
		{

			for (std::map<std::string,BaseContainer*>::iterator it = liveConfig.begin();
				it != liveConfig.end(); ++it)
					it->second->save();
			XML.SaveXMLFile("GlobalConfig.xml");
		}

		void load()
		{
			std::string message;
			if( XML.LoadXMLFile("GlobalConfig.xml") ){
				message = "GlobalConfig.xml loaded!";
			}else{
				message = "unable to load GlobalConfig.xml check data/ folder";
			}
			LogFile::Add(message);
		}


		template<typename T>
		static T get(const std::string& k, const T & defaultvalue = T())
		{
			GlobalConfigDetail & gc = GlobalConfigDetail::Instance();
			return gc.XML.GetValue(k,defaultvalue);
		}

		template<typename T>
		static void set(const std::string& k,const T & t)
		{
			GlobalConfigDetail & gc = GlobalConfigDetail::Instance();
			///Workaround for strange commas appearing in XML
			/*std::stringstream s;
			s << t;*/
			gc.XML.SetValue(k,t);
		}

		public:

		GlobalConfigDetail()
		{
			load();
		}
		~GlobalConfigDetail()
		{
			if(getRef("XML_AUTOSAVE",1))
			{
				LogFile::Add( "Saving xml...");
				save();
			}
			else
			{
				LogFile::Add("XML_AUTOSAVE preventing XML to save");
			}
		}

		template<typename T>
		static T & getRef(const std::string& k, const T & defaultvalue = T())
		{
			GlobalConfigDetail & gc = GlobalConfigDetail::Instance();
			if(gc.liveConfig.find(k)==gc.liveConfig.end())
			{
				gc.liveConfig[k]=new Container<T>(k,defaultvalue);
			}
			Container<T> * c = static_cast<Container<T> *>(gc.liveConfig[k]);
			return c->getRef();
		}

	};
}