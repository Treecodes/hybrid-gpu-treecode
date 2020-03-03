/*
 *Procedures for Particle-Cluster Treecode
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <mpi.h>

#include "array.h"
#include "globvars.h"
#include "struct_nodes.h"
#include "struct_particles.h"
#include "tools.h"

#include "interaction_lists.h"



void cc_compute_interaction_list_1(
                int source_tree_node, const int *source_tree_numpar, const double *source_tree_radius,
                const double *source_tree_x_mid, const double *source_tree_y_mid, const double *source_tree_z_mid,
                const int *source_tree_num_children, const int *source_tree_children,

                int target_tree_node, const int *target_tree_numpar, const double *target_tree_radius,
                const double *target_tree_x_mid, const double *target_tree_y_mid, const double *target_tree_z_mid,
                const int *target_tree_num_children, const int *target_tree_children,

                int **target_approx_list, int **target_direct_list,
                int *sizeof_approx_list, int *sizeof_direct_list,
                int *approx_index_counter, int *direct_index_counter,
                int interpolationOrder, double sizeCheckFactor);

void cc_compute_interaction_list_2(
                int target_tree_node, const int *target_tree_numpar, const double *target_tree_radius,
                const double *target_tree_x_mid, const double *target_tree_y_mid, const double *target_tree_z_mid,
                const int *target_tree_num_children, const int *target_tree_children,

                int source_tree_node, const int *source_tree_numpar, const double *source_tree_radius,
                const double *source_tree_x_mid, const double *source_tree_y_mid, const double *source_tree_z_mid,
                const int *source_tree_num_children, const int *source_tree_children,

                int **target_approx_list, int **target_direct_list,
                int *sizeof_approx_list, int *sizeof_direct_list,
                int *approx_index_counter, int *direct_index_counter,
                int interpolationOrder, double sizeCheckFactor);
                
                
                
void InteractionList_CC_Make(const struct tnode_array *source_tree_array,
                             struct tnode_array *target_tree_array,
                             int ***approx_inter_list_addr, int ***direct_inter_list_addr,
                             int interpolationOrder, double sizeCheckFactor)
{
       int source_tree_numnodes = source_tree_array->numnodes;
       const int *source_tree_numpar = source_tree_array->numpar;
       const double *source_tree_radius = source_tree_array->radius;
       const double *source_tree_x_mid = source_tree_array->x_mid;
       const double *source_tree_y_mid = source_tree_array->y_mid;
       const double *source_tree_z_mid = source_tree_array->z_mid;

       const int *source_tree_num_children = source_tree_array->num_children;
       const int *source_tree_children = source_tree_array->children;
       
       
       int target_tree_numnodes = target_tree_array->numnodes;
       const int *target_tree_numpar = target_tree_array->numpar;
       const double *target_tree_radius = target_tree_array->radius;
       const double *target_tree_x_mid = target_tree_array->x_mid;
       const double *target_tree_y_mid = target_tree_array->y_mid;
       const double *target_tree_z_mid = target_tree_array->z_mid;
       
       const int *target_tree_num_children = target_tree_array->num_children;
       const int *target_tree_children = target_tree_array->children;
       
       int *num_approx_inter = target_tree_array->numApprox;
       int *num_direct_inter = target_tree_array->numDirect;
       

       make_matrix(*approx_inter_list_addr, target_tree_numnodes, 50);
       make_matrix(*direct_inter_list_addr, target_tree_numnodes, 50);
       int **approx_inter_list = *approx_inter_list_addr;
       int **direct_inter_list = *direct_inter_list_addr;


       int *sizeof_approx_inter_list, *sizeof_direct_inter_list;
       make_vector(sizeof_approx_inter_list, target_tree_numnodes);
       make_vector(sizeof_direct_inter_list, target_tree_numnodes);
       

       for (int i = 0; i < target_tree_numnodes; i++) sizeof_approx_inter_list[i] = 50;
       for (int i = 0; i < target_tree_numnodes; i++) sizeof_direct_inter_list[i] = 50;
       
       for (int i = 0; i < target_tree_numnodes; i++)
           for (int j = 0; j < 50; j++)
               approx_inter_list[i][j] = -1;

       for (int i = 0; i < target_tree_numnodes; i++)
           for (int j = 0; j < 50; j++)
               direct_inter_list[i][j] = -1;
               
       for (int i = 0; i < target_tree_numnodes; i++) num_approx_inter[i] = 0;
       for (int i = 0; i < target_tree_numnodes; i++) num_direct_inter[i] = 0;
    
       
       // Fill interaction lists
       // !!!!!!!!!!!!!!!!!!!!
       cc_compute_interaction_list_1(
                   0, source_tree_numpar, source_tree_radius,
                   source_tree_x_mid, source_tree_y_mid, source_tree_z_mid,
                   source_tree_num_children, source_tree_children,

                   0, target_tree_numpar, target_tree_radius,
                   target_tree_x_mid, target_tree_y_mid, target_tree_z_mid,
                   target_tree_num_children, target_tree_children,

                   approx_inter_list, direct_inter_list,
                   sizeof_approx_inter_list, sizeof_direct_inter_list,
                   num_approx_inter, num_direct_inter,
                   interpolationOrder, sizeCheckFactor);


       free_vector(sizeof_approx_inter_list);
       free_vector(sizeof_direct_inter_list);

       return;

} /* END of function pc_treecode */



void InteractionList_CC_MakeRemote(const struct tnode_array *source_tree_array,
                                   struct tnode_array *target_tree_array,
                                   int *approx_list_unpacked,int *approx_list_packed, int *direct_list,
                                   int interpolationOrder, double sizeCheckFactor)
{
    int source_tree_numnodes = source_tree_array->numnodes;
    const int *source_tree_numpar = source_tree_array->numpar;
    const double *source_tree_radius = source_tree_array->radius;
    const double *source_tree_x_mid = source_tree_array->x_mid;
    const double *source_tree_y_mid = source_tree_array->y_mid;
    const double *source_tree_z_mid = source_tree_array->z_mid;

    const int *source_tree_num_children = source_tree_array->num_children;
    const int *source_tree_children = source_tree_array->children;
    
    
    int target_tree_numnodes = target_tree_array->numnodes;
    const int *target_tree_numpar = target_tree_array->numpar;
    const double *target_tree_radius = target_tree_array->radius;
    const double *target_tree_x_mid = target_tree_array->x_mid;
    const double *target_tree_y_mid = target_tree_array->y_mid;
    const double *target_tree_z_mid = target_tree_array->z_mid;
    
    const int *target_tree_num_children = target_tree_array->num_children;
    const int *target_tree_children = target_tree_array->children;


    for (int i = 0; i < source_tree_numnodes; i++) approx_list_unpacked[i] = -1;
    for (int i = 0; i < source_tree_numnodes; i++) approx_list_packed[i] = -1;
    for (int i = 0; i < source_tree_numnodes; i++) direct_list[i] = -1;


    int **temp_approx_inter_list, **temp_direct_inter_list;
    int *sizeof_approx_inter_list, *sizeof_direct_inter_list;
    int *num_approx_inter, *num_direct_inter;

    make_matrix(temp_approx_inter_list, target_tree_numnodes, 50);
    make_matrix(temp_direct_inter_list, target_tree_numnodes, 50);

    make_vector(sizeof_approx_inter_list, target_tree_numnodes);
    make_vector(sizeof_direct_inter_list, target_tree_numnodes);
    
    make_vector(num_approx_inter, target_tree_numnodes);
    make_vector(num_direct_inter, target_tree_numnodes);


    for (int i = 0; i < target_tree_numnodes; i++) sizeof_approx_inter_list[i] = 50;
    for (int i = 0; i < target_tree_numnodes; i++) sizeof_direct_inter_list[i] = 50;
    
    for (int i = 0; i < target_tree_numnodes; i++)
        for(int j = 0; j < 50; j++)
            temp_approx_inter_list[i][j] = -1;

    for (int i = 0; i < target_tree_numnodes; i++)
        for(int j = 0; j < 50; j++)
            temp_direct_inter_list[i][j] = -1;
            
    for (int i = 0; i < target_tree_numnodes; i++) num_approx_inter[i] = 0;
    for (int i = 0; i < target_tree_numnodes; i++) num_direct_inter[i] = 0;
 
    
    // Fill interaction lists
    // !!!!!!!!!!!!!!!!!!!!
    cc_compute_interaction_list_1(
                0, source_tree_numpar, source_tree_radius,
                source_tree_x_mid, source_tree_y_mid, source_tree_z_mid,
                source_tree_num_children, source_tree_children,

                0, target_tree_numpar, target_tree_radius,
                target_tree_x_mid, target_tree_y_mid, target_tree_z_mid,
                target_tree_num_children, target_tree_children,

                temp_approx_inter_list, temp_direct_inter_list,
                sizeof_approx_inter_list, sizeof_direct_inter_list,
                num_approx_inter, num_direct_inter,
                interpolationOrder, sizeCheckFactor);


    for (int j = 0; j < target_tree_numnodes; j++) {
        for (int i = 0; i < num_approx_inter[j]; i++) {

            int source_node_index = temp_approx_inter_list[j][i];
            approx_list_unpacked[source_node_index] = source_node_index;
        }

        for (int i = 0; i < num_direct_inter[j]; i++) {

            int source_node_index = temp_direct_inter_list[j][i];
            direct_list[source_node_index] = source_node_index;
        }
    }


    int approx_counter = 0;
    for (int i = 0; i < source_tree_numnodes; i++) {
        if (approx_list_unpacked[i] > -1) {
            approx_list_packed[approx_counter] = i;
            approx_counter++;
        }
    }


    free_matrix(temp_approx_inter_list);
    free_matrix(temp_direct_inter_list);

    free_vector(sizeof_approx_inter_list);
    free_vector(sizeof_direct_inter_list);
    
    free_vector(num_approx_inter);
    free_vector(num_direct_inter);

    return;

} /* END of function pc_treecode */



/**********************************************/
/************ LOCAL FUNCTIONS *****************/
/**********************************************/


void cc_compute_interaction_list_1(
                int source_tree_node, const int *source_tree_numpar, const double *source_tree_radius,
                const double *source_tree_x_mid, const double *source_tree_y_mid, const double *source_tree_z_mid,
                const int *source_tree_num_children, const int *source_tree_children,     

                int target_tree_node, const int *target_tree_numpar, const double *target_tree_radius,
                const double *target_tree_x_mid, const double *target_tree_y_mid, const double *target_tree_z_mid,
                const int *target_tree_num_children, const int *target_tree_children,

                int **target_tree_list, int **target_direct_list,
                int *sizeof_tree_list, int *sizeof_direct_list,
                int *tree_index_counter, int *direct_index_counter,
                int interpolationOrder, double sizeCheckFactor)
{

    /* determine DIST for MAC test */
    double tx = target_tree_x_mid[target_tree_node] - source_tree_x_mid[source_tree_node];
    double ty = target_tree_y_mid[target_tree_node] - source_tree_y_mid[source_tree_node];
    double tz = target_tree_z_mid[target_tree_node] - source_tree_z_mid[source_tree_node];
    double dist = sqrt(tx*tx + ty*ty + tz*tz);

    if (((source_tree_radius[source_tree_node] + target_tree_radius[target_tree_node]) < dist * sqrt(thetasq))
      && (source_tree_radius[source_tree_node] != 0.00) //) {
      && (sizeCheckFactor*(interpolationOrder+1)*(interpolationOrder+1)*(interpolationOrder+1) < source_tree_numpar[source_tree_node])) {
    /*
 *   * If MAC is accepted and there is more than 1 particle
 *   * in the box, use the expansion for the approximation.
 *   */

        if (tree_index_counter[target_tree_node] >= sizeof_tree_list[target_tree_node]) {
            sizeof_tree_list[target_tree_node] *= 1.5;
            target_tree_list[target_tree_node] = realloc_vector(target_tree_list[target_tree_node],
                                                                sizeof_tree_list[target_tree_node]);
        }

        target_tree_list[target_tree_node][tree_index_counter[target_tree_node]] = source_tree_node;
        tree_index_counter[target_tree_node]++;

    } else {
    /*
 *   * If MAC fails check to see if there are children. If not, perform direct
 *   * calculation. If there are children, call routine recursively for each.
 *   */
        if (target_tree_num_children[target_tree_node] == 0) {

            if (direct_index_counter[target_tree_node] >= sizeof_direct_list[target_tree_node]) {
                sizeof_direct_list[target_tree_node] *= 1.5;
                target_direct_list[target_tree_node] = realloc_vector(target_direct_list[target_tree_node],                                                         sizeof_direct_list[target_tree_node]);
            }

            target_direct_list[target_tree_node][direct_index_counter[target_tree_node]] = source_tree_node;
            direct_index_counter[target_tree_node]++;

        } else {
            for (int i = 0; i < target_tree_num_children[target_tree_node]; i++) {
                cc_compute_interaction_list_2(target_tree_children[8*target_tree_node + i],
                           target_tree_numpar, target_tree_radius,
                           target_tree_x_mid, target_tree_y_mid, target_tree_z_mid,
                           target_tree_num_children, target_tree_children,

                           source_tree_node, source_tree_numpar, source_tree_radius,
                           source_tree_x_mid, source_tree_y_mid, source_tree_z_mid,
                           source_tree_num_children, source_tree_children,

                           target_tree_list, target_direct_list,
                           sizeof_tree_list, sizeof_direct_list,
                           tree_index_counter, direct_index_counter,
                           interpolationOrder, sizeCheckFactor);
            }
        }
    }

    return;

} 



void cc_compute_interaction_list_2(
                int target_tree_node, const int *target_tree_numpar, const double *target_tree_radius,
                const double *target_tree_x_mid, const double *target_tree_y_mid, const double *target_tree_z_mid,
                const int *target_tree_num_children, const int *target_tree_children,

                int source_tree_node, const int *source_tree_numpar, const double *source_tree_radius,
                const double *source_tree_x_mid, const double *source_tree_y_mid, const double *source_tree_z_mid,
                const int *source_tree_num_children, const int *source_tree_children,

                int **target_tree_list, int **target_direct_list,
                int *sizeof_tree_list, int *sizeof_direct_list,
                int *tree_index_counter, int *direct_index_counter,
                int interpolationOrder, double sizeCheckFactor)
{

    /* determine DIST for MAC test */
    double tx = target_tree_x_mid[target_tree_node] - source_tree_x_mid[source_tree_node];
    double ty = target_tree_y_mid[target_tree_node] - source_tree_y_mid[source_tree_node];
    double tz = target_tree_z_mid[target_tree_node] - source_tree_z_mid[source_tree_node];
    double dist = sqrt(tx*tx + ty*ty + tz*tz);

    if (((source_tree_radius[source_tree_node] + target_tree_radius[target_tree_node]) < dist * sqrt(thetasq))
      && (target_tree_radius[source_tree_node] != 0.00) //) {
      && (sizeCheckFactor*(interpolationOrder+1)*(interpolationOrder+1)*(interpolationOrder+1) < target_tree_numpar[target_tree_node])) {
    /*
 *   * If MAC is accepted and there is more than 1 particle
 *   * in the box, use the expansion for the approximation.
 *   */

        if (tree_index_counter[target_tree_node] >= sizeof_tree_list[target_tree_node]) {
            sizeof_tree_list[target_tree_node] *= 1.5;
            target_tree_list[target_tree_node] = realloc_vector(target_tree_list[target_tree_node],
                                                                sizeof_tree_list[target_tree_node]);
        }

        target_tree_list[target_tree_node][tree_index_counter[target_tree_node]] = source_tree_node;
        tree_index_counter[target_tree_node]++;

    } else {
    /*
 *   * If MAC fails check to see if there are children. If not, perform direct
 *   * calculation. If there are children, call routine recursively for each.
 *   */
        if (source_tree_num_children[source_tree_node] == 0) {

            if (direct_index_counter[target_tree_node] >= sizeof_direct_list[target_tree_node]) {
                sizeof_direct_list[target_tree_node] *= 1.5;
                target_direct_list[target_tree_node] = realloc_vector(target_direct_list[target_tree_node],                                                         sizeof_direct_list[target_tree_node]);
            }

            target_direct_list[target_tree_node][direct_index_counter[target_tree_node]] = source_tree_node;
            direct_index_counter[target_tree_node]++;

        } else {
            for (int i = 0; i < source_tree_num_children[source_tree_node]; i++) {
                cc_compute_interaction_list_1(source_tree_children[8*source_tree_node + i],
                           source_tree_numpar, source_tree_radius,
                           source_tree_x_mid, source_tree_y_mid, source_tree_z_mid,
                           source_tree_num_children, source_tree_children,
                           
                           target_tree_node, target_tree_numpar, target_tree_radius,
                           target_tree_x_mid, target_tree_y_mid, target_tree_z_mid,
                           target_tree_num_children, target_tree_children,

                           target_tree_list, target_direct_list,
                           sizeof_tree_list, sizeof_direct_list,
                           tree_index_counter, direct_index_counter,
                           interpolationOrder, sizeCheckFactor);
            }
        }
    }

    return;

}