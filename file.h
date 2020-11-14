// File input/ouput utilites.
#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>

#include "kml.h"

using CoordinateArray = std::vector<std::array<double, 2>>;

// Reads csv file of coordinates.
size_t readFile(const std::string fn, CoordinateArray& table)
{
    std::ifstream in(fn);
    size_t n = 0;

    if (!in)
    {
        std::cerr << "Unable to read file \"" << fn << "\"\n";
        return n;
    }

    std::string sRow;

    while (getline(in, sRow))
    {
        std::array<double, 2> dRow;
        std::string value;
        std::stringstream ss(sRow);
        char separator = ',';
        int i = 0;

        while (getline(ss, value, separator))
            dRow[i++] = (std::stod(value));
        table.push_back(dRow);
        n++;
    }

    in.close();
    return n;
}

// Writes basic kml file with solution.
void writeFile(const std::string fn, const CoordinateArray& pts, const std::vector<int>& tour)
{
    KML kml;

    // Start file.
    if (kml.open(fn))
    {
        kml.tagOpen("Folder");
        kml.tagStart("Placemark"); kml.write("id="); kml.tagEnd("\"TOUR\"");

        // Change line style.
        kml.tagOpen("Style");
        kml.tagOpen("LineStyle");
        //kml.tagLine("color", "FF0000FF");  // Red.
        kml.tagLine("width", "3.0");
        kml.tagClose(); // linestyle.
        kml.tagClose(); // style.

        // Tour coordinates.
        kml.tagOpen("LineString");
        kml.tagOpen("coordinates");
        for (auto n : tour)
            kml.write(std::to_string(pts[n][1]) + "," + std::to_string(pts[n][0]) + "\n", true);
        kml.write(std::to_string(pts[tour[0]][1]) + "," + std::to_string(pts[tour[0]][0]) + "\n", true);
        kml.tagClose(); // coordinates.
        kml.tagClose(); // linestring.
        kml.tagClose(); // placemark.

        // Add individual points.
        for (size_t i = 0; i < pts.size(); i++)
        {
            kml.tagOpen("Placemark");
            kml.tagLine("name", std::to_string(i + 1));
            kml.tagOpen("Point");
            kml.tagOpen("coordinates");
            kml.write(std::to_string(pts[i][1]) + "," + std::to_string(pts[i][0]) + "\n", true);
            kml.tagClose(); // coordinates.
            kml.tagClose(); // point.
            kml.tagClose(); // placemark.
        }

        // Finish file.
        kml.tagClose(); // folder.
    }
    else
        std::cerr << "Error opening output file.\n";
}

#endif
