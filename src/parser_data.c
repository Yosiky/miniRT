#include "miniRT.h"

uint    ee_color_parse(char *str)
{
    uint    dst;
    int     i;
    uint    value;
    const uint  ind[] = {16, 8, 0};

    dst = 0;
    i = 0;
    while (i < 3)
    {
        value = (uint)ft_atoi(str);
        if (value > 255)
            ee_error(9, "ERROR: don't valid color");
        dst += value << ind[i++];
        while (*str != ',' && *str != '\n')
            ++str;
        ++str;
    }
    return (dst);
}

typedef void    (*t_func_creator)(const char *str, void *dst);

void    parse_data(t_file *ptr)
{
    static t_func_creator func[] = {create_light, create_light, create_camera, create_sphere, create_plane, create_cylinder};
    int type;
    uint64_t    count[3];
    t_camera    *camera;
    t_object    *obj;
    t_light     *light;
    int64_t     i;
    uint        flag;

    flag = 0;
    count[0] = count_type_in_file(ptr);
    count[1] = count[0] >> 32;
    count[2] = count[0] & 0xffffffff;
    obj = (t_object *)ee_malloc(sizeof(t_object) * (count[1] + 1));
    light = (t_light *)ee_malloc(sizeof(t_light) * (count[2] + 1));
    camera = (t_camera *)ee_malloc(sizeof(t_camera));
    obj[count[1]].type = OBJ_NONE;
    light[count[2]].type = LIGHT_NONE;
    get_camera(camera);
    get_object(obj);
    get_light(light);
    i = 0;
    while (i < ptr->count)
    {
        type = get_type_line(ptr->data[i]);
        if ((flag & 0x1 && type == PARSE_AMBIENT) || (flag & 0x2 && type == PARSE_POINT) || (flag & 0x4 && type == PARSE_CAMERA))
            ee_error(7, "ERROR: don't valid file");
        flag |= 1 << type;
        if (type == PARSE_AMBIENT || type == PARSE_POINT)
            func[type](ptr->data[i], (void *)&light[--count[2]]);
        else if (type == PARSE_CAMERA)
            func[type](ptr->data[i], (void *)camera);
        else if (type > PARSE_CAMERA)
            func[type](ptr->data[i], (void *)&obj[--count[1]]);
        ++i;
    }
}
