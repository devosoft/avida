#ifndef AVD_MESSAGE_CLASS_BASE_HH
#define AVD_MESSAGE_CLASS_BASE_HH

class avdMessageDisplayBase;
class avdMessageTypeBase;
class avdMessageClassBase{
public:
  virtual ~avdMessageClassBase(){};
  virtual const char *const className() const = 0;
  virtual avdMessageDisplayBase **messageDisplay() const = 0;
  virtual bool isFatal() const = 0;
  virtual bool isPrefix() const = 0;
  virtual bool noPrefix() const = 0;
};
extern avdMessageClassBase &avdMCInfo;
extern avdMessageClassBase &avdMCDebug;
extern avdMessageClassBase &avdMCError;
extern avdMessageClassBase &avdMCFatal;
extern avdMessageClassBase &avdMCPlain;

#endif
