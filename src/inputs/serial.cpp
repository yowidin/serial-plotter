/**
 * @file   serial.cpp
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */

#include <asp/inputs/serial.h>
#include <asp/options.h>
#include <asp/types.h>
#include <asp/ui/logs.h>
#include <asp/ui/std_input_text.h>

#include <imgui.h>

#include <vector>

using namespace asp::inputs;

namespace asio = boost::asio;

class serial::line_separator_picker {
public:
   asp::result_t<void> set_selection(const std::string &separator) {
      auto it = std::find(std::begin(values_), std::end(values_), separator);
      if (it == std::end(values_)) {
         return std::errc::result_out_of_range;
      }

      current_selection_ =
          static_cast<int>(std::distance(std::begin(values_), it));
      return asp::outcome::success();
   }

   [[nodiscard]] const std::string &separator() const {
      return values_[current_selection_];
   }

   bool draw() {
      return ImGui::Combo("Line Separator", &current_selection_, texts_.data(),
                          static_cast<int>(texts_.size()));
   }

private:
   const std::vector<const char *> texts_{"\\n\\r", "\\r\\n", "\\n", "\\r"};
   const std::vector<std::string> values_{"\n\r", "\r\n", "\n", "\r"};

   int current_selection_{0};
};

class serial::baud_rate_picker {
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

bool ends_with(const std::string &what, const std::string &with) {
   if (with.length() > what.length()) {
      return false;
   }

   auto m =
       std::mismatch(with.rbegin(), with.rend(), what.rbegin(), what.rend());

   return m.first == with.rend();
}

////////////////////////////////////////////////////////////////////////////////
/// Class: options
////////////////////////////////////////////////////////////////////////////////
asp::po_desc_t serial::options::prepare() {
   namespace po = boost::program_options;

   po::options_description od("Serial Options");

   // clang-format off
   od.add_options()
       ("serial-port,p", po::value<std::string>(), "Serial Port")
       ("baud-rate,b", po::value<int>()->required()->default_value(9600), "Baud rate")
       ("mirror,m", "Mirror input data to the standard output")
       ("line-separator,s", po::value<std::string>()->default_value("nl"),
            "Line separator, one of nl, cr, nlcr, crnl. Where nl - is the new "
            "line character ('\\n'), and cr - is the carriage return "
            "character ('\\r') ")
       ;
   // clang-format on

   return od;
}

serial::options serial::options::load(po_vars_t &vm) {
   using namespace std::string_literals;

   options opts;
   if (vm.count("serial-port")) {
      opts.port = vm["serial-port"].as<std::string>();
   }
   opts.baud_rate = vm["baud-rate"].as<int>();
   opts.mirror = vm.count("mirror") != 0;
   opts.line_separator = vm["line-separator"].as<std::string>();
   if (opts.line_separator == "cr") {
      opts.line_separator = "\r";
   } else if (opts.line_separator == "nl") {
      opts.line_separator = "\n";
   } else if (opts.line_separator == "nlcr") {
      opts.line_separator = "\n\r";
   } else if (opts.line_separator == "crnl") {
      opts.line_separator = "\r\n";
   } else {
      throw boost::program_options::error("Invalid separator value: "s +
                                          opts.line_separator);
   }

   return opts;
}

////////////////////////////////////////////////////////////////////////////////
/// Class: serial
////////////////////////////////////////////////////////////////////////////////
serial::serial(boost::asio::io_context &ctx,
               const asp::options &opts,
               data &data,
               ui::logs &logs)
   : opts_{opts.serial}
   , data_{&data}
   , serial_{ctx}
   , logs_{&logs}
   , line_separator_picker_{new line_separator_picker}
   , baud_rate_picker_{new baud_rate_picker} {
   using namespace std::string_literals;
   auto res = baud_rate_picker_->set_selection(opts_.baud_rate);
   if (!res) {
      throw std::runtime_error("Baud Rate out of range: "s +
                               std::to_string(opts_.baud_rate));
   }

   res = line_separator_picker_->set_selection(opts_.line_separator);
   if (!res) {
      throw std::runtime_error("Invalid line separator");
   }
}

serial::~serial() = default;

void serial::set_error(const std::string &err) {
   logs_->add("Serial port error: ", err);
   status_ = err;
   collapsed_ = false;
}

void serial::start() {
   status_ = "Opening...";

   boost::system::error_code ec;
   serial_.open(opts_.port, ec);
   if (ec) {
      set_error(ec.message());
      connected_ = false;
      return;
   }

   serial_.set_option(asio::serial_port::baud_rate(opts_.baud_rate), ec);
   if (ec) {
      set_error(ec.message());
      close();
      return;
   }

   buffer_.resize(32);

   status_.clear();

   connected_ = true;
   collapsed_ = false;

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
                                 set_error(ec.message());
                                 close();
                              }
                           });
}

void serial::split_data(std::size_t bytes_read) {
   for (std::size_t i = 0; i < bytes_read; ++i) {
      auto ch = buffer_[i];
      if (opts_.mirror) {
         std::cout << ch;
      }

      remainder_ += ch;

      if (ends_with(remainder_, opts_.line_separator)) {
         // It's a line split
         remainder_.resize(remainder_.size() - opts_.line_separator.size());
         data_->add_raw_entry(remainder_);
         remainder_.clear();
      }
   }

   if (opts_.mirror) {
      std::cout.flush();
   }
}

void serial::handle_separator_change() {
   std::string::size_type from = 0;
   std::string::size_type to;
   std::string entry;
   while (from < remainder_.size()) {
      to = remainder_.find(opts_.line_separator, from);
      if (to == std::string::npos) {
         break;
      }

      entry = remainder_.substr(from, to - from);
      data_->add_raw_entry(entry);
      from = to + opts_.line_separator.size();
   }

   if (from != 0) {
      if (from == remainder_.size()) {
         remainder_.clear();
      } else {
         remainder_ = remainder_.substr(from, remainder_.size() - from);
      }
   }
}

void serial::draw() {
   if ((!status_.empty() || !connected_) && !collapsed_) {
      ImGui::SetNextItemOpen(true);
   }

   if (!ImGui::CollapsingHeader("Serial")) {
      collapsed_ = true;
      return;
   }

   if (!ImGui::BeginTable("serial##options", 2,
                          ImGuiTableFlags_SizingFixedSame)) {
      return;
   }

   auto next = [] { ImGui::TableNextColumn(); };

   auto separator_picker = [&, this] {
      next();
      if (line_separator_picker_->draw()) {
         opts_.line_separator = line_separator_picker_->separator();
         handle_separator_change();
      }
   };

   if (!connected_) {
      next();
      ui::std_input_text("Port", opts_.port);

      next();
      if (baud_rate_picker_->draw()) {
         opts_.baud_rate = baud_rate_picker_->baud_rate();
      }

      next();
      if (ImGui::Button("Open")) {
         start();
      }

      separator_picker();
   } else {
      next();
      if (ImGui::Button("Close")) {
         close();
      }

      separator_picker();
   }

   next();
   ImGui::Checkbox("Mirror to console", &opts_.mirror);

   ImGui::EndTable();

   if (!status_.empty()) {
      ImGui::Text("Status: ");
      ImGui::SameLine();
      ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "%s", status_.c_str());
   } else {
      ImGui::Text(connected_ ? "Status: operating" : "Status: idle");
   }
}

void serial::close() {
   boost::system::error_code ec;
   serial_.close(ec);
   if (ec) {
      set_error(ec.message());
   }
   connected_ = false;
}
