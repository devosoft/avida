#ifndef AVD_MESSAGE_DISPLAY_BASE_HH
#define AVD_MESSAGE_DISPLAY_BASE_HH

class cString;
class avdMessageDisplayBase{
public:
  virtual ~avdMessageDisplayBase(){}
  virtual void out(const cString &) const = 0;
  virtual void abort() const { abort(); }
};
extern avdMessageDisplayBase *s_avd_info_display;
extern avdMessageDisplayBase *s_avd_debug_display;
extern avdMessageDisplayBase *s_avd_error_display;
extern avdMessageDisplayBase *s_avd_fatal_display;
extern avdMessageDisplayBase *s_avd_plain_display;
void setAvdInfoDisplay(avdMessageDisplayBase &info);
void setAvdDebugDisplay(avdMessageDisplayBase &md);
void setAvdErrorDisplay(avdMessageDisplayBase &md);
void setAvdFatalDisplay(avdMessageDisplayBase &md);
void setAvdPlainDisplay(avdMessageDisplayBase &md);

#endif
