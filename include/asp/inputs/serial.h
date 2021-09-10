/**
 * @file   serial.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_INPUTS_SERIAL_H
#define INCLUDE_ASP_INPUTS_SERIAL_H

#include <asp/types.h>
#include <asp/inputs/data.h>
#include <asp/ui/drawable.h>

#include <boost/asio.hpp>

#include <cstdint>
#include <vector>
#include <memory>

namespace asp {

struct options;

namespace inputs {

class serial : public ui::drawable {
public:
   class line_separator_picker;
   class baud_rate_picker;

   struct options {
      std::string port{};
      int baud_rate{9600};
      bool mirror{false};
      std::string line_separator{"\n"};

      static po_desc_t prepare();
      static options load(po_vars_t &vm);
   };

public:
   serial(context_t &ctx,
          const asp::options &opts,
          data &data,
          ui::logs &logs);
   ~serial();

public:
   void start();

   void draw() override;

private:
   void read_some();
   void split_data(std::size_t bytes_read);
   void close();
   void handle_separator_change();

   void set_error(const std::string &err);

private:
   data *data_;
   ui::logs *logs_;
   options opts_;
   bool connected_{false};
   bool collapsed_{false};

   boost::asio::serial_port serial_;
   std::vector<char> buffer_;
   std::string remainder_{};

   std::string status_{};

   std::unique_ptr<line_separator_picker> line_separator_picker_;
   std::unique_ptr<baud_rate_picker> baud_rate_picker_;
};

} // namespace inputs
} // namespace asp

#endif /* INCLUDE_ASP_INPUTS_SERIAL_H */
