#pragma once

#include "geotypes.h"
#include <vector>
#include <boost/variant.hpp>
#include "geoparser_config.h"

namespace geo {



std::vector<boost::variant<Location, std::string>> parse_area_description(std::string const& input, parser::position_cache& positions);
bool starts_with_coord(std::string const& input);

}
