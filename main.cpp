/*************************************************************************
* Title: Route Optimizer
* Author: James Eli
* Date: 10/22/2020
*
* Program runs christofides algorithm and 2-opt optimization for solving 
* a traveling salesman problem. Expects input to be a csv file of decimal
* degree lat/long coordinates. Outputs a basic kml file viewable in
* Google earth.
*
* Notes:
*  (1) Compiled with MS Visual Studio 2019 Community (v141).
*************************************************************************
* Change Log:
*   10/20/2020: Initial release. JME
*   10/26/2020: Corrected rhumbline calculation for 90/270. JME
*   10/27/2020: Refactored fillMatrix (does not throw exception). JME
*   10/28/2020: Add file failures to kml. JME
*************************************************************************/
#include <iostream>
#include <iomanip>
#include <vector>
#include <array>

#include "tsp.h"
#include "file.h"
#include "navigation.h"
#include "utility.h"

// Visual Leak Detector.
#include "C:\Program Files (x86)\Visual Leak Detector\include\vld.h"

// Distance equation scaling factor.
double scaleFactor = 1.0;

// fillMatrix return status.
enum Status { FAIL = 0, SUCCESS, RETRY };

Status fillMatrix(const size_t n, const std::vector<std::array<double, 2>>& pts, std::vector<std::vector<int>>& am)
{
	// Fill matrix with cost (haversine distance).
	for (size_t r = 0; r < n; r++)
	{
		for (size_t c = r; c < n; c++)
		{
			if (r != c)
			{
				// Rhumbline distance.
				int d = (int)(rhumbline(pts[r][LATITUDE], pts[r][LONGITUDE], pts[c][LATITUDE], pts[c][LONGITUDE]) * scaleFactor);
					
				if (d <= 0)
				{
					if (pts[r][LATITUDE] == pts[c][LATITUDE] && pts[r][LONGITUDE] == pts[c][LONGITUDE])
					{
						std::cerr << "Input failure: duplicate coordinates.\n";
						return FAIL;
					}
					// Attempt to start over and scale up all distances.
					constexpr double MAX_SCALE_FACTOR{ 64.0 };

					scaleFactor *= 2;
					if (scaleFactor > MAX_SCALE_FACTOR)
					{
						std::cerr << "Scaling failure: insufficient distance between coordinates.\n";
						return FAIL;
					}
					return RETRY;
				}

				am[r][c] = d;
				am[c][r] = d;
			}
			else
				am[r][c] = 0;
		}
	}

	return SUCCESS;
}

int main(int argc, char* argv[])
{
	timer time;  // Time this.

	// Commandline argument?
	if (argc < 2)
	{
		std::cout << "Usage: route input.csv\n";
		std::cout << "Input is a comma delimited file of decimal degree latitude/longitude coordinates.\n";
		std::cout << "Output is a kml file of an optimized route.\n";
		exit(-1);
	}

	// Attempt input from file.
	std::vector<std::array<double, 2>> coordinates;
	size_t numCoords = readFile(argv[1], coordinates);
	if (!numCoords)
		return EXIT_FAILURE;

	// Remove any duplicates.
	coordinates.erase(removeDuplicates(coordinates.begin(), coordinates.end()), coordinates.end());
	size_t n = coordinates.size();
	if (n != numCoords)
		std::cout << (numCoords - n) << " duplicate coordinates removed.\n";

	// Attempt to fill adjacency matrix (scale distances if necessary).
	std::vector<std::vector<int>> adjMatrix;
	adjMatrix.resize(n, std::vector<int>(n, 0));
	Status status;
	while ((status = fillMatrix(n, coordinates, adjMatrix)) != SUCCESS)
		if (status == FAIL)
			return EXIT_FAILURE;

	// Solve traveling salesman problem.
	TSP tsp(n);
	Tour tour = tsp(adjMatrix);

	// Display stats.
	if (scaleFactor != 1.0)
		std::cout << scaleFactor << "x distance scaling applied.\n";
	std::cout << "Number of coordinates: " << n << std::endl;
	std::cout << std::fixed << std::setprecision(1) << "Total distance: ";
	std::cout << ((tour.cost / scaleFactor) * NM_PER_KM) << "nm \nTour path: ";
	for (int i = 0; i < (int)tour.path.size(); i++)
		std::cout << tour.path[i] + 1 << " ";
	std::cout << tour.path[0] + 1 << std::endl;

	// Output kml file.
	writeFile(argv[1], coordinates, tour.path);
	
	return EXIT_SUCCESS;
}
