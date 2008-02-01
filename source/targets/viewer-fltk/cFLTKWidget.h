/*
 *  cFLTKWidget.h
 *  Avida
 *
 *  Created by Charles on 7-9-07
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

// This is a base class for all FLTK widgets

#ifndef cFLTKWidget_h
#define cFLTKWidget_h

#include "cColor.h"
#include "cGUIWidget.h"

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Pixmap.H>

class cFLTKWidget {
protected:
  Fl_Widget * m_widget;
  cGUIWidget * m_gui_widget;

  cString m_tooltip;

public:
  cFLTKWidget(cGUIWidget * gui_widget) : m_widget(NULL), m_gui_widget(gui_widget) { ; }
  ~cFLTKWidget() { ; }

  void SetWidget(Fl_Widget * widget) { m_widget = widget; }

  void SetBackgroundColor(const cColor & color) {
    assert(m_widget != NULL);
    fl_color(color.Red(), color.Green(), color.Blue());
    m_widget->color(fl_color());
  }

  void SetLabel(const cString & name) {
    assert(m_widget != NULL);
    m_widget->copy_label(name);
    m_gui_widget->SetName(name);
  }

  void SetFont(int id) {
    m_widget->labelfont(id);
  }
  
  void SetFontSize(int size) {
    m_widget->labelsize(size);
  }
  
  void SetFontColor(const cColor & color) {
    assert(m_widget != NULL);
    fl_color(color.Red(), color.Green(), color.Blue());
    m_widget->labelcolor(fl_color());
  }

  void SetFontAlign(int id) {
    m_widget->align(id);
  }

  void SetTooltip(const cString & in_tip) {
    m_tooltip = in_tip;
    m_widget->tooltip(m_tooltip);
  }
  
  void SetImage_XPM(char *xpm_info[]) {
    // @CAO Should save the pixmap?
    Fl_Pixmap * pixmap = new Fl_Pixmap(xpm_info);
    m_widget->image(pixmap);
  }

  void Refresh() {
    m_widget->copy_label(m_gui_widget->GetName());
    // m_widget->labelsize(m_gui_widget->GetFontSize());
    m_widget->redraw();
  }

};

#endif
