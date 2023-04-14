#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
    
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "stopif.h"

#define GRAY 1
#define PI 4. * atan(1.)

typedef struct Point_s {
    int x, y;
} Point_s;

/* Point coordinates for circular sampling of neighborhood.
Considering center at (0, 0), points are located at
(x, y) where x = -Rsin(2*PI/P), y = Rcos(2*PI/P)
*/
Point_s *coordinates(int radius, int points) {
    Point_s *window = malloc(points * sizeof(Point_s));
    double step = 2 * PI / points;
    for (int i = 0; i < points; ++i) {
        double x = -radius * cos(i * step);
        double y =  radius * sin(i * step);
        window[i].x = round(x);
        window[i].y = round(y);
    }
    return window;
}

/* Indexes around the central pixel according to coordinates of window
*/
int *neighbor_idxs(Point_s *window, int points, int width, int center) { 
    int *idxs = malloc(points * sizeof(int));
    for (int p = 0; p < points; ++p) {
        int idx = center - width * window[p].y + window[p].x;
        //printf("%d ", idx);
        idxs[p] = idx;
    }
    return idxs;
}

/* LBP value
*/
int lbp_value(unsigned char* data, int *idxs, int points, int center) {
    int value = 0;     
    for (int p = 0; p < points; ++p) {
        int sign = data[idxs[p]] < data[center] ? 0 : 1;
        value = value | sign << (points - p - 1);
    }
    return value;
}

#ifdef test_lbp
int main(int argc, char *argv[]) {
    argc--;
    argv++;
    Stopif(!argc, return 0, "Please provide an image filename.");
    int width, height, bpp;
    unsigned char* data = stbi_load(*argv, &width, &height, &bpp, GRAY);
    //printf("Image info %d %d %d\n", width, height, bpp);

    int radius = 1;
    int points = 8;
    Point_s *window = coordinates(radius, points);
    int sztable = (int) pow(2, points);
    int *histogram = calloc(sztable, sizeof(int));

    struct timeval start;
    gettimeofday(&start, NULL);
    for (int i = 0; i < width * height; ++i) {
        int row = i / width;
        int col = i % width;
        if ((row < radius) || (row >= height - radius) || 
            (col < radius) || (col >= width - radius))
            continue;
        int *idxs = neighbor_idxs(window, points, width, i);
        int value = lbp_value(data, idxs, points, i);
        free(idxs);
        histogram[value] += 1;
    }
    struct timeval end;
    gettimeofday(&end, NULL);
    double delta = ((end.tv_sec  - start.tv_sec) * 1000000u + 
                     end.tv_usec - start.tv_usec) / 1.e6;
    printf("%g", delta);
    free(histogram);
    free(window);
    stbi_image_free(data);
}
#endif
