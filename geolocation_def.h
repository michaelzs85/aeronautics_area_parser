
#pragma once

#include <boost/spirit/home/x3.hpp>

#include "geoast.h"
#include "geoast_adapted.h"
#include "geolocation.h"

namespace geo {
namespace parser {

namespace x3 = boost::spirit::x3;

namespace _private {
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

} // namespace private

using namespace _private;
using x3::lit;

x3::rule<class lattag, ast::lat_data> const lat_ = "latitude";
auto const lat__def = two_digit > -lit("°") > two_digit > -lit("'") > -no_exp_double_ > -lit("\"") > northsouth;

x3::rule<class lontag, ast::lon_data> const lon_ = "longitude";
auto const lon__def = three_digit > -lit("°") > two_digit > -lit("'") > -no_exp_double_ > -lit("\"") > eastwest;

x3::rule<class coordtag, ast::coord_data> const coord_ = "coordinate";
auto const coord__def = lat_ > -lit(",") > lon_;


BOOST_SPIRIT_DEFINE(lat_, lon_, coord_);

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

}
