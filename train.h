//
// Created by mohamed on 07/12/18.
//
typedef struct {
    int passengers_in_station;  //if no passengers in station and the train not full, then train leaves
    bool train_in;      //check if there is a train in the station or not
    sem_t sem_available_seats;
    int available_seats;  //decremented by station_board() when equals zero ,then train leaves
    pthread_cond_t cond_train_in;  //condition to make passengers wait until a train is in the station
    pthread_cond_t cond_train_leave; //condition to make train wait until seats is zero or passengers in station is zero
    int x;                            //to count number of passengers waiting for the train

}station_t;

//for the struct to allow only one thread to change in it at a time
pthread_mutex_t station_mutex;

//for the station to allow only one train to enter it
pthread_mutex_t mutex_allow_train;


//this function is called when train enters the station and open it's doors
void station_load_train(station_t* station, int count);
//this function is called when passengers enters the station
void* station_wait_train(void* station_void);
//this function is called when a passenger sit ina seat
void station_board(station_t* station);
//this function gurrantee that only one train enters the station
void* enter_station(void* station_void);
//initialize the station
void station_init(station_t* station);
