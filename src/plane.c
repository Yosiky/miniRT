#include "plane.h"

double   intersect_ray_plane(t_vector3 *restrict o, t_vector3 *restrict d,
        void *data)
{
    t_plane   *plane = (t_plane *)data;
    double      res;
    double      dot = vector3_dot(&plane->normal, d);
    t_vector3   v;
    vector3_minus(&v, &plane->point, o);
    double          lenght = vector3_dot(&plane->normal, &v);
    res = INFINITY;
    res = fmin(lenght / dot, res);
    if (EPS < res)
        return (res);
    return (INFINITY);
}

void    get_normal_plane(t_vector3 *restrict res, __attribute__((__unused__))t_vector3 *restrict a,
        void *data)
{
    t_plane *plane = (t_plane *)data;
    
    *res = plane->normal;
}
