#include <omp.h>
#include "lbp.c"

int main(int argc, char *argv[]) {
    argc--;
    argv++;
    Stopif(!argc, return 0, "Please provide an image filename.");
    int width, height, bpp;
    unsigned char* data = stbi_load(*argv, &width, &height, &bpp, GRAY);

    argv++;
    int NTHREADS = *argv ? atoi(*argv) : omp_get_num_procs();
    omp_set_num_threads(NTHREADS);

    int radius = 1;
    int points = 8;
    Point_s *window = coordinates(radius, points);
    int sztable = (int) pow(2, points);
    int *histogram = calloc(sztable, sizeof(int));

    double start = omp_get_wtime();
    #pragma omp parallel for schedule(static, 256)
    for (int i = 0; i < width * height; ++i) {
        int row = i / width;
        int col = i % width;
        if ((row < radius) || (row >= height - radius) || 
            (col < radius) || (col >= width - radius))
            continue;
        int *idxs = neighbor_idxs(window, points, width, i);
        int value = lbp_value(data, idxs, points, i);
        free(idxs);
        #pragma omp atomic //update
        histogram[value]++;
    }
    double end = omp_get_wtime();
    printf("%g", end - start);

    free(histogram);
    free(window);
    stbi_image_free(data);
}
