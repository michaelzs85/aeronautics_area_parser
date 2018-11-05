#pragma once

#include <boost/optional.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>


namespace geo::ast {

namespace x3 = boost::spirit::x3;

namespace _private
{
struct angle_data : x3::position_tagged
{
  double deg;
  double min;
  boost::optional<double> sec;
  double sign;
};
}

struct lat_data : _private::angle_data {};
struct lon_data : _private::angle_data {};

struct coord_data : x3::position_tagged
{
  lat_data lat;
  lon_data lon;
};

struct arc_data : x3::position_tagged
{
  double direction;
  double radius;
  double unit;
  coord_data center;
  boost::optional<coord_data> target;
};

struct circle_data : x3::position_tagged
{
  double radius;
  double unit;
  coord_data center;
};

using boost::fusion::operator<<;
}

