#include <stdio.h>
#include <mpi.h>
#include <limits.h>

#include "array.h"
#include "globvars.h"
#include "tnode.h"
#include "batch.h"
#include "particles.h"
#include "tools.h"
#include "tree.h"

#include "treedriver.h"


/* definition of primary treecode driver */

void treedriver(struct particles *sources, struct particles *targets,
                int order, double theta, int maxparnode, int batch_size,
                int pot_type, double kappa, int tree_type,
                double *tEn, double *tpeng, double *timetree, int numDevices, int numThreads)
{

    int verbosity=0;
    /* local variables */
    struct tnode *troot = NULL;
    int level;
    double xyzminmax[6];
    
//    int i, j;
    
    /* batch variables */
    struct batch *batches = NULL;
    double batch_lim[6];
    
    /* date and time */
    double time1, time2, time3, timeFillClusters1, timeFillClusters2;

    
    time1 = MPI_Wtime();
    
    level = 0;
    numleaves = 0;
    minlevel = INT_MAX;
    minpars = INT_MAX;
    maxlevel = 0;
    maxpars = 0;
    
    int *tree_inter_list, *tree_inter_list2;
    int *direct_inter_list, *direct_inter_list2;
    struct tnode_array *tree_array = NULL;
    numnodes = 0;
    struct particles *clusters = NULL;
    clusters = malloc(sizeof(struct particles));


    time2 = MPI_Wtime();

    /* call setup to allocate arrays for Taylor expansions and setup global vars */
    if (tree_type == 0) {

        fprintf(stderr, "ERROR: Cluster-particle treecode currently disabled.\n");
        exit(1);

        //setup(targets, order, theta, xyzminmax);
        //cp_create_tree_n0(&troot, targets, 1, targets->num,
        //                  maxparnode, xyzminmax, level);
        //setup_batch(&batches, batch_lim, targets, batch_size);
        //create_source_batch(batches, sources, 1, sources->num,
        //                    batch_size, batch_lim);
        
    } else if (tree_type == 1) {
        if (verbosity>0) printf("Treetype %i: entering setup.\n", tree_type);

        time1 = MPI_Wtime();
        setup(sources, order, theta, xyzminmax);
        time2 = MPI_Wtime();

        if (verbosity>0) printf("Time to setup: %f\n", time2-time1);

        time1 = MPI_Wtime();
        
        #pragma omp parallel
        {
            #pragma omp single
            { 
                pc_create_tree_n0(&troot, sources, 1, sources->num,
                                  maxparnode, xyzminmax, level);
            }
        }

        int final_index = pc_set_tree_index(troot, 0);

        time2 = MPI_Wtime();
        printf("Time to pc_create_tree_n0: %f\n", time2-time1);
        
        time1 = MPI_Wtime();
        tree_array = malloc(sizeof(struct tnode_array));
        tree_array->numnodes = numnodes;
        make_vector(tree_array->ibeg, numnodes);
        make_vector(tree_array->iend, numnodes);
        make_vector(tree_array->numpar, numnodes);
        make_vector(tree_array->x_mid, numnodes);
        make_vector(tree_array->y_mid, numnodes);
        make_vector(tree_array->z_mid, numnodes);
        make_vector(tree_array->x_min, numnodes);

        make_vector(tree_array->y_min, numnodes);
        make_vector(tree_array->z_min, numnodes);
        make_vector(tree_array->x_max, numnodes);
        make_vector(tree_array->y_max, numnodes);
        make_vector(tree_array->z_max, numnodes);
        make_vector(tree_array->level, numnodes);
        make_vector(tree_array->cluster_ind, numnodes);
        make_vector(tree_array->radius, numnodes);
        time2 = MPI_Wtime();
//		printf("Time to make tree_array: %f\n", time2-time1);

        time1 = MPI_Wtime();
        pc_create_tree_array(troot, tree_array);
        time2 = MPI_Wtime();
//        printf("Time to pc_create_tree_array: %f\n", time2-time1);

        time1 = MPI_Wtime();
        setup_batch(&batches, batch_lim, targets, batch_size);
        time2 = MPI_Wtime();
//        printf("Time to setup_batch: %f\n", time2-time1);

        time1 = MPI_Wtime();
        create_target_batch(batches, targets, 1, targets->num,batch_size, batch_lim);
        time2 = MPI_Wtime();
//        printf("Time to create_target_batch: %f\n", time2-time1);
        if (verbosity>0) printf("Exiting create_target_batch.\n");

        timeFillClusters1 = MPI_Wtime();
        if         ((pot_type == 0) || (pot_type==1))  {
            fill_in_cluster_data(   clusters, sources, troot, order,
                                  numDevices, numThreads, tree_array);

        } else if  ((pot_type == 2) || (pot_type==3)) {
            fill_in_cluster_data_SS(clusters, sources, troot, order);

        } else if  ((pot_type == 4) || (pot_type==5)) {
            fill_in_cluster_data_hermite(clusters, sources, troot, order);

        } else if  ((pot_type == 6) || (pot_type==7)) {
            fill_in_cluster_data_hermite_SS(clusters, sources, troot, order);
        }
        timeFillClusters2 = MPI_Wtime();
        timeFillClusters1 = timeFillClusters2-timeFillClusters1;
//        printf("Time to compute modified weights(s):  %f\n", timeFillClusters1);

    }

    time2 = MPI_Wtime();
    timetree[0] = time2-time1;

    if (verbosity>0) {
        printf("Tree creation (s):  %f\n\n", time2-time1);
        printf("Tree information: \n\n");

        printf("                      numpar: %d\n", troot->numpar);
        printf("                       x_mid: %e\n", troot->x_mid);
        printf("                       y_mid: %e\n", troot->y_mid);
        printf("                       z_mid: %e\n\n", troot->z_mid);
        printf("                      radius: %f\n\n", troot->radius);
        printf("                       x_len: %e\n", troot->x_max - troot->x_min);
        printf("                       y_len: %e\n", troot->y_max - troot->y_min);
        printf("                       z_len: %e\n\n", troot->z_max - troot->z_min);
        printf("                      torder: %d\n", torder);
        printf("                       theta: %f\n", theta);
        printf("                  maxparnode: %d\n", maxparnode);
        printf("               tree maxlevel: %d\n", maxlevel);
        printf("               tree minlevel: %d\n", minlevel);
        printf("                tree maxpars: %d\n", maxpars);
        printf("                tree minpars: %d\n", minpars);
        printf("            number of leaves: %d\n", numleaves);
        printf("             number of nodes: %d\n", numnodes);
        printf("           number of devices: %d\n", numDevices);
        printf("           target batch size: %d\n", batch_size);
        printf("           number of batches: %d\n\n", batches->num);
    }


    if (tree_type == 0) {
        fprintf(stderr, "ERROR: Cluster-particle treecode is currently not disabled.\n");
        exit(1);
//        if (pot_type == 0) {
//            cp_treecode(troot, batches, sources, targets,
//                        tpeng, tEn, &timetree[1]);
//        }
//        } else if (pot_type == 1) {
//            cp_treecode_yuk(troot, batches, sources, targets, kappa,
//                            tpeng, tEn, &timetree[1]);
//        }
    } else if (tree_type == 1) {

    	MPI_Init(&argc, &argv);
		int ierr;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    	// Get number of nodes for each processor's LET
		int numNodesOnProc[numProcs];
    	MPI_Allgather(numnodes, 1, MPI_INT,
    			numNodesOnProc, 1, MPI_INT,
                MPI_Comm comm)

    	int pointsPerCluster = (order+1)*(order+1)*(order+1)
		int let_sources_length, let_clusters_length, let_tree_array_length;
    	// Allocate structures before MPI round robin

    	struct tnode_array *let_tree_array = NULL;
    	let_tree_array_length=numnodes;
    	let_tree_array = malloc(sizeof(struct tnode_array));
    	allocate_tree_array(let_tree_array,let_tree_array_length); // start by allocating let_tree_array with size numnodes

		struct particles *let_clusters = NULL;
		let_clusters_length=numnodes*pointsPerCluster;
		let_clusters = malloc(sizeof(struct particles)); // let_clusters will hold all cluster data for LET
		allocate_cluster(let_clusters,let_clusters_length);

		struct particles *let_sources = NULL;
		let_sources = malloc(sizeof(struct particles));  // let_sources will hold all source nodes needed for direct interactions
		let_sources_length = troot->numpar
		allocate_sources(let_sources,let_sources_length);



		// Fill LET with own data

		for (int i=0; i<numnodes*pointsPerCluster; i++){
			// Fill in clusters
			let_clusters->x[i] = cluster->x[i];
			let_clusters->y[i] = cluster->y[i];
			let_clusters->z[i] = cluster->z[i];
			let_clusters->q[i] = cluster->q[i];
			let_clusters->w[i] = cluster->w[i];
		}
		for (int i=0; i<troot->numpar; i++){
			// Fill in own sources
			let_sources->x[i] = sources->x[i];
			let_sources->y[i] = sources->y[i];
			let_sources->z[i] = sources->z[i];
			let_sources->q[i] = sources->q[i];
			let_sources->w[i] = sources->w[i];
		}



    	// Perform MPI round robin, filling LET with remote data
		for (int procID=1;procID<numProcs;procID++){


			getFrom = (numProcs+rank-procID)%numProcs;

			// Allocate remote_tree_array
			struct tnode_array *remote_tree_array = NULL;
			remote_tree_array = malloc(sizeof(struct tnode_array));
			allocate_tree_array(remote_tree_array,numNodesOnProc[procID]); // start by allocating let_tree_array with size numnodes

			// Get remote_tree_array


			// Construct masks
			int * approx_mask, * direct_mask;
			make_vector(approx_mask,numNodesOnProc[procID]);
			make_vector(direct_mask,numNodesOnProc[procID]);
			interaction_masks( remote_tree_array, batches, approx_mask, direct_mask);

		    // Reallocate structs to hold new data

			for (int i=0;i<remote_tree_array_length,i++){
				if (approx_mask[i]==1) let_clusters_length+=pointsPerCluster
			}


			// Use masks to get remote data



			// Fill in LET


			free_vector(approx_mask);
			free_vecotr(direct_mask);

		} // end loop over numProcs


    	// Compute interaction lists based on LET
        make_vector(tree_inter_list, batches->num * let_tree_array->numnodes);
        make_vector(direct_inter_list, batches->num * let_tree_array->numnodes);
    	pc_make_interaction_list(let_tree_array, batches, tree_inter_list,  direct_inter_list);



        // After filling LET, call interaction_list_treecode
    	time1 = MPI_Wtime(); // start timer for tree evaluation
        if (pot_type == 0) {
            if (verbosity>0) printf("Entering particle-cluster, pot_type=0 (Coulomb).\n");
            pc_interaction_list_treecode(tree_array, clusters, batches,
                                         tree_inter_list, direct_inter_list, sources, targets,
                                         tpeng, tEn, numDevices, numThreads);

        } else if (pot_type == 1) {
            if (verbosity>0) printf("Entering particle-cluster, pot_type=1 (Yukawa).\n");
            pc_interaction_list_treecode_yuk(tree_array, clusters, batches,
                                             tree_inter_list, direct_inter_list, sources, targets,
                                             tpeng, kappa, tEn, numDevices, numThreads);

        } else if (pot_type == 2) {
            if (verbosity>0) printf("Entering particle-cluster, pot_type=2 (Coulomb w/ singularity subtraction).\n");
            pc_treecode_coulomb_SS(troot, batches, sources, targets,clusters,
                                   kappa, tpeng, tEn, numDevices, numThreads);
//          pc_interaction_list_treecode_Coulomb_SS(tree_array, clusters, batches,
//                                                  tree_inter_list, direct_inter_list, sources, targets,
//                                                  tpeng, kappa, tEn, numDevices, numThreads);

        } else if (pot_type == 3) {
            if (verbosity>0) printf("Entering particle-cluster, pot_type=3 (Yukawa w/ singularity subtraction).\n");
            pc_treecode_yuk_SS(troot, batches, sources, targets,clusters,
                               kappa, tpeng, tEn, numDevices, numThreads);

        } else if (pot_type == 4) {
            if (verbosity>0) printf("Entering particle-cluster, pot_type=4 (Coulomb Hermite).\n");
            pc_interaction_list_treecode_hermite_coulomb(tree_array, clusters, batches,
                                                    tree_inter_list, direct_inter_list, sources, targets,
                                                    tpeng, tEn, numDevices, numThreads);

        }else if (pot_type == 5) {
            if (verbosity>0) printf("Entering particle-cluster, pot_type=4 (Yukawa Hermite).\n");
            pc_interaction_list_treecode_hermite_yukawa(tree_array, clusters, batches,
                                                    tree_inter_list, direct_inter_list, sources, targets,
                                                    tpeng, kappa, tEn, numDevices, numThreads);

        }else if (pot_type == 6) {
            if (verbosity>0) printf("Entering particle-cluster, pot_type=6 (Coulomb Hermite w/ singularity subtraction).\n");
            pc_treecode_hermite_coulomb_SS(troot, batches, sources, targets,clusters,
                                           kappa, tpeng, tEn, numDevices, numThreads);
        }
        
        reorder_energies(batches->reorder, targets->num, tEn);
    }


    time2 = MPI_Wtime();  // end time for tree evaluation
    timetree[3] = time2-time1; //+ timetree[0];

    if (verbosity>0) printf("Time to compute: %f\n", time2-time1);


    cleanup(troot);

    // free interaction lists
    free_vector(tree_inter_list);
    free_vector(direct_inter_list);

    // free clusters
    free_vector(clusters->x);
    free_vector(clusters->y);
    free_vector(clusters->z);
    free_vector(clusters->q);
    free_vector(clusters->w);
    free(clusters);

    // free tree_array
    free_vector(tree_array->ibeg);
    free_vector(tree_array->iend);
    free_vector(tree_array->x_mid);
    free_vector(tree_array->y_mid);
    free_vector(tree_array->z_mid);
    free_vector(tree_array->x_min);
    free_vector(tree_array->y_min);
    free_vector(tree_array->z_min);
    free_vector(tree_array->x_max);
    free_vector(tree_array->y_max);
    free_vector(tree_array->z_max);
    free(tree_array);

    // free target batches
    free_vector(batches->reorder);
    free_matrix(batches->index);
    free_matrix(batches->center);
    free_vector(batches->radius);
    free(batches);

    return;

} /* END function treecode */

