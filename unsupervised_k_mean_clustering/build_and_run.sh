#!/bin/bash 

# run (make cluster)
make cluster 

# run the program with the arguments <total_data_points> <no_of_centroids>
DATA_POINTS=10000
CENTROIDS=5
./unsupervised_k_means $DATA_POINTS $CENTROIDS