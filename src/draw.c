#include "miniRT.h"

typedef struct s_some_struct
{
    t_object    *ptr;
    double      value;
}   t_some_struct;

t_vector3   reflect_ray(t_vector3 *r, t_vector3 *n)
{
    t_vector3   res;

    vector3_mul(&res, n, vector3_dot(n, r) * 2);
    vector3_minus(&res, &res, r);
    return (res);
}

static t_some_struct    closestIntersection(t_vector3 *o, t_vector3 *d, t_object *objects, char flag)
{
    double      res;
    double      min_value;
    t_object    *ptr_obj;

    ptr_obj = NULL;
    res = INFINITY;
    while (objects->type != -1)
    {
        min_value = intersect_ray(o, d, objects);
        if (min_value < res)
        {
            res = min_value;
            ptr_obj = objects;
            if (flag)
                return ((t_some_struct){ptr_obj, res});
        }
        ++objects;
    }
    return ((t_some_struct){ptr_obj, res});
}

static t_color  compute_lighting(t_vector3 *p, t_vector3 *n, t_vector3 *v, int s, t_object *objects, t_color *dop)
{
    static t_light      *light = NULL;
    static int          indx;
    static double       intensity;
    static double       max;
    static t_vector3    l;
    double       n_dot_l;
    t_vector3           len;
    t_color             color;
    t_color             buff;

    intensity = 0;
    indx = -1;
    if (light == NULL)
        light = get_light(NULL);
    color_create(&color, 0, 0);
    while (light[++indx].type != LIGHT_NONE)
    {
        if (light[indx].type == LIGHT_AMBIENT)
            color_add(&color, &color, color_create(&buff, light[indx].color, light[indx].intensity));
        else 
        {
            if (light[indx].type == LIGHT_POINT)
            {
                vector3_minus(&l, &(light[indx].position), p);
                vector3_normalized(&l);
                max = 1;
                vector3_minus(&len, p, &(light[indx].position));
            }
            else
            {
                l = light[indx].position;
                max = INFINITY;
            }
            t_some_struct   va = closestIntersection(p, &l, objects, 1);
            if (va.ptr != NULL && ((light[indx].type == LIGHT_POINT && vector3_length(&len) > va.value) || light[indx].type == LIGHT_DERECTIONAL))
                continue ;
            n_dot_l = vector3_dot(n, &l);
            if (n_dot_l < EPS)
                n_dot_l *= -1; // ?????
            if (n_dot_l > EPS)
                color_add(&color, &color, color_create(&buff, light[indx].color, light[indx].intensity * n_dot_l));
            if (s != -1)
            {
                t_vector3 R;
                t_vector3 some;
                vector3_mul(&some, n, 2 * vector3_dot(n, &l));
                vector3_minus(&R, &some, &l);
                double r_dot_v = vector3_dot(&R, v);
                if (r_dot_v > 0)
                    color_add(dop, dop, color_create(&buff, light[indx].color, light[indx].intensity * pow(r_dot_v / (vector3_length(&R) * vector3_length(v)), s)));
            }
        }
    }
    return (color);
}

t_color get_right_color(t_color *a, t_color *b, double c)
{
    t_color res;
    int     i;

    i = -1;
    while (++i < 4)
        res.arr[i] = a->arr[i] * (1 - c) + b->arr[i] * c;
    return (res);
}

static t_color  trace_ray(t_vector3 *o, t_vector3 *d, t_object *objects, int depth)
{
    t_vector3   p;
    t_vector3   n;
    t_some_struct res;
    t_color     local_color;
    t_color     a;
    t_color     b;
    t_color     c;

    c = color_init((double [4]){0, 0, 0, 0});
    res = closestIntersection(o, d, objects, 0);
    if (res.ptr == NULL)
        return ((t_color){0., 0., 0.});
    t_vector3 arr = {o->x + res.value * d->x, o->y + res.value * d->y, o->z + res.value * d->z};
    set_coordinates(&p, arr);
    get_normal(&n, &p, res.ptr);
    vector3_mul(d, d, -1);
    color_create(&a, res.ptr->color, 1);
    b = compute_lighting(&p, &n, d, res.ptr->specular, objects, &c);
    color_mul(&local_color, &a, &b);
    color_add(&local_color, &local_color, &c);
    double  r = res.ptr->reflective;
    if (depth <= 0 || r <= EPS)
        return (local_color);
    t_vector3 vec_r = reflect_ray(d, &n);
    vector3_normalized(&vec_r);
    t_color reflect_color = trace_ray(&p, &vec_r, objects, depth - 1);
    return (get_right_color(&local_color, &reflect_color, r));
}

void    draw_on_img(t_image *img, t_object *objects)
{
    uint         x;
    uint         y;
    int         width_x = img->x / 2;
    int         width_y = img->y / 2;
    t_color     color;
    t_vector3   d;

    y = 0;
    while (y < img->y)
    {
        x = 0;
        while (x < img->x)
        {
            set_coordinates(&d, (t_vector3){ ((double)x - width_x) * 1 / WINDOW_X, ((double)width_y - y) * 1 / WINDOW_Y, 1});
            rotate(&d, 0);
            color = trace_ray(get_viewer(NULL), &d, objects, RECURSIVE_DEPTH);
            ee_mlx_pixel_put(img, x, y, color_double_int(&color));
            ++x;
        }
        ++y;
    }
}
