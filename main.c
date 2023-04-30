// Creation of the Battleship game using threads and processes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
char boardP1[10][10] = {'.'}; // Create the board for player 1
char boardP2[10][10] = {'.'}; // Create the board for player 2
sem_t sem_P1, sem_P2; // Create the semaphores for the players

// Create a struct to store the coordinates of the ships
typedef struct{
    char x;
    int y;
}coord;

typedef struct{
    int ship5;
    int ship4;
    int ship3;
    int ship2;
}shipsPerPlayer;

shipsPerPlayer shipsP1, shipsP2;

// Initialize the ships
void initShipsPerPlayer(shipsPerPlayer *ships){
    ships->ship5 = 5;
    ships->ship4 = 4;
    ships->ship3 = 3;
    ships->ship2 = 2;
}

// Check if all the ships are drowned
int allShipsDrowned(shipsPerPlayer ships){
    if(ships.ship5 == 0 && ships.ship4 == 0 && ships.ship3 == 0 && ships.ship2 == 0)
        return 1;
    return 0;
}

// Print matrix
// TODO: Print the matrix with colors
void printMatrix(char matrix[10][10], int player){
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
    /*    
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
    */
}

int toNumber(char letter){
    // Convert the letter to uppercase
    if(letter >= 97 && letter <= 122)
        letter -= 32;

    // Convert the letter to a number
    return letter - 65;
}

int validateCoordinates(char x, int y){
    // Check if the coordinates are valid
    if(toNumber(x) < 0 || toNumber(x) > 9 || y < 0 || y > 9){
        printf("Invalid coordinates, try again\n");
        return 0;
    }

    return 1;
}

// TODO: Check if the new coordinates are in the row or column
void fillEachShip(int n, char matrix[10][10], int player){
    int orientation;
    coord coordinates;
    char tam = n + '0';

    // Read the coordinates
    printf("Select (1) to place the ship horizontally or (2) to place it vertically: ");
    
    // Read the orientation
    do {
        scanf("%d", &orientation);
        // Clear the buffer
        while(getchar() != '\n');
        if(orientation != 1 && orientation != 2)
            printf("Invalid orientation, try again: ");
    } while(orientation != 1 && orientation != 2);
    
    // TODO: Check if the inputs are in range
    printf("Enter the starting row (A-J): ");
    coordinates.x = getchar();
    printf("Enter the starting column (0-9): ");
    scanf("%d", &coordinates.y);
    // Clear the buffer
    while(getchar() != '\n');

    // Validate the coordinates
    if(validateCoordinates(coordinates.x, coordinates.y)){
        // Check if it is possible to place the ship
        if((9 - coordinates.y + 1) < n && (9 - toNumber(coordinates.x) + 1) < n){
            printf("The ship does not fit in that position, try again\n");
            fillEachShip(n, matrix, player);
            return;
        }
    }
    else{
        fillEachShip(n, matrix, player);
        return;
    }

    // Horizontal
    if(orientation == 1){
        // Check if there is no overlap
        for(int i = coordinates.y; i <= coordinates.y + n - 1; i++){
            if(matrix[toNumber(coordinates.x)][i] != '.'){
                printf("There is a overlap with other ship, try again\n");
                fillEachShip(n, matrix, player);
                return;
            }
        }

        // Place the ship
        for(int i = 0; i < n; i++){
            if(player == 1)
                boardP1[toNumber(coordinates.x)][coordinates.y + i] = tam;
            else
                boardP2[toNumber(coordinates.x)][coordinates.y + i] = tam;
            
        }
    }
    else{ // Vertical
        // Check if there is no overlap
        for(int i = toNumber(coordinates.x); i <= toNumber(coordinates.x) + n - 1; i++){
            if(matrix[i][coordinates.y] != '.'){
                printf("There is a overlap with other ship, try again\n");
                fillEachShip(n, matrix, player);
                return;
            }
        }

        // Place the ship
        for(int i = 0; i < n; i++){
            if(player == 1)
                boardP1[toNumber(coordinates.x) + i][coordinates.y] = tam;
            else
                boardP2[toNumber(coordinates.x) + i][coordinates.y] = tam;
            
        }
    }

    // Print the matrix
    printMatrix(matrix, player);
    return;
}

void placeShips(char matrix[10][10], int player){
    // Create the ships
    printMatrix(matrix, player);
    printf("\n------ Player %d, place your ships ------\n", player);

    printf("[Ship of size 5]:\n");
    fillEachShip(5, matrix, player);

    printf("\n[Ship of size 4]:\n");
    fillEachShip(4, matrix, player);

    printf("\n[Ship of size 3]:\n");
    fillEachShip(3, matrix, player);

    printf("\n[Ship of size 2]:\n");
    fillEachShip(2, matrix, player);
    
    return;
}

void* playerInputThread(void* arg){
    // Reads the input from the user
    while(1){
        // Read the coordinates and validate them

    }

    return NULL;
}

int main(){
    // Initialize the boards
    initBoard(boardP1);
    initBoard(boardP2);

    // Initialize the ships
    initShipsPerPlayer(&shipsP1);
    initShipsPerPlayer(&shipsP2);

    // Show the instructions
    showInstructions();

    // Place the ships
    placeShips(boardP1, 1);
    placeShips(boardP2, 2);

    printf("\nHola, esta es la matriz del jugador 1\n");
    printMatrix(boardP1, 1);
    printf("\nHola, esta es la matriz del jugador 2\n");
    printMatrix(boardP2, 2);
    
    /* 
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
    */

    return 0;
}