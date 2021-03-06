/* IMPORTING LIBRARIES */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/* DEFINITIONS */
#define EXPECTED_COMMAND_LINE_ARGS 3
#define BUFFER_SIZE 4096
#define OUTPUT_FILE "OUTPUT.TXT"
#define SUCCESS 0
#define FAILURE 1
#define DELIM ","


/* ERROR MESSAGES */
#define CENTROID_FILE_DOES_NOT_EXIST_ERR_MSG "The centroid file chosen does not exist.\n"
#define ALLOCATION_FAILURE_CENTROID_ARR_ERR_MSG "Failed to allocate memory for the centroid array\n"

#define DATASET_FILE_DOES_NOT_EXIST_ERR_MSG "The dataset file chosen does not exist.\n"
#define ALLOCATION_FAILURE_DATASET_ARR_ERR_MSG "Failed to allocate memory for the dataset array\n"

#define OUTPUT_FILE_CREATION_ERR_MSG "Failure to create a file to persist the results.\n"


/* STRUCTS */

/* 
A record of the coordinates representing the data points given 
in the dataset. 
*/
typedef struct Coordinates {
    double x;
    double y;
} coordinates;

/*
A record that will keep track of a centroid's coordinates as well 
as its name.
*/
typedef struct Centroid {
    struct Coordinates data_points;
    char name[20];
    int no_of_belonging_data_points;
    double sum_of_belonging_x_coordinates;
    double sum_of_belonging_y_coordinates;
} centroid;

/*
A record that will keep track of the dataset in terms of the coordinates
and what centroid they belong to
*/
typedef struct Data_Point {
    struct Coordinates data_points; 
    int centroid_index;
} data_point;


/* CONSTANTS */
const char * DATA_POINTS_FILE_PATH = "./input.csv";
const char * CENTROIDS_FILE_PATH = "./centroid_clusters.csv";
const char * COMMAND_LINE_ARGS_ERR_MSG = "Expected Format:\n./unsupervised_k_means <no_of_data_points> <value_of_k>"; 


/* VARIABLES */
unsigned int total_data_points = 0;
unsigned int total_centroids = 0;


/* DATA STRUCTURES */
centroid * centroid_arr = NULL;
data_point * dataset_arr = NULL;


/* FUNCTION DECLARATIONS */
bool is_command_line_args_valid(int argv, char* args[]);

int load_centroids();
int load_dataset();

double cluster_data();
void run_expectation_step(double * total_error_metric, bool * hasConverged);
void run_maximisation_step();

double calculate_euclidian_distance(data_point * d, centroid * c);
int print_clustering_results(double total_error_metric);
void reset_centroids_records();

void free_memory();


/* ENTRY POINT */
int main(int argv, char* args[]) {
    if(!is_command_line_args_valid(argv, args)) {
        fprintf(stderr, "%s\n", COMMAND_LINE_ARGS_ERR_MSG);
        return EXIT_FAILURE;
    }

    if(load_centroids() != SUCCESS) {
        return EXIT_FAILURE;
    }

    if(load_dataset() != SUCCESS) {
        return EXIT_FAILURE;
    }

    double total_error_metric = cluster_data();
    
    if(print_clustering_results(total_error_metric) != SUCCESS) {
        return EXIT_FAILURE;
    }    

    free_memory();
    return EXIT_SUCCESS;
}


/**
 * Check whether the command line arguments follow the following format
 * ./<program_name> <no_of_data_points> <value_of_k>
 * where both [<no_of_data_points> & <value_of_k>] >= 1
*/
bool is_command_line_args_valid(int argv, char* args[]) {
    if(argv != EXPECTED_COMMAND_LINE_ARGS) {
        return false;
    }

    for(int i = 1; i < argv; i++) {
        int val = atoi(args[i]);
        if(val < 1) {
            return false;
        } 
    }

    total_data_points = (unsigned int) atoi(args[1]);
    total_centroids = (unsigned int) atoi(args[2]);

    return true;
}


/**
 * Load in the centroids from the dataset into the heap. The amount of 
 * memory allocated in the heap is dependent on the value of K provided 
 * by the user. 
 * 
 * Upon unsuccessful allocation, return an error back to the calling method
 * to stop the whole process. 
*/
int load_centroids() {
    /* open the csv file in 'CENTROIDS_FILE_PATH' for reading */
    FILE * centroid_file = fopen(CENTROIDS_FILE_PATH, "r");

    if(centroid_file == NULL) {
        fprintf(stderr, "%s", CENTROID_FILE_DOES_NOT_EXIST_ERR_MSG);
        return FAILURE;
    }

    /* 
    allocate an array of size K that will store all the centroid read from 
    the file specified in 'CENTROIDS_FILE_PATH'
    */
    centroid_arr = (centroid *) calloc(total_centroids, sizeof(centroid)); 
    if(centroid_arr == NULL) {
        fprintf(stderr, "%s", ALLOCATION_FAILURE_CENTROID_ARR_ERR_MSG);
        return FAILURE;
    }

    /* 
    Read the csv file one line at a time, tokenising the input to split at every delim 
    spotted. 

    Assumption:
    I am assuming the correctness of the data input into the file. Therefore, I am not 
    doing rigorous checking. 
    */ 
   char buffer[BUFFER_SIZE];
   char * token;
   int centroid_arr_index = 0;
    while(fgets(buffer, BUFFER_SIZE, centroid_file) != NULL) {
        /* tokenise each line and split by DELIM */
        token = strtok(buffer, DELIM);

        /* retrieve the appropriate centroid from the centroid arr */
        centroid * target_centroid = &(centroid_arr[centroid_arr_index]);

        /* 
        Keep track of which column we are currently at to properly 
        populate each centroid. 
        */
        int current_col = 1; 

        /* walk through the rest of the tokens */
        while(token != NULL) {
            if(current_col == 1) {
                strcpy(target_centroid->name, token);
            } else if(current_col == 2) {
                target_centroid->data_points.x = atof(token);
            } else {
                target_centroid->data_points.y = atof(token);
            }

            current_col++;
            token = strtok(NULL, DELIM);
        }

        centroid_arr_index++;
    }

    /* close the file */
    fclose(centroid_file);
    return SUCCESS;
}


/**
 * Loading the dataset points 
*/
int load_dataset() {
    /* open the file to read the csv */
    FILE * dataset_file = fopen(DATA_POINTS_FILE_PATH, "r");
    if(dataset_file == NULL) {
        fprintf(stderr, "%s", DATASET_FILE_DOES_NOT_EXIST_ERR_MSG);
        return FAILURE;
    }

    /* Allocate an array of type struct Data_Point with the same size as total_data_points */
    dataset_arr = (data_point *) calloc(total_data_points, sizeof(data_point));
    if(dataset_arr == NULL) {
        fprintf(stderr, "%s", ALLOCATION_FAILURE_DATASET_ARR_ERR_MSG);
        return FAILURE;
    }

    /* 
    Parse the csv splitting by the chosen DELIM 
    Input format: (x-coordinate, y-coodrinate)

    Assumption:
    I am assuming the correctness of the data input into the file. Therefore, I am not 
    doing rigorous checking.
    */
    char buffer[BUFFER_SIZE];
    char * token;
    int dataset_arr_index = 0;
    while(fgets(buffer, BUFFER_SIZE, dataset_file) != NULL) {
        /* tokenise the line */
        token = strtok(buffer, DELIM);

        /* Retrieve the current struct to populate */
        data_point * point = &(dataset_arr[dataset_arr_index]);

        /* 
        Keep track of the current coordinate found to populate the appropriate
        coordinate in the struct 
        */
        int current_col = 1;
        while(token != NULL) {
            if(current_col == 1) {
                point->data_points.x = atof(token);
            } else {
                point->data_points.y = atof(token);
            }

            current_col++;
            token = strtok(NULL, DELIM);
        }

        dataset_arr_index++;
    }

    /* close the file */
    fclose(dataset_file);
    return SUCCESS;
}


/**
 * Run the k means algorithm  
*/
double cluster_data() {
    double total_error_metric = 0;
    bool hasConverged = false; 

    while(true) {
        run_expectation_step(&total_error_metric, &hasConverged);

        if(hasConverged) {
            break;
        }

        run_maximisation_step();
    }

    return total_error_metric;
}

/**
 * Executing the expectation step which follows the following 
 * pseudo-code:
 * for each data point:
 *     for each centroid available:
 *          calculate the euclidian distance 
 * 
 * The aim is to locate the centroid with the shortest euclidian distance 
 * with the data point for each data point. In addition, you will need to 
 * keep track of the total error metric which is the sum of all the distances
 * returned by calculate_euclidian_distance()
*/
void run_expectation_step(double * total_error_metric, bool * hasConverged) {
    reset_centroids_records();

    double temp_total_error_metric = 0;
    for(int i = 0; i < total_data_points; i++) {
        data_point * d = &dataset_arr[i];

        double shortest_euclidian_distance;
        int closest_centroid_index; 

        for(int j = 0; j < total_centroids; j++) {
            centroid * c = &centroid_arr[j];

            if(j == 0) {
                shortest_euclidian_distance = calculate_euclidian_distance(d, c);
                closest_centroid_index = j; 
            } else {
                double temp = calculate_euclidian_distance(d, c);

                if(temp < shortest_euclidian_distance) {
                    shortest_euclidian_distance = temp;
                    closest_centroid_index = j;
                }
            }
        }

        /* Record the closest centroid for the current data point */
        d->centroid_index = closest_centroid_index;

        /* 
        Update the sums of coordinates at the closest centroid for the 
        current data point as well as the total points belonging to that 
        centroid
        */
        centroid * c = &centroid_arr[closest_centroid_index];
        c->no_of_belonging_data_points++;
        c->sum_of_belonging_x_coordinates += d->data_points.x;
        c->sum_of_belonging_y_coordinates += d->data_points.y;

        /* update the total_error_metric's value */
        temp_total_error_metric += shortest_euclidian_distance;
    }

    /* 
    Check whether the total error metric in this iteration as the same as 
    the previous one. 
    */
    if(temp_total_error_metric == *(total_error_metric)) {
        *(hasConverged) = true;
    } else {
        *(total_error_metric) = temp_total_error_metric;
    }
}


/**
 * Executing the maximisation step which follows the following
 * pseudo-code:
 * for each centroid in centroids_arr:
 *      calculate the average x-coordinates of all 
 *      belonging data points. 
 * 
 *      calculate the average y-coordinates of all 
 *      belonging data points. 
 * 
 *      update the value of the centroid's x and y
 *      coordinates
*/
void run_maximisation_step() {
    for(int i = 0; i < total_centroids; i++) {
        centroid * c = &centroid_arr[i];

        /* Avoid division by zero (arithmetic error) */
        if(c->no_of_belonging_data_points == 0) {
            continue;
        }

        c->data_points.x = (c->sum_of_belonging_x_coordinates / c->no_of_belonging_data_points);
        c->data_points.y = (c->sum_of_belonging_y_coordinates / c->no_of_belonging_data_points); 
    }
}


/**
 * Given a (d)ata point and a (c)entroid, measure the Euclidian Distance 
 * using the following formula:
 * dist = SQRT([x-Cx]^2 + [y-Cy]^2)
 * 
 * Possible improvement:
 * Utilise bitwise operators to perform power operations as they are more 
 * efficient.
*/
double calculate_euclidian_distance(data_point * d, centroid * c) {
    double x = d->data_points.x, y = d->data_points.y;
    double centroid_x = c->data_points.x, centroid_y = c->data_points.y;
    double distance = sqrt(pow((x - centroid_x), 2) + pow((y - centroid_y), 2));

    return distance;
}


/**
 * A function to reset the values of the following records in the centroids:
 * a. no_of_belonging_data_points
 * b. sum_of_belonging_x_coordinates
 * c. sum_of_belonging_y_coordinates
 * 
 * Note:
 * could be improved by avoiding this iteration and adding a conditional 
 * statement in the expectation step to check whether i = 0 or more and 
 * updating the records accordingly. 
*/
void reset_centroids_records() {
    for(int i = 0; i < total_centroids; i++) {
        centroid * c = &centroid_arr[i];
        c->no_of_belonging_data_points = 0;
        c->sum_of_belonging_x_coordinates = 0;
        c->sum_of_belonging_y_coordinates = 0;
    }
}


/**
 * Print the results of the file to a text file
 * textfile name = OUTPUT_FILE
*/
int print_clustering_results(double total_error_metric) {
    /* open the file for writing */
    FILE * output_file = fopen(OUTPUT_FILE, "w");
    if(output_file == NULL) {
        fprintf(stderr, "%s", OUTPUT_FILE_CREATION_ERR_MSG);
        return FAILURE;
    }

    /* write the total error metric rounded to 3 decimal places */
    fprintf(output_file, "error = %.3f\n", total_error_metric);

    /* 
    iterate through the dataset and print the centroid that the data point
    belongs to.
    */
    for(int i = 0; i < total_data_points; i++) {
        data_point * d = &(dataset_arr[i]);
        centroid * c = &(centroid_arr[d->centroid_index]);
        fprintf(output_file, "%s\n", c->name);
    }

    /* closing the file */
    fclose(output_file);
    return SUCCESS;
}

/**
 * free any dynamically allocated memory which includes the following:
 * a. centroid_arr
 * b. dataset_arr
*/
void free_memory() {
    free(centroid_arr);
    free(dataset_arr);
}