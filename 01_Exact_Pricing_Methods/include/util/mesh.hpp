/*
                            +–––––––––––––––––––––––––––––––––+
                            |            mesh.hpp             |
                            |  Copyright © 2025 Yvan Richard  |
                            +–––––––––––––––––––––––––––––––––+

                            Utility function to generate a
                            mesh (grid) of doubles separated by
                            a fixed mesh size h.
*/

#ifndef mesh_hpp
#define mesh_hpp

#include <vector>

namespace yvan
{
    namespace util
    {
        // Create a mesh of doubles from start to end inclusive, with step size h
        std::vector<double> mesh_vector(double start, double end, double h);
    }
}

#endif // mesh_hpp