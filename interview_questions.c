#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/* Function Declarations */

/* Task: Given a positive integer, reverse the digits of that number. */
int reverse_number(int num);

/* Task: Given a number, decide if it is an Armstrong number or not */
bool is_num_armstrong(int num);
int count_digits(int num);

/* Task: Given an integer, decide if it is prime or not */
bool is_prime(int num);

/* Task: Print the fibonacci sequence iteratively */
void fib_iterative(int limit);

/* Task: Print the fibonacci sequence recursively */
int fib_recursive(int num);
void print_fib_recursive_result(int limit);

/* Task: Print the fibonacci sequence recursively */
int count_char_occurrence(char * str, char targetChar);

/* Task: 
    I would like to see whether sizeof(struct) will give the size of the 
    struct when the object is passed by reference and not by value.
*/
typedef struct Person {
    char name[20];
    int age;
} person;
void calculate_sizeof_struct_Person(struct Person * person);

/* Task: 
    Given a sentence, split the input into individual words and print
    each word on an individual line 
*/
void split_string_by_delim(char * str, char * delim);

/*********************************************************/

int main() {
    char sent[30] = "I love hummus a lot";
    char delim[2] = " ";
    split_string_by_delim(sent, delim);
    return 0;
}

/*********************************************************/

/**
 * Question: 
 * Given a positive integer, reverse the digits of that number. 
*/
int reverse_number(int num) {
    if(num < 0) {
        return -1;
    } else if(num >= 0 && num <= 9) {
        return num;
    } else {
        int reverse = 0;
        while(num > 0) {
            /* Detecting overflows */
            if(reverse > INT_MAX - (num % 10)) return -1;
            reverse = (reverse * 10) + (num % 10);
            num = num / 10;
        }

        return reverse;
    }
}

/**
 * Question:
 * Given a number, decide if it is an Armstrong number or not 
*/
bool is_num_armstrong(int num) {
    if(num >= 0 && num <= 9) {
        return true;
    } else {
        int digit_count = count_digits(num);
        int temp = num;
        int res = 0;
        while(temp > 0) {
            int rem = temp % 10;
            temp = temp / 10;

            res += (int)pow(rem, digit_count);
        }

        if(res == num) {
            return true;
        }
    }

    return false;
}

int count_digits(int num) {
    int counter = 0;
    while(num > 0) {
        num = num / 10;
        counter++;
    }

    return counter;
}

/**
 * Question: 
 * Given an integer, decide if it is prime or not
*/
bool is_prime(int num) {
    if(num <= 1) {
        return false;
    } else if(num >= 2 && num <= 3) {
        return true;
    } else {
        for(int i = 2; i <= num/2; i++) {
            if((num % i) == 0) {
                return false;
            }
        }
    }

    return true;
}

/**
 * Question:
 * Print the fibonacci sequence iteratively
*/
void fib_iterative(int limit) {
    /* defining the first 2 elements of the series: 0, 1 */
    int zero = 0, one = 1;

    if(limit == 0) {
        fprintf(stdout, "done\n");
    } else if(limit == 1) {
        fprintf(stdout, "%d\n", zero);
    } else {
        int first = 0, second = 1;
        fprintf(stdout, "%d %d ", first, second);
        for(int i = 2; i <= limit; i++) {
            int temp = first; 
            first = second; 
            second += temp; 
            fprintf(stdout, "%d ", second);
        }
    }
}

/**
 * Question: 
 * Print the fibonacci sequence recursively
*/
int fib_recursive(int num) {
    if(num == 0) {
        return 0;
    } else if(num == 1) {
        return 1;
    } else {
        return fib_recursive(num-1) + fib_recursive(num-2);
    }
}

void print_fib_recursive_result(int limit) {
    for(int i = 0; i < limit; i++) {
        fprintf(stdout, "%d ", fib_recursive(i));
    }
}

/**
 * Question:
 * Given a string and a character to search for, count the total number of occurrences of the 
 * character in the provided string
*/
int count_char_occurrence(char * str, char targetChar) {
    if(str == NULL || targetChar == '\0') {
        return -1;
    }

    fprintf(stdout, "Size of str: %lu", sizeof(*(str)));

    int count = 0, i = 0;
    while(str[i] != '\0') {
        if(str[i] == targetChar) {
            count++;
        }
        i++;
    }

    return count; 
}

/**
 * Exploration:
 * I would like to see whether sizeof(struct) will give the size of the 
 * struct when the object is passed by reference and not by value. 
*/
void calculate_sizeof_struct_Person(struct Person * person) {
    fprintf(stdout, "Size: %lu\n", sizeof(*(person)));
}

/**
 * Question:
 * Given a sentence, split the input into individual words and print
 * each word on an individual line
*/
void split_string_by_delim(char str[], char delim[]) {
    char * token = strtok(str, delim);
    while(token != NULL) {
        fprintf(stdout, "%s\n", token);
        token = strtok(NULL, delim);
    }
}   