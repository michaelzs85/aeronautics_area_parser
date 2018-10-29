#include "geovincenty.h"

namespace geo {

constexpr double const pi = 3.14159265358979323846;

constexpr double deg2rad(double angle)
{
  return angle * pi /180.;
}

constexpr double rad2deg(double angle)
{
  return angle * 180 / pi;
}

Location endPoint(Location start, Azimuth direction, Meters distance)
{
  using namespace std;
  start.lat = Lat{deg2rad(start.lat)};
  start.lon = Lon{deg2rad(start.lon)};
  direction = Azimuth{deg2rad(direction)};

  constexpr double const a = 6'378'137.0; //length of semi-major axis of the ellipsoid (radius at equator); value according to WGS84
  constexpr double const f = 1./298.257'223'563; // Flattening of ellipsoid; value according to WGS84
  constexpr double const b = (1. - f)*a;
  double const U_1 = atan((1. - f)*tan(start.lat));
  double const sigma_1 = atan2(tan(U_1), cos(direction));
  double const sin_alpha = cos(U_1)*sin(direction);
  double const cos2_alpha = 1. - pow(sin_alpha, 2);
  double const u2 = cos2_alpha * ((pow(a,2) - pow(b,2))/pow(b,2));
  double const k1 = (sqrt(1.+u2)-1.)/(sqrt(1.+u2)+1.);
  double const A = (1. + 0.25*pow(k1,2))/(1. - k1);
  double const B = k1*(1. - (3./8.)*pow(k1,2));

  double sigma = distance/(b*A);
  double delta_sigma = std::numeric_limits<double>::max();
  double two_sigma_m{}, cos_two_sigma_m{};
  //int i = 0;
  while(abs(delta_sigma) > 0.1)
  {
    //fmt::print("obtaining sigma; it: {}\n", i++);
    two_sigma_m = 2*sigma_1 + sigma;
    cos_two_sigma_m = cos(two_sigma_m);
    delta_sigma = B * sin(sigma)*(cos_two_sigma_m + 0.25*B*(cos(sigma)*(-1+2*pow(cos_two_sigma_m, 2))) - (B/6.)*cos_two_sigma_m*(-3.+4*pow(sin(sigma),2))*(-3+4*pow(cos_two_sigma_m,2)));
    sigma = distance/(b*A) + delta_sigma;
  }

  double lat_rad = atan2(sin(U_1)*cos(sigma)+cos(U_1)*sin(sigma)*cos(direction),
                         (1-f)*sqrt(pow(sin_alpha,2) +pow(sin(U_1)*sin(sigma)-cos(U_1)*cos(sigma)*cos(direction),2)));
  Lat lat{rad2deg(lat_rad)};

  double const lambda = atan2(sin(sigma)*sin(direction), cos(U_1)*cos(sigma) - sin(U_1)*sin(sigma)*cos(direction));
  double const C = (f/16.)*cos2_alpha*(4+f*(4-3*cos2_alpha));
  double const L = lambda - (1 - C)*f*sin_alpha*(sigma + C*sin_alpha*(cos_two_sigma_m + C*cos(sigma)*(-1+2*pow(cos_two_sigma_m,2))));
  double lon_rad = L + start.lon;
  Lon lon{rad2deg(lon_rad)};

  //double const alpha_2 = atan2(sin_alpha, -sin(U_1)*sin(sigma)+cos(U_1)*cos(sigma)*cos(direction));

  //fmt::print("alpha_2: {}\n", alpha_2);

  return {lat, lon};
}


BearingDistance distance(Location start, Location end)
{
  using namespace std;
  start.lat = Lat{deg2rad(start.lat)};
  start.lon = Lon{deg2rad(start.lon)};
  end.lat = Lat{deg2rad(end.lat)};
  end.lon = Lon{deg2rad(end.lon)};

  constexpr double const a = 6'378'137.0; //length of semi-major axis of the ellipsoid (radius at equator); value according to WGS84
  constexpr double const f = 1./298.257'223'563; // Flattening of ellipsoid; value according to WGS84
  constexpr double const b = (1. - f)*a;

  double const U_1 = atan((1.-f)*tan(start.lat));
  double const U_2 = atan((1-f)*tan(end.lat));
  double const L = end.lon - start.lon;

  double λ = L;
  double Δλ = std::numeric_limits<double>::max();

  double cos2_α{}, sin_σ{}, cos_2_σ_m{}, cos_σ{}, σ{};

  while (Δλ > pow(10.,-8))
  {
    sin_σ = sqrt(pow(cos(U_2)*sin(λ),2) + pow(cos(U_1)*sin(U_2) - sin(U_1)*cos(U_2)*cos(λ),2));
    cos_σ = sin(U_1)*sin(U_2) + cos(U_1)*cos(U_2)*cos(λ);
    σ = atan2(sin_σ,cos_σ);
    assert(σ > pow(10.,-8));
    double const sin_α = (cos(U_1)*cos(U_2)*sin(λ))/sin_σ;
    cos2_α = 1-pow(sin_α,2);
    cos_2_σ_m = cos(σ) - (2.*sin(U_1)*sin(U_2))/(cos2_α);
    double const C = f/16.*cos2_α*(4+f*(4-3*cos2_α));
    double const new_λ = L + (1-C)*f*sin_α*(σ+C*sin_σ*(cos_2_σ_m + C*cos_σ*(-1+2*pow(cos_2_σ_m, 2))));
    Δλ = new_λ - λ;
    fmt::print("Δλ: {}\n", Δλ);
    λ = new_λ;
  }

  double const u2 = cos2_α*((pow(a,2)-pow(b,2))/pow(b,2));
  double const A = 1 + u2/16384*(4096+u2*(-768+u2*(320-175*u2)));
  double const B = u2/1024*(256+u2*(-128+u2*(74-47*u2)));
  double const Δσ = B*sin_σ*(cos_2_σ_m + 0.25*B*(cos_σ*(-1.+2*pow(cos_2_σ_m,2))-B/6.*cos_2_σ_m*(-3+4*pow(sin_σ,2)*(-3+4*pow(cos_2_σ_m,2)))));
  double const s = b*A*(σ - Δσ);
  double const α_1 = atan2(cos(U_2)*sin(λ), cos(U_1)*sin(U_2) - sin(U_1)*cos(U_2)*cos(λ));
  //double const α_2 = atan2(cos(U_1)*sin(λ), -sin(U_1)*cos(U_2)+cos(U_1)*sin(U_2)*cos(λ));

  return BearingDistance{Azimuth{rad2deg(α_1)}, Meters{s}};
}

}
