/*******************************************************************************************
*	Some utility functions, mostly useful math
*
*   Created by Davis Teigeler
********************************************************************************************/

#ifndef utilityh
#define utilityh
#define u_InvLerp(a, b, value) (value-a)/(b-a)
#define u_Lerp(a, b, t) a + t * (b - a)
#define u_Sign(val) ((val >> 31) - (-val >> 31))
#endif