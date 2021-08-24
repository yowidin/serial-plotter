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

private:
   context_t ctx_;
   keep_alive_t keep_alive_;

   options options_;
   ui::window window_;
};

} // namespace ig

#endif /* INCLUDE_ASP_APPLICATION_H */
