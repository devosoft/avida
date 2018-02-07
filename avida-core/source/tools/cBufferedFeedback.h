//
//  cBufferedFeedback.h
//  Avida-Core
//
//  Created by Matthew Rupp on 1/17/18.
//

#ifndef cBufferedFeedback_h
#define cBufferedFeedback_h

#include "avida/core/Feedback.h"
#include "avida/core/Types.h"

namespace Avida{
  class cBufferedFeedback : public Feedback
  {
  
  
  public:
    enum eFeedbackType {
      UF_ERROR,
      UF_WARNING,
      UF_NOTIFICATION
    };
    
    
  protected:
    struct sEntry {
      eFeedbackType ftype;
      cString message;
      AbortCondition cond;
      
      sEntry() { ; }
      sEntry(eFeedbackType in_ftype, const cString& in_msg) : ftype(in_ftype), message(in_msg) { ; }
      sEntry(eFeedbackType in_ftype, const char* fmt, va_list args) : ftype(in_ftype) { message.Set(fmt, args); }
    };
    
  public:
    
    virtual int GetNumMessages() const = 0;
    virtual int GetNumNotifications() const = 0;
    virtual int GetNumWarnings() const = 0;
    virtual int GetNumErrors() const = 0;
    
    virtual const cString& GetMessage(int k) const = 0;
    virtual eFeedbackType GetMessageType(int k) const = 0;
    
    virtual void Clear() = 0;
  };
}

#endif /* cBufferedFeedback_h */
