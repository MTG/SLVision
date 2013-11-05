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
