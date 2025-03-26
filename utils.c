/*******************************************************************************
 * Name        : utils.c
 * Author      : Lucas Vanzelli
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include "utils.h"
//Type cast to int and then compare them
int cmpr_int(void* first, void* second){
    if(second == NULL){
        return 1;
    }

    //type cast to int
    int first_param = *((int*) first);
    int second_param = *((int*) second);

    //1 = right child, -1 = left child, 0 = right child (but data is equal)
    if (first_param > second_param){
        return 1;
    } 
    else if (second_param > first_param) {
        return -1;
    } else {
        return 0;
    }

}

//Type cast to float and then compare
int cmpr_float(void* first, void* second){
    if(second == NULL){
        return 1;
    }

    //Type cast to float
    float first_param = *((float*) first);
    float second_param = *((float*) second);

    if (first_param > second_param){
        return 1;
    } 
    else if (second_param > first_param) {
        return -1;
    } else {
        return 0;
    }
}

void print_int(void* int_param){
    printf("%d ", *((int*)int_param));
}


void print_float(void* float_param){
    printf("%f ", *((float*)float_param));
}