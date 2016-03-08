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
#include <stringstream>

namespace Avida {
  namespace WebViewer {

    class Feedback : public Avida::Feedback
    {
      private:
        std::vector<std::string> m_notify;
        std::vector<std::string> m_warning;
        std::vector<std::string> m_error;
        std::vector<std::string> m_data;
      
      public:
        Feedback() {}
        ~Feedback() {}
        
        Feedback Feedback(const Feedback&) = delete;
        Feedback& operator=(const Feedback&) = delete;
        
        
        //Note: Traditional formatting is not available to avoid buffer size limitations
        virtual void Error(const char* fmt, ...) { m_error.push_back(string(fmt));
        virtual void Warning(const char* fmt, ...) {m_error.push_back(string(fmt));
        virtual void Notify(const char* fmt, ...) {m_error.push_back(string(fmt));
        virtual void Data(const char* fmt, ...) {m_data.pushback(string(fmt));
        virtual void Clear() {m_notify.clear(); m_warning.clear(); m_data.clear(); m_error.clear();}
        
        void Error(const std::string& str) { m_error.push_back(str); }
        void Notify(const std::string& str) {m_warning.push_back(str);}
        void Warning(const std::string& str) {m_warning.push_back(str);}
        void Data(const std::string& str) {m_data.push_back(str);}
        
    };

  }
}

#endif /* WebFeedback_h */
