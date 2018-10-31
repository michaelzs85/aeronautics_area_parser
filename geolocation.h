
#pragma once

#include <boost/spirit/home/x3.hpp>
#include "geoast.h"

namespace geo {
namespace parser {

namespace x3 = boost::spirit::x3;

using lat_type = x3::rule<class lattag, ast::lat_data>;
using lon_type = x3::rule<class lontag, ast::lon_data>;
using coord_type = x3::rule<class coordtag, ast::coord_data>;

BOOST_SPIRIT_DECLARE(lat_type, lon_type, coord_type);
}

parser::lat_type lat_();
parser::lon_type lon_();
parser::coord_type coord_();

}
