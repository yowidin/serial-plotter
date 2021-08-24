#include <iostream>
#include <memory>

#include <asp/application.h>
#include <asp/options.h>

std::unique_ptr<asp::application> g_application;

int main(int argc, char **argv) {
   auto res = asp::options::load(argc, argv);
   if (!res) {
      return EXIT_FAILURE;
   }
   auto options = res.value();

   try {
      g_application = std::make_unique<asp::application>(options);
      g_application->run();
   } catch (std::exception &e) {
      std::cout << "std::exception: " << e.what() << std::endl;
   }

   return EXIT_SUCCESS;
}
