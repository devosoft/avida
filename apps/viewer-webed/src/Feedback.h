//
//  WebFeedback.h
//  viewer-webed
//
//  Created by Matthew Rupp on 2/19/16.
//  Copyright © 2016 MSU. All rights reserved.
//

#ifndef WebFeedback_h
#define WebFeedback_h

#include <string>
#include <vector>
#include "avida/core/Feedback.h"
#include "cUserFeedback.h"
#include "cString.h"
#include <sstream>

namespace Avida {
  namespace WebViewer{
    
    class Feedback : public Avida::Feedback
    {
      public:
       enum FeedbackType {ERROR, WARNING, NOTIFY, DATA, DDEBUG, UNKNOWN};
      
      class FeedbackEntry 
      {
      private:
        std::string fb_msg;
        FeedbackType fb_type;
        static ostringstream m_debug_stream;
        
      public:
        FeedbackEntry(const string& m, FeedbackType t) : fb_msg(m), fb_type(t) {}
        const std::string& GetMessage() const {return fb_msg;}
        const FeedbackType GetType() const {return fb_type;}
        ostringstream& GetDebugStream() {return m_debug_stream;}
        
      };
      
      typedef vector<FeedbackEntry> FeedbackEntries;
      
    private:
      
      FeedbackEntries m_entries;
      
    public:
      Feedback() {}
      virtual ~Feedback() {}
      
      Feedback(const cUserFeedback& fb)
      {
        for (int i=0; i < fb.GetNumMessages(); i++){
          cUserFeedback::eFeedbackType t = fb.GetMessageType(i);
          const char* msg = fb.GetMessage(i).GetData();
          switch (t){
            case cUserFeedback::UF_ERROR:
              m_entries.push_back(FeedbackEntry(msg, ERROR));
              break;
            case cUserFeedback::UF_WARNING:
              m_entries.push_back(FeedbackEntry(msg, WARNING));
              break;
            case cUserFeedback::UF_NOTIFICATION:
              m_entries.push_back(FeedbackEntry(msg, NOTIFY));
              break;
            case cUserFeedback::UF_DATA:
              m_entries.push_back(FeedbackEntry(msg, DATA));
              break;
          }
        }
      }
      
      Feedback(const Feedback& fb)
      {
        *this = fb;
      }
      
      
      Feedback& operator=(const Feedback& fb)
      {
        this->m_entries = fb.m_entries;
        return *this;
      }
      
      
      //Note: Traditional formatting is not available to avoid buffer size limitations
      virtual void Error(const char* fmt, ...) 
      { 
        m_entries.push_back(FeedbackEntry(std::string(fmt),ERROR));
      }
      
      
      virtual void Warning(const char* fmt, ...) 
      {
        m_entries.push_back(FeedbackEntry(std::string(fmt),WARNING));
      }
      
      virtual void Notify(const char* fmt, ...)
      {
        m_entries.push_back(FeedbackEntry(std::string(fmt), NOTIFY));
      }
      
      
      virtual void Data(const char* fmt, ...)
      {
        m_entries.push_back(FeedbackEntry(std::string(fmt), DATA));
      }
      
      void Debug(const char* fmt, ...)
      {
        m_entries.push_back(FeedbackEntry(std::string(fmt), DDEBUG));
      }
      
      virtual void Clear() { m_entries.clear(); }
      
      const FeedbackEntries& GetFeedback() const { return m_entries;}
      
    };
  }
}


#endif /* WebFeedback_h */
