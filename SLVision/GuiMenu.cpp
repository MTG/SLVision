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

#include "GuiMenu.h"
#include "Globals.h"

#define X_POSITION 50

GuiMenu::GuiMenu(std::string menu_name):name(menu_name)
{
	y_position = 100;
	selected_position = 0;
	selected = "";
}

GuiMenu::~GuiMenu(void)
{
}

void GuiMenu::AddBar(std::string name, float min_value, float max_value, float step)
{
	map_menu[name] = new MenuBar(min_value, max_value, step);
	
	for(Map_menu::iterator it = map_menu.begin(); it != map_menu.end(); it++)
	{
		it->second->SetSelected(false);
	}
	selected = map_menu.begin()->first ;
	selected_position = 0;
	map_menu.begin()->second->SetSelected();
}

void GuiMenu::Draw(IplImage* img)
{
	y_position = 100;
	Globals::Font::Write(img,name.c_str(),cvPoint(X_POSITION, y_position),FONT_AXIS,0,255,0);
	for(Map_menu::iterator it = map_menu.begin(); it != map_menu.end(); it++)
	{
		y_position += 20;
		Globals::Font::Write(img,it->first.c_str(),cvPoint(X_POSITION, y_position),FONT_HELP,0,255,0);
		y_position += 10;
		it->second->DrawMenu(img,X_POSITION,y_position);
		y_position += 20;
	}
}

void GuiMenu::SetValue(std::string name, float value)
{
	if(map_menu.find(name) != map_menu.end())
		map_menu[name]->SetValue(value);
}

void GuiMenu::ValueUp(std::string name)
{
	if(map_menu.find(name) != map_menu.end())
		map_menu[name]->StepUp();
}

void GuiMenu::ValueDown(std::string name)
{
	if(map_menu.find(name) != map_menu.end())
		map_menu[name]->StepDown();
}

float GuiMenu::GetValue(std::string name)
{
	if(map_menu.find(name) != map_menu.end())
		return map_menu[name]->GetParameter();
	return -1.0f;
}


void GuiMenu::MoveUp()
{
	if(map_menu.size() > 1 && selected_position < (int)map_menu.size()-1)
	{
		selected_position++;
		int i = 0;
		for(Map_menu::iterator it = map_menu.begin(); it != map_menu.end(); it++)
		{
			it->second->SetSelected(false);
			if( i == selected_position )
			{
				selected = it->first; 
				it->second->SetSelected(true);
			}
			i++;
		}
	}
}

void GuiMenu::MoveDown()
{
	if(map_menu.size() > 1 && selected_position > 0)
	{
		selected_position--;
		int i = 0;
		for(Map_menu::iterator it = map_menu.begin(); it != map_menu.end(); it++)
		{
			it->second->SetSelected(false);
			if( i == selected_position )
			{
				selected = it->first; 
				it->second->SetSelected(true);
			}
			i++;
		}
	}
}

void GuiMenu::SelectedUp()
{
	if(map_menu.size() != 0)
		ValueUp(selected);
}

void GuiMenu::SelectedDown()
{
	if(map_menu.size() != 0)
		ValueDown(selected);
}