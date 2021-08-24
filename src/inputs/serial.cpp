/**
 * @file   serial.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/inputs/serial.h>
#include <asp/options.h>
#include <asp/types.h>
#include <asp/ui/std_input_text.h>

#include <imgui.h>

#include <vector>

using namespace asp::inputs;

namespace asio = boost::asio;

namespace {
class baud_rate_picker {
public:
   asp::result_t<void> set_selection(int baud_rate) {
      auto it = std::find(std::begin(values_), std::end(values_), baud_rate);
      if (it == std::end(values_)) {
         return std::errc::result_out_of_range;
      }

      current_selection_ =
          static_cast<int>(std::distance(std::begin(values_), it));
      return asp::outcome::success();
   }

   [[nodiscard]] int baud_rate() const { return values_[current_selection_]; }

   bool draw() {
      return ImGui::Combo("Baud Rate", &current_selection_, rates_.data(),
                          static_cast<int>(rates_.size()));
   }

private:
   const std::vector<const char *> rates_{
       "300",    "1200",   "2400",   "4800",    "9600",
       "19200",  "38400",  "57600",  "76800",   "115200",
       "230400", "250000", "500000", "1000000", "2000000"};

   const std::vector<int> values_{300,    1200,   2400,   4800,    9600,
                                  19200,  38400,  57600,  76800,   115200,
                                  230400, 250000, 500000, 1000000, 2000000};

   int current_selection_{0};
};

baud_rate_picker g_baud_rate_picker;

} // namespace

////////////////////////////////////////////////////////////////////////////////
/// Class: options
////////////////////////////////////////////////////////////////////////////////
asp::po_desc_t serial::options::prepare() {
   namespace po = boost::program_options;

   po::options_description od("Serial Options");

   // clang-format off
   od.add_options()
       ("serial-port,p", po::value<std::string>()->required(), "Serial Port")
       ("baud-rate,b", po::value<int>()->required()->default_value(9600), "Baud rate")
       ;
   // clang-format on

   return od;
}

serial::options serial::options::load(po_vars_t &vm) {
   options opts;
   opts.port = vm["serial-port"].as<std::string>();
   opts.baud_rate = vm["baud-rate"].as<int>();
   return opts;
}

////////////////////////////////////////////////////////////////////////////////
/// Class: serial
////////////////////////////////////////////////////////////////////////////////
serial::serial(boost::asio::io_context &ctx,
               const asp::options &opts,
               data &data,
               ui::logs &logs)
   : ctx_{&ctx}
   , opts_{opts.serial}
   , data_{&data}
   , serial_{ctx}
   , logs_{&logs} {
   // Nothing to do here
   auto res = g_baud_rate_picker.set_selection(opts_.baud_rate);
   if (!res) {
      throw std::runtime_error("Baud Rate out of range");
   }
}

void serial::start() {
   status_ = "Opening...";

   boost::system::error_code ec;
   serial_.open(opts_.port, ec);
   if (ec) {
      status_ = ec.message();
      connected_ = false;
      return;
   }

   serial_.set_option(asio::serial_port::baud_rate(opts_.baud_rate), ec);
   if (ec) {
      status_ = ec.message();
      close();
      return;
   }

   buffer_.resize(32);

   status_.clear();

   connected_ = true;

   read_some();
}

void serial::read_some() {
   serial_.async_read_some(asio::buffer(buffer_),
                           [this](auto ec, auto bytes_read) {
                              if (!ec) {
                                 split_data(bytes_read);
                                 read_some();
                              } else {
                                 connected_ = false;
                                 status_ = ec.message();
                                 logs_->add_entry(status_);
                                 close();
                              }
                           });
}

void serial::split_data(std::size_t bytes_read) {
   for (std::size_t i = 0; i < bytes_read; ++i) {
      auto ch = buffer_[i];
      if (ch != '\n') {
         remainder_ += ch;
         continue;
      }

      // It's a line split
      data_->add_raw_entry(remainder_);
      remainder_.clear();
   }
}

void serial::draw() {
   if (!status_.empty()) {
      ImGui::SetNextItemOpen(true);
   }

   if (!ImGui::CollapsingHeader("Serial")) {
      return;
   }

   if (!ImGui::BeginTable("serial##options", 2,
                          ImGuiTableFlags_SizingFixedSame)) {
      return;
   }

   auto next = [] { ImGui::TableNextColumn(); };

   if (!connected_) {
      next();
      ui::std_input_text("Port", opts_.port);

      next();
      if (g_baud_rate_picker.draw()) {
         opts_.baud_rate = g_baud_rate_picker.baud_rate();
      }

      next();
      if (ImGui::Button("Open")) {
         start();
      }
   } else {
      next();
      if (ImGui::Button("Close")) {
         close();
      }
   }

   if (!status_.empty()) {
      next();
      ImGui::Text("Status: ");
      ImGui::SameLine();
      ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "%s", status_.c_str());
   } else {
      next();
      ImGui::Text("Status: Operating");
   }

   ImGui::EndTable();
}

void serial::close() {
   boost::system::error_code ec;
   serial_.close(ec);
   if (ec) {
      status_ = ec.message();
   }
   connected_ = false;
}
