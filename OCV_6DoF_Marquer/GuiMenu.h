#pragma once

#include "MenuBar.h"
#include <map>
#include <string>

typedef std::map<std::string,MenuBar*> Map_menu;

class GuiMenu
{
protected:
	Map_menu map_menu;
	std::string name;
	int y_position;
	std::string selected;
	int selected_position;
public:
	GuiMenu(std::string menu_name);
	~GuiMenu(void);
	void AddBar(std::string name, float min_value, float max_value, float step);
	void Draw(IplImage* img);
	void SetValue(std::string name, float value);
	void ValueUp(std::string name);
	void ValueDown(std::string name);
	float GetValue(std::string name);
	void MoveUp();
	void MoveDown();
	void SelectedUp();
	void SelectedDown();
};

