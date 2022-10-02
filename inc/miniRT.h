#ifndef MINIRT_H
# define MINIRT_H

# include "APIminilibx.h"
# include "vector3.h"
# include "ee_list.h"
# include "light.h"

# include "objects.h"
# include "sphere.h"
# include <limits.h>
# include <math.h>
# include <unistd.h>

#include <stdio.h>

# define WINDOW_NAME "miniRT"
# define WINDOW_X 1000
# define WINDOW_Y 1000
# define IMG_X 1000
# define IMG_Y 1000
# define SCREEN_IMG_X 1900
# define SCREEN_IMG_Y 1024

# define RECURSIVE_DEPTH 5
# define COLOR_BACKGROUND   0X00000000

void    ee_error(int code, const char *str);
void    draw_on_img(t_image *img, t_object *objects);

#endif
