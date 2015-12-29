/*
 * Copyright (C) 2002-2004, 2006-2011 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef WL_EDITOR_UI_MENUS_EDITOR_MAIN_MENU_NEW_MAP_H
#define WL_EDITOR_UI_MENUS_EDITOR_MAIN_MENU_NEW_MAP_H

#include "ui_basic/box.h"
#include "ui_basic/button.h"
#include "ui_basic/spinbox.h"
#include "ui_basic/window.h"

struct EditorInteractive;

/**
 * This is the new map selection menu. It offers
 * the user to choose the new world and a few other
 * things like size, world ....
*/
struct MainMenuNewMap : public UI::Window {
	MainMenuNewMap(EditorInteractive &);

private:
	void clicked_create_map();
	void clicked_cancel();

	int32_t margin_;
	int32_t box_width_;
	UI::Box box_;
	UI::SpinBox width_;
	UI::SpinBox height_;

	// Buttons
	UI::Box button_box_;
	UI::Button ok_button_, cancel_button_;
};

#endif  // end of include guard: WL_EDITOR_UI_MENUS_EDITOR_MAIN_MENU_NEW_MAP_H
