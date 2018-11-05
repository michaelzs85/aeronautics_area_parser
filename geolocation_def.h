
#pragma once

#include <boost/spirit/home/x3.hpp>

#include "geoast.h"
#include "geoast_adapted.h"
#include "geolocation.h"
#include "fmt/format.h"

namespace geo {
namespace parser {

namespace x3 = boost::spirit::x3;
using x3::lit;
using x3::double_;
using x3::no_case;

namespace _private {

class arcinittag : annotate_position {};
class centertag : annotate_position {};
class circleinittag : annotate_position {};
class septag : annotate_position{};
class texttag : annotate_position {};

// TODO:: move this internal stuff in _private into separate header file
using two_digit_type = x3::int_parser<double, 10, 2, 2>;
two_digit_type const two_digit = {};

using three_digit_type = x3::int_parser<double, 10, 3, 3>;
three_digit_type const three_digit = {};

template <typename T>
struct no_exp_ureal_polcies : x3::ureal_policies<T>
{
  template <typename Iterator>
  static bool
  parse_exp(Iterator& /*first*/, Iterator const& /*last*/)
  {
    return false;
  }
};

using no_exp_double_type = x3::real_parser<double, no_exp_ureal_polcies<double>>;
no_exp_double_type const no_exp_double_ = {};

struct northsouth_ : x3::symbols<double>
{
  northsouth_()
  {
    add
        ("N", 1.)
        ("n", 1.)
        ("S", -1.)
        ("s", -1.)
        ;
  }
} const northsouth;

struct eastwest_ : x3::symbols<double>
{
  eastwest_()
  {
    add
        ("E", 1.)
        ("e", 1.)
        ("W", -1.)
        ("w", -1.)
        ;
  }
} const eastwest;

struct unit_ : x3::symbols<double>
{
  unit_()
  {
    add
        ("ft", 0.3048)
        ("FT", 0.3048)
        ("nm", 1852.)
        ("NM", 1852.)
        ("km", 1000.)
        ("KM", 1000.)
        ("m", 1.)
        ("M", 1.);
  }
} const unit;

x3::rule<septag> const sep = "seperator";
auto const sep_def = lit("-")|","|";";

auto is_center = [](auto& ctx){
  std::string text = _attr(ctx);
  std::transform(text.begin(), text.end(), text.begin(), [](auto c) -> decltype (c) {return std::tolower(c);});
  if(text.find("center") == std::string::npos
     && text.find("centre") == std::string::npos)
  {
    x3::_pass(ctx) = false;
  }
};

x3::rule<centertag> const center = "arc_center_parser";
auto const center_def = x3::lexeme[no_case[*(~x3::digit)]][is_center];


auto is_arc_init = [](auto& ctx){
  std::string text = _attr(ctx);
  std::transform(text.begin(), text.end(), text.begin(), [](auto c) -> decltype (c) {return std::tolower(c);});

  // there should be the word "arc" in there somewhere
  if(text.find("arc") == std::string::npos)
  {
    x3::_pass(ctx) = false;
    return;
  }
  // if a direction is specified use it, otherwise assume clockwise
  if(text.find("anticlockwise") != std::string::npos
     || text.find("anti-clockwise") != std::string::npos)
  {
    _val(ctx) = -1.;
    return;
  }
  _val(ctx) = 1.;
};

x3::rule<arcinittag, double> const arcinit = "arc_init_parser";
auto const arcinit_def = x3::lexeme[no_case[*(~x3::digit)]][is_arc_init];

auto is_circle_init = [](auto& ctx){
  std::string text = _attr(ctx);
  std::transform(text.begin(), text.end(), text.begin(), [](auto c) -> decltype (c) {return std::tolower(c);});
  if(text.find("circle") == std::string::npos)
  {
    x3::_pass(ctx) = false;
  }
};

x3::rule<circleinittag> const circleinit = "circle_init_parser";
auto const circleinit_def = x3::lexeme[no_case[*(~x3::digit)]][is_circle_init];

x3::rule<texttag, std::string> const text = "textparser";
auto const text_def = x3::lexeme[*(x3::char_ - sep)];

BOOST_SPIRIT_DEFINE(arcinit, center, circleinit, sep, text);
} // namespace private

using namespace _private;

class lattag : annotate_position {};
class lontag : annotate_position {};
class coordtag : annotate_position {};
class arctag : annotate_position {};
class circletag : annotate_position {};
class areatag : annotate_position{};

x3::rule<lattag, ast::lat_data> const lat_ = "latitude";
auto const lat__def = two_digit > -lit("°") > two_digit > -lit("'") > -no_exp_double_ > -lit("\"") > northsouth;

x3::rule<lontag, ast::lon_data> const lon_ = "longitude";
auto const lon__def = three_digit > -lit("°") > two_digit > -lit("'") > -no_exp_double_ > -lit("\"") > eastwest;

x3::rule<coordtag, ast::coord_data> const coord_ = "coordinate";
auto const coord__def = lat_ > -lit(",") > lon_;

x3::rule<arctag, ast::arc_data> const arc_ = "arc";
auto const arc__def = arcinit > double_ > unit > center > coord_ > -no_case[(lit("to")|sep)] > -coord_;

x3::rule<circletag, ast::circle_data> const circle_ = "circle";
auto const circle__def = circleinit > double_ > unit > center > coord_ > x3::omit[*x3::char_];

x3::rule<areatag, ast_type > const area_ = "area";
auto const area__def = ((coord_|arc_|text) % sep) /*| circle_*/ >> -(lit(".")|",");

BOOST_SPIRIT_DEFINE(lat_, lon_, coord_, arc_, circle_, area_);

}

parser::lat_type lat_()
{
  return parser::lat_;
}

parser::lon_type lon_()
{
  return parser::lon_;
}

parser::coord_type coord_()
{
  return parser::coord_;
}

parser::arc_type arc_()
{
  return parser::arc_;
}

parser::circle_type circle_()
{
  return parser::circle_;
}

parser::area_type area_()
{
  return parser::area_;
}


parser::_private::text_type text_()
{
  return parser::_private::text;
}

}
