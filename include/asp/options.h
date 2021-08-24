/**
 * @file   options.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_OPTIONS_H
#define INCLUDE_ASP_OPTIONS_H

#include <asp/types.h>
#include <asp/inputs/serial.h>

namespace asp {

struct options {
   bool full_screen{false};
   int width{1280};
   int height{900};

   inputs::serial::options serial;

   static result_t<options> load(int argc, char **argv);
};

} // namespace asp

#endif /* INCLUDE_ASP_OPTIONS_H */
