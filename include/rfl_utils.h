#ifndef FL_UTILS_H
#define FL_UTILS_H

#include <rbl_utils.h>

namespace FConstants
{
    static const uint eod = RConstants::eod;
    static const double eps = RConstants::eps;

    static const int maxAge = 100;
    static const int minReproductiveAge = 15;
    static const int maxReproductiveAge = 50;

    enum Direction
    {
        None  = 0,
        Up    = 1 << 0,
        Down  = 1 << 1,
        Left  = 1 << 2,
        Right = 1 << 3
    };
}

typedef int FDirectionMask;

#endif /* FL_UTILS_H */
