/**
 * @file   options.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_OPTIONS_H
#define INCLUDE_ASP_OPTIONS_H

#include <asp/types.h>
#include <asp/inputs/serial.h>
#include <asp/inputs/data.h>
#include <asp/ui/window.h>

namespace asp {

struct options {
   ui::window::options window;
   inputs::serial::options serial;
   inputs::data::options data;

   static result_t<options> load(int argc, char **argv);
};

} // namespace asp

#endif /* INCLUDE_ASP_OPTIONS_H */
