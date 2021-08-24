/**
 * @file   data.h
 * @author Dennis Sitelew
 * @date   Aug. 21, 2021
 */
#ifndef INCLUDE_ASP_INPUTS_DATA_H
#define INCLUDE_ASP_INPUTS_DATA_H

#include <asp/ui/logs.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace asp::inputs {

class data {
public:
   data(ui::logs &logs, ui::logs &raw);

public:
   void draw();
   void add_raw_entry(const std::string &entry);

private:
   ui::logs *logs_;
   ui::logs *raw_;
   std::unordered_map<std::string, std::vector<float>> plot_data_;

   bool cross_hair_{false};
   bool follow_{true};
   int follow_interval_{1000};
};

} // namespace asp::inputs

#endif /* INCLUDE_ASP_INPUTS_DATA_H */
