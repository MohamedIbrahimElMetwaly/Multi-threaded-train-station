//
// Created by mohamed on 07/12/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include "train.h"

void station_load_train(station_t* station, int count){

    //make the available seats in the train equals count
    pthread_mutex_lock(&station_mutex);
    station->available_seats = count;
    printf("train enters station with %d seats-------------------------------\n",station->available_seats);
    pthread_mutex_unlock(&station_mutex);
    int initial_seats = count;

    //to make no passenger wait for a train
    pthread_mutex_lock(&station_mutex);
    station->train_in = true;
    pthread_mutex_unlock(&station_mutex);
    //unblock any passenger waiting for a train on waiting condition
    pthread_cond_broadcast(&station->cond_train_in);


    //unblock a passenger if it was blocked or give access to number of passengers equals to number of available seats
    while(initial_seats > 0){
        sem_post(&station->sem_available_seats);
        initial_seats--;
    }

    //Alternative of pthread_cond_broadcast(&station->cond_train_in)
    //unblock any thread that is blocked because there is no train
/*    pthread_mutex_lock(&station_mutex);
    while(station->x > 0) {
        pthread_cond_signal(&station->cond_train_in);
        station->x--;
    }
    pthread_mutex_unlock(&station_mutex);
*/

    //check if the train is ready to leave the station
    pthread_mutex_lock(&station_mutex);
    while(station->passengers_in_station != 0 && station->available_seats != 0) {
        printf("train enter waiting condition\n");
        pthread_cond_wait(&station->cond_train_leave, &station_mutex);
    }
    //to make passengers wait on condition variables when enter station
    station->train_in = false;
    //if one or more train enters the station and no passengers in the station reset the semaphore because it will be greater than 0
    //when the next train enters the station so the seats available will be -ve values
    if(station->passengers_in_station == 0)
        sem_init(&station->sem_available_seats, 0,0);

    pthread_mutex_unlock(&station_mutex);

    printf("train leaves station\n");


    //train leaves the station
    /*   pthread_mutex_lock(&station_mutex);
       station->train_in = false;
       pthread_mutex_unlock(&station_mutex);
   */


}

void* station_wait_train(void* station_void){
    station_t* station = station_void;

    //increase number of passengers in station
    pthread_mutex_lock(&station_mutex);
    station->passengers_in_station++;
    printf("Passenger enters station\n");
    pthread_mutex_unlock(&station_mutex);

    //check if there is a train in the station to make the passenger wait or not
    pthread_mutex_lock(&station_mutex);
    while(!(station->train_in)){
        station->x++;       // used for the alternative of pthread_cond_broadcast()
        pthread_cond_wait(&station->cond_train_in, &station_mutex);
    }
    pthread_mutex_unlock(&station_mutex);



    //Allow passengers equals to available seats to enter the train and block others
    sem_wait(&station->sem_available_seats);
    //passenger take a seat
    station_board(station);
    pthread_exit(0);

}

void station_board(station_t* station){

    pthread_mutex_lock(&station_mutex);
    station->available_seats--;
    station->passengers_in_station--;
    printf("Passenger takes a seat, seats left: %d  passengers left in station: %d\n",station->available_seats, station->passengers_in_station);
    //check condition to allow the train leaves the station if there is no seats in the train or no passengers in the station
    if(station->available_seats == 0 || station->passengers_in_station == 0 ){
        printf("allow train to leave\n");
        pthread_cond_signal(&station->cond_train_leave);
    }
    pthread_mutex_unlock(&station_mutex);
}

void* enter_station(void* station_void){

    station_t* station = station_void;
    int lower_limit = 1, upper_limit = 10;
    //generate a random number of available seats for each train
    int avialable_seats = 2;//(rand() %(upper_limit - lower_limit +1)) + lower_limit;

    //allow only one train to enter the station
    pthread_mutex_lock(&mutex_allow_train);
    station_load_train(station, avialable_seats);
    pthread_mutex_unlock(&mutex_allow_train);
    pthread_exit(0);
}

void station_init(station_t* station){

    station = malloc(sizeof(station_t));
    station->train_in = false;
    station->passengers_in_station = 0;
    station->available_seats = 0;
    station->x = 0;
    sem_init(&station->sem_available_seats, 0,0);
    pthread_mutex_init(&station_mutex, NULL);
    pthread_cond_init(&station->cond_train_in, NULL);
    pthread_cond_init(&station->cond_train_leave, NULL);


}