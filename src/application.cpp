/**
 * @file   application.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/application.h>

using namespace asp;

application::application(options opts)
   : ctx_{}
   , keep_alive_{boost::asio::make_work_guard(ctx_)}
   , options_{opts}
   , window_{ctx_, options_} {
   // Nothing to do here
}

application::~application() {
   keep_alive_.reset();
   ctx_.stop();
}

void application::run() {
   window_.start();

   while (!window_.can_stop()) {
      window_.update();

      // Run context tasks within the desired frame rate interval
      auto now = std::chrono::steady_clock::now;
      auto start = now();
      while (ctx_.poll_one()) {
         if ((now() - start) > std::chrono::milliseconds(4)) {
            break;
         }
      }
   }
}
