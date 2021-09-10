/**
 * @file   application.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_APPLICATION_H
#define INCLUDE_ASP_APPLICATION_H

#include <asp/config.h>
#include <asp/options.h>
#include <asp/ui/window.h>
#include <asp/ui/logs.h>

#include <boost/asio.hpp>

namespace asp {

class application {
public:
   using context_t = boost::asio::io_context;
   using executor_t = context_t::executor_type;
   using keep_alive_t = boost::asio::executor_work_guard<executor_t>;

public:
   explicit application(options opts);
   ~application();

public:
   void run();

   const options &get_options() const { return options_; }

   context_t &get_context() { return ctx_; }
   ui::window &get_window() { return window_; };
   inputs::data &get_data() { return data_; }

private:
   //! I/O Context
   context_t ctx_;

   //! Keep alive job, preventing context from expiring while application is
   //! running
   keep_alive_t keep_alive_;

   //! Application options
   options options_;

   //! Application log messages
   ui::logs logs_;

   //! Parsed data container
   inputs::data data_;

   //! Serial port input
   inputs::serial serial_;

   //! Main application window
   ui::window window_;
};

} // namespace ig

#endif /* INCLUDE_ASP_APPLICATION_H */
