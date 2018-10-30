#include "geoutils.h"
#include "geotypes.h"
#include "geovincenty.h"


std::vector<geo::Location> geo::create_circle_points(geo::Location center, geo::Meters radius, int pts_per_deg)
{
  std::vector<geo::Location> circle;
  double stepsize = 1./pts_per_deg;
  for(double angle = 0; angle < 360; angle+=stepsize)
  {
    circle.push_back(endPoint(center, Azimuth{angle}, radius));
  }
  return circle;
}

std::vector<geo::Location> geo::create_arc_points(geo::Location center, geo::Location start, geo::Location end, geo::dir direction, int pts_per_deg)
{
  //fmt::print(">>>---");
  std::vector<geo::Location> arc;
  BearingDistance arcstart = distance(center, start);
  arcstart.bear = Azimuth{(arcstart.bear < 0) ? 360 + arcstart.bear : arcstart.bear};
  BearingDistance arcend = distance(center, end);
  arcend.bear = Azimuth{(arcend.bear < 0) ? 360 + arcend.bear : arcend.bear};
  double stepsize = 1./pts_per_deg;
  double dir = 1.;
  if(direction == geo::dir::anticlockwise)
      dir = -1.;
  dir *= stepsize;
  double angle = arcstart.bear + dir;
  double target_angle = arcend.bear;
  while(!(angle >= target_angle-std::abs(dir)-std::abs(stepsize)/2. && angle <= target_angle+std::abs(dir)+std::abs(stepsize)/2.))
  {
    angle = (angle + dir);
    if(angle >= 360.)
       angle -= 360;
    if(angle < 0)
      angle += 360;
    arc.push_back(endPoint(center, Azimuth{angle}, arcstart.dist));
  }
  arc.push_back(end);
  //fmt::print("--->>>");
  return arc;
}
