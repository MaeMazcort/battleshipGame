// Creation of the Battleship game using threads and processes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <mutex.h>
char boardP1[10][10] = {'.'}; // Create the board for player 1
char boardP2[10][10] = {'.'}; // Create the board for player 2
sem_t sem_P1, sem_P2; // Create the semaphores for the players

// Create a struct to store the coordinates of the ships
typedef struct{
    int x;
    int y;
}coord;

// Print matrix
void printMatrix(char matrix[10][10], int player){
    // Print the player
    if(player == 1) printf("Player 1\n");
    else printf("Player 2\n");

    // Print the numbers
    printf("  0 1 2 3 4 5 6 7 8 9\n");
    for(int i = 0; i < 10; i++){
        // Print the letters
        printf("%c ", i + 65);
        for(int j = 0; j < 10; j++)
            printf("%c ", matrix[i][j]);
        printf("\n");
    }
}

// Initialize the board
void initBoard(char matrix[10][10]){
    for(int i = 0; i < 10; i++)
        for(int j = 0; j < 10; j++)
            matrix[i][j] = '.';
}

// TODO: Finish the function looking at previous exercises
void showInstructions(){
    int fd, m;
    // TODO: Check if the second parameter is correct
    fd = open("instructions.txt", 1);

    // Check if the file was opened correctly
    if(fd == -1){
        perror("Error opening the file");
        exit(1);
    }

    // Read the file


    close(fd);
}

// TODO: Finish the function
void placeShips(){
    // FIXME
}

void* playerInputThread(void* arg){
    // FIXME

    return NULL;
}

int main(){
    initBoard(boardP1);
    initBoard(boardP2);
    
    pid_t pid;
    pid = fork();

    if(pid == 0){ // Child process, player 2
        // Initialize the semaphores
        sem_init(&sem_P1, 0, 0);
        sem_init(&sem_P2, 0, 1);

        // Create the threads
        pthread_t inputThread, updateThread;
        pthread_create(&inputThread, NULL, playerInputThread, NULL);
        pthread_create(&updateThread, NULL, playerUpdateThread, NULL);

        // Wait for the threads to finish
        pthread_join(inputThread, NULL);
        pthread_join(updateThread, NULL);

        // Destroy the semaphores
        sem_destroy(&sem_P1);
        sem_destroy(&sem_P2);
    }
    else if(pid > 0){ // Parent process, player 1
        // Initialize the semaphores
        sem_init(&sem_P1, 0, 1);
        sem_init(&sem_P2, 0, 0);

        // Create the threads
        pthread_t inputThread, updateThread;
        pthread_create(&inputThread, NULL, playerInputThread, NULL);
        pthread_create(&updateThread, NULL, playerUpdateThread, NULL);

        // Wait for the threads to finish
        pthread_join(inputThread, NULL);
        pthread_join(updateThread, NULL);

        // Destroy the semaphores
        sem_destroy(&sem_P1);
        sem_destroy(&sem_P2);
    }   
    else{
        perror("Error creating the process");
        exit(1);
    }

    return 0;
}