#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include "train.h"

#define num_of_trains 10
#define num_of_passengers 500

station_t main_station;
int main() {
    printf("Welcome to the automated station:\n");
    printf("======================================================================\n");
    int i = 0;
    pthread_mutex_init(&mutex_allow_train, NULL);
    pthread_t train_thread[num_of_trains];
    pthread_t passenger_thread[num_of_passengers];
    // station_t* main_station;

    station_init(&main_station);
    printf("finished initialization\n");



    //creating passengers
    for(i=0; i<num_of_passengers; i++){
        pthread_create(&passenger_thread[i], NULL, station_wait_train, &main_station);
    }
    printf("finished creating passenger threads\n");

    //creating trains
    for(i=0; i<num_of_trains; i++){
        pthread_create(&train_thread[i], NULL, enter_station, &main_station);
    }
    printf("finished creating train threads\n");

    //wait until all trains leave the station
    for(i=0; i<num_of_trains; i++)
    {
        pthread_join(train_thread[i], NULL);
    }

    /*   for(i=0; i<num_of_passengers; i++)
       {
           pthread_join(passenger_thread[i], NULL);
       }
       printf(" program finished\n");
    */

    return 0;
}