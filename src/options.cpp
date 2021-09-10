/**
 * @file   options.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/options.h>

using namespace asp;

namespace po = boost::program_options;

result_t<options> options::load(int argc, char **argv) {
   po::options_description general("Serial Plotter");
   general.add_options()("help,h", "Show help");

   po::options_description all;

   auto window_args = ui::window::options::prepare();
   auto serial_args = inputs::serial::options::prepare();
   auto data_args = inputs::data::options::prepare();

   all.add(general).add(window_args).add(serial_args).add(data_args);

   try {
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, all), vm);

      if (vm.count("help")) {
         std::cout << all << "\n";
         return std::errc::interrupted;
      }

      po::notify(vm);

      auto window = ui::window::options::load(vm);
      auto serial = inputs::serial::options::load(vm);
      auto data = inputs::data::options::load(vm);

      return options{window, serial, data};

   } catch (std::exception const &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      std::cerr << all << std::endl;
      return std::errc::invalid_argument;
   }
}
