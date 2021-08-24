/**
 * @file   logs.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_UI_LOGS_H
#define INCLUDE_ASP_UI_LOGS_H

#include <string>
#include <vector>
#include <sstream>

namespace asp::ui {

class logs {
public:
   logs(std::string name);

public:
   void draw();
   void add_entry(const std::string &entry);

   template <typename ... ARGS>
   void add(ARGS ... args) {
      std::ostringstream os;
      do_format(os, args...);
      add_entry(os.str());
   }

private:
   template <typename T>
   void do_format(std::ostream &os, T value) {
      os << value;
   }

   template <typename T, typename ... ARGS>
   void do_format(std::ostream &os, T value, ARGS ... args) {
      os << value;
      do_format(os, args...);
   }


private:
   std::vector<std::string> logs_{};
   bool auto_scroll_{true};
   const std::string name_;
   const std::string auto_scroll_id_;
   const std::string clear_id_;
   const std::string child_id_;
   const std::string copy_id_;
};

}

#endif /* INCLUDE_ASP_UI_LOGS_H */
