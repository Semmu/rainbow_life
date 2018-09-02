#ifndef UTIL_H
#define UTIL_H

typedef struct
{
    double r; // a fraction between 0 and 1
    double g; // a fraction between 0 and 1
    double b; // a fraction between 0 and 1
} RGB;

typedef struct
{
    double h; // angle in degrees
    double s; // a fraction between 0 and 1
    double v; // a fraction between 0 and 1
} HSV;

HSV RGB2HSV(RGB in);
RGB HSV2RGB(HSV in);

#endif /* UTIL_H */