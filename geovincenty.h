
#pragma once

#include "geotypes.h"

namespace geo {

Location endPoint(Location start, Azimuth direction, Meters distance);

BearingDistance distance(Location start, Location end);

} // namespace geo

