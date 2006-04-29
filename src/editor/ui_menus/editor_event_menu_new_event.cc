/*
 * Copyright (C) 2002-4 by the Widelands Development Team
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "constants.h"
#include "editor_event_menu_new_event.h"
#include "i18n.h"
#include "ui_window.h"
#include "ui_multilinetextarea.h"
#include "ui_textarea.h"
#include "ui_button.h"
#include "ui_listselect.h"
#include "editorinteractive.h"
#include "event.h"
#include "event_factory.h"
#include "map.h"
#include "map_event_manager.h"
#include "error.h"

Editor_Event_Menu_New_Event::Editor_Event_Menu_New_Event(Editor_Interactive* parent) :
   UIWindow(parent, 0, 0, 400, 240, _("New Event")) {
  m_parent=parent;

   // Caption
   UITextarea* tt=new UITextarea(this, 0, 0, _("New Event Menu"), Align_Left);
   tt->set_pos((get_inner_w()-tt->get_w())/2, 5);

   const int offsx=5;
   const int offsy=25;
   const int spacing=5;
   int posx=offsx;
   int posy=offsy;

   // Event List
   new UITextarea(this, spacing, offsy, _("Available Events: "), Align_Left);
   m_event_list=new UIListselect(this, spacing, offsy+20, (get_inner_w()/2)-2*spacing, get_inner_h()-offsy-55);
   m_event_list->selected.set(this, &Editor_Event_Menu_New_Event::selected);
   m_event_list->double_clicked.set(this, &Editor_Event_Menu_New_Event::double_clicked);

   uint i=0;
   for(i=0; i<Event_Factory::get_nr_of_available_events(); i++) {
      Event_Descr* d=Event_Factory::get_event_descr(i);
      m_event_list->add_entry(d->name, d);
   }
   m_event_list->sort();

   // Descr List
   new UITextarea(this, (get_inner_w()/2)+spacing, offsy, _("Description: "), Align_Left);
   m_description=new UIMultiline_Textarea(this, (get_inner_w()/2)+spacing, offsy+20, (get_inner_w()/2)-2*spacing, get_inner_h()-offsy-55, "", Align_Left);

   posy=get_inner_h()-30;
   posx=(get_inner_w()/2)-80-spacing;
   m_ok_button=new UIButton(this, posx, posy, 80, 20, 0, 1);
   m_ok_button->set_title(_("Ok"));
   m_ok_button->clickedid.set(this, &Editor_Event_Menu_New_Event::clicked);
   m_ok_button->set_enabled(0);
   posx=(get_inner_w()/2)+spacing;
   UIButton* b=new UIButton(this, posx, posy, 80, 20, 1, 0);
   b->set_title(_("Cancel"));
   b->clickedid.set(this, &Editor_Event_Menu_New_Event::clicked);

   center_to_parent();
}

/*
 * cleanup
 */
Editor_Event_Menu_New_Event::~Editor_Event_Menu_New_Event(void) {
}

/*
 * Handle mouseclick
 *
 * we're a modal, therefore we can not delete ourself
 * on close (the caller must do this) instead
 * we simulate a cancel click
 */
bool Editor_Event_Menu_New_Event::handle_mouseclick(uint btn, bool down, int mx, int my) {
   if(btn == MOUSE_RIGHT && down) {
      clicked(0);
      return true;
   } else
      return false; // we're not dragable
}

/*
 * a button has been clicked
 */
void Editor_Event_Menu_New_Event::clicked(int i) {
   if(!i) {
      // Cancel has been clicked
      end_modal(0);
      return;
   }

   Event_Descr* d=static_cast<Event_Descr*>(m_event_list->get_selection());
   // Create new event
   Event* event=
      Event_Factory::make_event_with_option_dialog(d->id.c_str(), m_parent, 0);
   if(!event) {
      // No event created, choose another, user
      return;
   }
   m_parent->get_map()->get_mem()->register_new_event(event);
   end_modal(1);
   return;
}

/*
 * the listbox got selected
 */
void Editor_Event_Menu_New_Event::selected(int i) {
   Event_Descr* d=static_cast<Event_Descr*>(m_event_list->get_selection());
   m_description->set_text(d->descr);
   m_ok_button->set_enabled(true);
}

/*
 * listbox got double clicked
 */
void Editor_Event_Menu_New_Event::double_clicked(int i) {
   // Ok
   clicked(1);
}
