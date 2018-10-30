#pragma once

#include <vector>

namespace geo {

class Location;
class Meters;

enum class dir : int {
  clockwise = 1,
  anticlockwise = -1
};

std::vector<Location> create_circle_points(Location center, Meters radius, int pts_per_deg=1);
std::vector<Location> create_arc_points(Location center, Location start, Location end, dir direction=dir::clockwise, int pts_per_deg=1);

} //namespace geo
