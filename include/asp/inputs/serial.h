/**
 * @file   serial.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_INPUTS_SERIAL_H
#define INCLUDE_ASP_INPUTS_SERIAL_H

#include <asp/inputs/data.h>
#include <asp/types.h>
#include <asp/ui/logs.h>

#include <boost/asio.hpp>

#include <cstdint>
#include <vector>

namespace asp {

class options;

namespace inputs {

class serial {
public:
   struct options {
      std::string port{""};
      int baud_rate{9600};

      static po_desc_t prepare();
      static options load(po_vars_t &vm);
   };

public:
   serial(boost::asio::io_context &ctx,
          const asp::options &opts,
          data &data,
          ui::logs &logs);

public:
   void start();
   void draw();

private:
   void read_some();
   void split_data(std::size_t bytes_read);
   void close();

private:
   boost::asio::io_context *ctx_;
   data *data_;
   ui::logs *logs_;
   options opts_;
   bool connected_{false};

   boost::asio::serial_port serial_;
   std::vector<char> buffer_;
   std::string remainder_{};

   std::string status_{};
};

} // namespace inputs
} // namespace asp

#endif /* INCLUDE_ASP_INPUTS_SERIAL_H */
