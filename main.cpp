#include "geotypes.h"
#include "geovincenty.h"
#include "fmt/format.h"
#include <cmath>

using geo::Lat;
using geo::Lon;
using geo::Location;
using geo::BearingDistance;
using geo::Azimuth;
using geo::Meters;
using geo::NM;
using geo::Km;
using geo::distance;
using geo::endPoint;


int main()
{
  Location start{Lat{47.060991}, Lon{15.451820}};
  Location end = endPoint(start, Azimuth(198), Meters(200));
  fmt::print("end: {}\n", end);


//  Location begin{Lat{55.477222}, Lon{-4.277500}};
//  Location centre{Lat{55.509444}, Lon{-4.594444}};

//  BearingDistance bd = distance(centre, begin);
//  fmt::print("bd: {}\n", bd);

  std::string lat("553034N");

  return 0;
}
