#pragma once

#include "geotypes.h"
#include <vector>
#include <boost/variant.hpp>


namespace geo {

std::vector<boost::variant<Location, std::string>> parse_area_description(std::string const& input);
bool starts_with_coord(std::string const& input);

}
