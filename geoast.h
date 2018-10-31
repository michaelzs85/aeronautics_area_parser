#pragma once

#include <boost/optional.hpp>
#include <boost/fusion/include/io.hpp>


namespace geo::ast {

namespace _private
{
struct angle_data
{
  double deg;
  double min;
  boost::optional<double> sec;
  double sign;
};
}

struct lat_data : _private::angle_data {};
struct lon_data : _private::angle_data {};

struct coord_data
{
  lat_data lat;
  lon_data lon;
};

using boost::fusion::operator<<;
}

