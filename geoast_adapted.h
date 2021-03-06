
#pragma once

#include <boost/fusion/include/adapt_struct.hpp>
#include "geoast.h"

BOOST_FUSION_ADAPT_STRUCT(geo::ast::lat_data,
                          deg,
                          min,
                          sec,
                          sign)

BOOST_FUSION_ADAPT_STRUCT(geo::ast::lon_data,
                          deg,
                          min,
                          sec,
                          sign)

BOOST_FUSION_ADAPT_STRUCT(geo::ast::coord_data,
                          lat,
                          lon)

BOOST_FUSION_ADAPT_STRUCT(geo::ast::arc_data,
                          direction,
                          radius,
                          unit,
                          center,
                          target);

BOOST_FUSION_ADAPT_STRUCT(geo::ast::circle_data,
                          radius,
                          unit,
                          center);

