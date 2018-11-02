#pragma once

#include "geotypes.h"
#include "geovincenty.h"
#include "geoutils.h"
#include "fmt/format.h"
#include <boost/optional.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/io.hpp>
#include <vector>
#include <boost/optional.hpp>
//#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>
//#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
//#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>



namespace geo {

namespace x3 = boost::spirit::x3;
namespace fusion = boost::fusion;

using boost::optional;
using x3::_attr;
using x3::_val;
using x3::lit;
using x3::rule;
using x3::double_;
using x3::omit;
using x3::no_case;
using x3::char_;
using x3::with;
using fusion::at_c;

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


rule<struct SeperatorId> const sep = "seperator_parser";
auto const sep_def = lit("-")|",";


// Latitude parser ddmmssH -> geo::Lat

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

auto lat_from_dms =[](auto& ctx){
  double deg = at_c<0>(_attr(ctx));
  double min = at_c<1>(_attr(ctx));
  double sec = at_c<2>(_attr(ctx)).get_value_or(0.);
  double sig = at_c<3>(_attr(ctx));
  _val(ctx) = Lat{sig*(deg + min/60. + sec/3600.)};
};


rule<struct LatRuleId, Lat> const latitude = "latitude_parser";
auto const latitude_def = (two_digit > -lit("°") > two_digit > -lit("'") > -no_exp_double_ > -lit("\"") > northsouth)[lat_from_dms];


// Longitude parser dddmmssH -> geo::Lon

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

auto lon_from_dms = [](auto& ctx){
  double deg = at_c<0>(_attr(ctx));
  double min = at_c<1>(_attr(ctx));
  double sec = at_c<2>(_attr(ctx)).get_value_or(0.);
  double sig = at_c<3>(_attr(ctx));
  _val(ctx) = Lon{sig*(deg + min/60. + sec/3600.)};
};

rule<struct LonRuleId, Lon> const longitude = "longitude_parser";
auto const longitude_def = (three_digit > -lit("°") > two_digit > -lit("'") > -no_exp_double_ > -lit("\"") > eastwest)[lon_from_dms];

// Coord Parser ddmmssH dddmmssH -> geo::Location



struct CoordRuleId;

rule<CoordRuleId, Location> const coord = "coordinate_parser";
auto const coord_def = latitude > -lit(",") > longitude;

// icao circle description parser

struct unit_ : x3::symbols<double>
{
  unit_()
  {
    add
        ("ft", 0.3048)
        ("nm", 1852.)
        ("NM", 1852.)
        ("km", 1000.)
        ("m", 1.);
  }
} const unit;

auto is_arc_center = [](auto& ctx){
  std::string text = _attr(ctx);
  std::transform(text.begin(), text.end(), text.begin(), [](auto c) -> decltype (c) {return std::tolower(c);});
  if(text.find("center") == std::string::npos
     && text.find("centre") == std::string::npos)
  {
    x3::_pass(ctx) = false;
  }
};

rule<struct ArcCenterId> const arccenter = "arc_center_parser";
auto const arccenter_def = x3::lexeme[no_case[*(~x3::digit)]][is_arc_center];

auto is_circle_init = [](auto& ctx){
  std::string text = _attr(ctx);
  std::transform(text.begin(), text.end(), text.begin(), [](auto c) -> decltype (c) {return std::tolower(c);});
  if(text.find("circle") == std::string::npos)
  {
    x3::_pass(ctx) = false;
  }
};

rule<struct CircleInitId> const circleinit = "circle_init_parser";
auto const circleinit_def = x3::lexeme[no_case[*(~x3::digit)]][is_circle_init];


auto circle_builder = [](auto& ctx){
  Meters radius{at_c<0>(_attr(ctx)) * at_c<1>(_attr(ctx))};
  Location center = at_c<2>(_attr(ctx));
  std::vector<Location> c = create_circle_points(center, radius, 2);
  _val(ctx).insert(std::end(_val(ctx)), std::begin(c), std::end(c));
};


rule<struct CircleDescId, std::vector<Location>> const circle = "circle_parser";
auto const circle_def = (circleinit > double_ > unit
                        > arccenter > coord
                        > x3::omit[*x3::char_])[circle_builder];

// arc parsing

auto is_arc_init = [](auto& ctx){
  std::string text = _attr(ctx);
  std::transform(text.begin(), text.end(), text.begin(), [](auto c) -> decltype (c) {return std::tolower(c);});

  // there should be the word "arc" in there somewhere
  if(text.find("arc") == std::string::npos)
  {
    x3::_pass(ctx) = false;
    return;
  }
  // maybe there is also a direction in there
  if(text.find("anticlockwise") != std::string::npos
     || text.find("anti-clockwise") != std::string::npos)
  {
    _val(ctx) = -1.;
    return;
  }
  _val(ctx) = 1.;

};

rule<struct ArcInitId, double> const arcinit = "arc_init_parser";
auto const arcinit_def = x3::lexeme[no_case[*(~x3::digit)]][is_arc_init];

auto arc_builder = [](auto& ctx){
  Location const& arc_begin = _val(ctx).back();
  double direction = at_c<0>(_attr(ctx));
  //double radius = at_c<1>(_attr(ctx)) * at_c<2>(_attr(ctx));
  Location const& center = at_c<3>(_attr(ctx));
  Location const& arc_end = at_c<4>(_attr(ctx));

  std::vector<Location> arc = create_arc_points(center, arc_begin, arc_end, (direction > 0) ? geo::dir::clockwise : geo::dir::anticlockwise);
  _val(ctx).insert(std::end(_val(ctx)), std::begin(arc), std::end(arc));
};

rule<struct ArcDescId, std::vector<Location>> const arc = "arc_parser";
auto const arc_def = (-sep >> (arcinit > double_ > unit > arccenter > coord
                                     > -no_case[(lit("to")|sep)] > coord) >> -sep)[arc_builder];


// Icao Area Parser -> std::vector<geo::Locations>

rule<struct AreaParserId, std::vector<Location>> const icao_area = "icao_area_parser";
auto const icao_area_def = (+((coord % sep) | arc) | circle) >> -(lit(".")|",");

BOOST_SPIRIT_DEFINE(sep, latitude, longitude, coord, icao_area, circleinit, circle, arcinit, arccenter, arc);

} // namespace geo
