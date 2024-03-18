/*******************************************************************************************
*	Collisions and intersections of common types - for now, only Rays and Axis Aligned
*   Bounding Boxes.
*
*   Created by Davis Teigeler
********************************************************************************************/
#include "components.h"















static bool AABBCollision (Rect3D a, Rect3D b)
{
    return (a.x <= b.w && a.w >= b.x) &&
           (a.y <= b.h && a.h >= b.y) &&
           (a.z <= b.d && a.d >= b.z);
}