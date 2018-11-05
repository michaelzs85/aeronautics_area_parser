
#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/variant.hpp>
#include "geoast.h"
#include "annotationhandler.h"

namespace geo {
namespace parser {

namespace x3 = boost::spirit::x3;

namespace _private {

class arcinittag;
class centertag;
class circleinittag;
class septag;
class texttag;

using arcinit_type = x3::rule<arcinittag, double>;
using center_type = x3::rule<centertag>;
using circleinit_type = x3::rule<circleinittag>;
using sep_type = x3::rule<septag>;
using text_type = x3::rule<texttag, std::string>;

BOOST_SPIRIT_DECLARE(arcinit_type, center_type, circleinit_type, sep_type, text_type);

} // namespace _private

class lattag;
class lontag;
class coordtag;
class arctag;
class circletag;
class areatag;

using lat_type = x3::rule<lattag, ast::lat_data>;
using lon_type = x3::rule<lontag, ast::lon_data>;
using coord_type = x3::rule<coordtag, ast::coord_data>;
using arc_type = x3::rule<arctag, ast::arc_data>;
using circle_type = x3::rule<circletag, ast::circle_data>;

using ast_type = boost::variant<std::vector<boost::variant<ast::coord_data, ast::arc_data, std::string>>, ast::circle_data>;
using area_type = x3::rule<areatag, ast_type>;

BOOST_SPIRIT_DECLARE(lat_type, lon_type, coord_type, arc_type, circle_type, area_type);

} // namespace parser

parser::lat_type lat_();
parser::lon_type lon_();
parser::coord_type coord_();
parser::arc_type arc_();
parser::circle_type circle_();
parser::_private::text_type text_();
parser::area_type area_();

using ast_type = parser::ast_type;

} // namespace geo
