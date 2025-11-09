/*
                            +–––––––––––––––––––––––––––––––––+
                            |            mesh.cpp             |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            Utility function to generate a
                            mesh (grid) of doubles separated by
                            a fixed mesh size h.
*/

#include "../../include/util/mesh.hpp"
#include <stdexcept>


namespace yvan
{
    namespace util
    {
        // Create a mesh of doubles from start to end inclusive, with step size h
        std::vector<double> mesh_vector(double start, double end, double h)
        {
            // Validate inputs
            if (h <= 0.0)
            {
                throw std::invalid_argument("Mesh size h must be positive.");
            }
            if (end < start)
            {
                throw std::invalid_argument("End value must be greater than or equal to start value.");
            }

            std::vector<double> mesh;
            for (double val = start; val <= end; val += h)
            {
                mesh.push_back(val);
            }

            // Ensure the end value is included
            if (mesh.back() < end)
            {
                mesh.push_back(end);
            }

            return mesh;
        }
    }
}