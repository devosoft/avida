
map_gui {
  HEADERS += \
    $$MAP_GUI_HH/avd_map_canvas.hh \
    $$MAP_GUI_HH/avd_map_canvas_rectangle.hh \
    $$MAP_GUI_HH/avd_map_canvas_rectangle_proto.hh \
    $$MAP_GUI_HH/avd_map_canvas_view.hh \
    $$MAP_GUI_HH/avd_map_ctrl.hh \
    $$MAP_GUI_HH/avd_map_gui_dbg.hh \
    $$MAP_GUI_HH/avd_map_gui_dbg_proto.hh \
    $$MAP_GUI_HH/avd_map_layer_proto.hh \
    $$MAP_GUI_HH/avd_map_layer.hh \
    $$MAP_GUI_HH/avd_map_view.hh

  SOURCES += \
    $$MAP_GUI_CC/avd_map_canvas.cc \
    $$MAP_GUI_CC/avd_map_canvas_rectangle.cc \
    $$MAP_GUI_CC/avd_map_canvas_view.cc \
    $$MAP_GUI_CC/avd_map_ctrl.cc \
    $$MAP_GUI_CC/avd_map_gui.cc \
    $$MAP_GUI_CC/avd_map_layer.cc \
    $$MAP_GUI_CC/avd_map_view.cc
}

# arch-tag: qmake include file for map module
