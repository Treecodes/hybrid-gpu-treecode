#ifndef H_PARTICLES_H
#define H_PARTICLES_H

/* declaration of struct with tag particles */
struct particles 
{
        int num;
        double *x;
        double *y;
        double *z;
        double *q;
        double *w;  // quadrature weights.  Set equal to 1 if interacting with particles, not performing convolution integral.
        int *order;

        // for hermite only...
        double *qx;
        double *qy;
        double *qz;
        double *qxy;
        double *qyz;
        double *qxz;
        double *qxyz;

};

#endif /* H_PARTICLES_H */
