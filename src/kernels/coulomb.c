#include <math.h>
#include <float.h>
#include <stdio.h>

void coulombDirect(int number_of_targets_in_batch, int number_of_source_points_in_cluster,
                   int starting_index_of_target, int starting_index_of_source,
                   double *target_x, double *target_y, double *target_z,
                   double *source_x, double *source_y, double *source_z, double *source_charge, double * source_weight,
                   double *potential, int gpu_async_stream_id)
{

#ifdef OPENACC_ENABLED
    #pragma acc kernels async(gpu_async_stream_id)
    #pragma acc loop independent
    {
#endif
    for (int i = 0; i < number_of_targets_in_batch; i++) {

        double temporary_potential = 0.0;

        double tx = target_x[starting_index_of_target + i];
        double ty = target_y[starting_index_of_target + i];
        double tz = target_z[starting_index_of_target + i];

        for (int j = 0; j < number_of_source_points_in_cluster; j++) {

            double dx = tx - source_x[starting_index_of_source + j];
            double dy = ty - source_y[starting_index_of_source + j];
            double dz = tz - source_z[starting_index_of_source + j];
            double r  = sqrt(dx*dx + dy*dy + dz*dz);

            if (r > DBL_MIN) {
                temporary_potential += source_charge[starting_index_of_source + j] * source_weight[starting_index_of_source + j] / r;
            }
        } // end loop over interpolation points
#ifdef OPENACC_ENABLED
        #pragma acc atomic
#endif
        potential[starting_index_of_target + i] += temporary_potential;
    }
#ifdef OPENACC_ENABLED
    } // end kernel
#endif
    return;
}


void coulombApproximationLagrange(int number_of_targets_in_batch, int number_of_interpolation_points_in_cluster,
                                  int starting_index_of_target, int starting_index_of_cluster,
                                  double *target_x, double *target_y, double *target_z,
                                  double *cluster_x, double *cluster_y, double *cluster_z, double *cluster_charge,
                                  double *potential, int gpu_async_stream_id)
{

#ifdef OPENACC_ENABLED
    #pragma acc kernels async(gpu_async_stream_id)
    #pragma acc loop independent
    {
#endif
    for (int i = 0; i < number_of_targets_in_batch; i++) {

        double temporary_potential = 0.0;

        double tx = target_x[starting_index_of_target + i];
        double ty = target_y[starting_index_of_target + i];
        double tz = target_z[starting_index_of_target + i];

        for (int j = 0; j < number_of_interpolation_points_in_cluster; j++) {

            double dx = tx - cluster_x[ starting_index_of_cluster + j];
            double dy = ty - cluster_y[ starting_index_of_cluster + j];
            double dz = tz - cluster_z[ starting_index_of_cluster + j];
            double r  = sqrt(dx*dx + dy*dy + dz*dz);

            if (r > DBL_MIN){
                temporary_potential += cluster_charge[starting_index_of_cluster + j]/r;
            }
        } // end loop over interpolation points
#ifdef OPENACC_ENABLED
        #pragma acc atomic
#endif
        potential[starting_index_of_target + i] += temporary_potential;
    }
#ifdef OPENACC_ENABLED
    } // end kernel
#endif
    return;
}
