
tools {
  HEADERS += $$TOOLS_HH/indexed_block_struct.hh \
             $$TOOLS_HH/cBlockStruct.h \
             $$TOOLS_HH/data_entry.hh \
             $$TOOLS_HH/data_manager_base.hh \
             $$TOOLS_HH/data_file.hh \
             $$TOOLS_HH/file.hh \
             $$TOOLS_HH/fixed_block.hh \
             $$TOOLS_HH/functions.hh \
             $$TOOLS_HH/merit.hh \
             $$TOOLS_HH/message_display.hh \
             $$TOOLS_HH/message_display_hdrs.hh \
             $$TOOLS_HH/random.hh \
             $$TOOLS_HH/string.hh \
             $$TOOLS_HH/string_list.hh \
             $$TOOLS_HH/string_util.hh \
             $$TOOLS_HH/tArray.hh \
             $$TOOLS_HH/tBuffer.hh \
             $$TOOLS_HH/tDictionary.hh \
             $$TOOLS_HH/tList.hh \
             $$TOOLS_HH/tMatrix.hh \
#            $$TOOLS_HH/tMemTrack.hh \
             $$TOOLS_HH/tools.hh \
             $$TOOLS_HH/tVector.hh \
             $$TOOLS_HH/uint.hh \
             $$TOOLS_HH/weighted_index.hh \
             $$TOOLS_HH/win32_mkdir_hack.hh
  
  SOURCES += $$TOOLS_CC/cBlockStruct.cc \
             $$TOOLS_CC/cConstSchedule.cc \
             $$TOOLS_CC/cCountTracker.cc \
             $$TOOLS_CC/data_entry.cc \
             $$TOOLS_CC/data_file.cc \
             $$TOOLS_CC/data_file_manager.cc \
             $$TOOLS_CC/data_manager_base.cc \
             $$TOOLS_CC/default_message_display.cc \
             $$TOOLS_CC/double_sum.cc \
             $$TOOLS_CC/file.cc \
             $$TOOLS_CC/fixed_block.cc \
             $$TOOLS_CC/genesis.cc \
             $$TOOLS_CC/help_alias.cc \
             $$TOOLS_CC/help_manager.cc \
             $$TOOLS_CC/help_type.cc \
             $$TOOLS_CC/histogram.cc \
             $$TOOLS_CC/indexed_block_struct.cc \
             $$TOOLS_CC/init_file.cc \
             $$TOOLS_CC/int_sum.cc \
             $$TOOLS_CC/integrated_schedule.cc \
             $$TOOLS_CC/integrated_schedule_node.cc \
             $$TOOLS_CC/merit.cc \
             $$TOOLS_CC/message_display.cc \
             $$TOOLS_CC/prob_schedule.cc \
             $$TOOLS_CC/random.cc \
             $$TOOLS_CC/ref_block.cc \
             $$TOOLS_CC/running_average.cc \
             $$TOOLS_CC/scaled_block.cc \
             $$TOOLS_CC/schedule.cc \
             $$TOOLS_CC/string.cc \
             $$TOOLS_CC/string_iterator.cc \
             $$TOOLS_CC/string_list.cc \
             $$TOOLS_CC/string_util.cc \
             $$TOOLS_CC/tools.cc \
             $$TOOLS_CC/weighted_index.cc
}
