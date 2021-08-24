/**
 * @file   options.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/options.h>

using namespace asp;

namespace po = boost::program_options;

result_t<options> options::load(int argc, char **argv) {
   po::options_description general("Arduino Serial Plotter");
   general.add_options()("help,h", "Show help");

   po::options_description all;

   // clang-format off
   all.add_options()
      ("full-screen", "Start in full screen")
      ("width", po::value<int>()->default_value(1280), "Window width")
      ("height", po::value<int>()->default_value(900), "Window height")
       ;
   // clang-format on

   auto serial_args = inputs::serial::options::prepare();

   all.add(general).add(serial_args);

   try {
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, all), vm);

      if (vm.count("help")) {
         std::cout << all << "\n";
         return std::errc::interrupted;
      }

      po::notify(vm);

      auto serial = inputs::serial::options::load(vm);

      auto full_screen = vm.count("full-screen") != 0;
      auto width = vm["width"].as<int>();
      auto height = vm["height"].as<int>();

      return options{full_screen, width, height, serial};

   } catch (std::exception const &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      std::cerr << all << std::endl;
      return std::errc::invalid_argument;
   }
}
