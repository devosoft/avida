#ifndef AVD_MESSAGE_TYPE_BASE_HH
#define AVD_MESSAGE_TYPE_BASE_HH

class avdMessageDisplayBase;
class avdMessageTypeBase{
public:
  virtual ~avdMessageTypeBase(){}
  virtual const char *const typeName() const = 0;
  virtual const char *const className() const = 0;
  virtual bool isFatal() const = 0;
  virtual bool isPrefix() const = 0;
  virtual bool noPrefix() const = 0;
  virtual bool isActive() const = 0;
  virtual bool showWhere() const = 0;
  virtual bool showFunction() const = 0;
  virtual avdMessageDisplayBase **messageDisplay() const = 0;
  virtual void setIsActive(bool is_active) = 0;
  virtual void setShowWhere(bool show_where) = 0;
  virtual void setShowFunction(bool show_function) = 0;
};

#endif
