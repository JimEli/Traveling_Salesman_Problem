#ifndef NAVIGATION_H
#define NAVIGATION_H

#define _USE_MATH_DEFINES
#include <math.h>

#define LATITUDE 0
#define LONGITUDE 1

// Convert degree to radian.
double DegToRad(double degree) { return (degree / 180.0) * M_PI; }
// Convert radian to degree.
double RadToDeg(double radian) { return (radian / M_PI) * 180.0; }

// Mod function.
double Mod(double y, double x) 
{
    if (y >= 0.0)
        return (y - x * floor(y / x));
    else
        return y + x * (floor(-(y / x)) + 1.0);
}

// Conversion factors.
// Kilometers.
constexpr double KM_PER_NM{ 1.852 };
constexpr double KM_PER_SM{ 1.609347 };
// Statute miles.
constexpr double SM_PER_KM{ (1.0 / 1.609347) };
constexpr double SM_PER_NM{ 1.150778974 };
// Nautical miles.
constexpr double NM_PER_KM{ (1.0 / 1.852) };
constexpr double NM_PER_SM{ (1.0 / 1.150778974) };

// Calculate rhumbline distance.
double rhumbline(const double& p1lat, const double& p1long, const double& p2lat, const double& p2long)
{
    double d = 0.;

    // Calculate true course.
    double tc = Mod(atan2(DegToRad(p1long - p2long), log(tan(DegToRad(p2lat) / 2.0 + M_PI / 4.0) / tan(DegToRad(p1lat) / 2.0 + M_PI / 4.0))), (M_PI * 2.0));

    // Calculate distance (nm).
    if ((tc > 1.570795 && tc < 1.570797) || (tc > 4.71238 && tc < 4.71239)) // 90 or 270.
        d = 60.0 * fabs(p2long - p1long) * cos(DegToRad(p1lat));
    else
        d = 60.0 * ((p2lat - p1lat) / cos(tc));

    //tc = RadToDeg(tc);

    // Convert to km.
    return d * KM_PER_NM;
}

// Haversine calculation (great circle distance).
double haversine(const double& p1lat, const double& p1long, const double& p2lat, const double& p2long)
{
    // Equatorial radius of earth.
    constexpr static double EARTH_RADIUS_KM{ 6372.8 };
    constexpr static double EARTH_RADIUS_SM{ 3958.76 }; //https://en.wikipedia.org/wiki/Earth_radius#Mean_radius
    constexpr static double EARTH_RADIUS_NM{ 3440.07 };

    // Convert to radians.
    double lat1 = DegToRad(p1lat);
    double lat2 = DegToRad(p2lat);
    double lon1 = DegToRad(p1long);
    double lon2 = DegToRad(p2long);

    // Calculate distance.
    double diffLa = lat2 - lat1;
    double doffLo = lon2 - lon1;
    double c = asin(sqrt(sin(diffLa / 2) * sin(diffLa / 2) + cos(lat1) * cos(lat2) * sin(doffLo / 2) * sin(doffLo / 2)));

    // Return km.
    return EARTH_RADIUS_KM * c * 2.;
}

#endif