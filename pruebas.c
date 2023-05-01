// Creation of the Battleship game using threads and processes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
char boardP1[10][10], boardP2[10][10] = {'.'}; // Create the board for player 1 and 2
sem_t sem_P1, sem_P2; // Create the semaphores for the players
int gameOver = 0; // Variable to check if the game is over

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

// Shared data between the threads
typedef struct{
    pthread_mutex_t mutexPlayer, mutexThread;
    int ready;
    coord coordinates;
    int currentPlayer;
} SharedData;

SharedData sharedData;

// Initialize the ships
void initShipsPerPlayer(shipsPerPlayer *ships){
    ships->ship5 = 5;
    ships->ship4 = 4;
    ships->ship3 = 3;
    ships->ship2 = 2;
}

void addDrownedShip(char s, int player){
    switch(s){
        case '5':
            if(player == 1) shipsP1.ship5--;
            else shipsP2.ship5--;
            break;
        case '4':
            if(player == 1) shipsP1.ship4--;
            else shipsP2.ship4--;
            break;
        case '3':
            if(player == 1) shipsP1.ship3--;
            else shipsP2.ship3--;
            break;
        case '2':
            if(player == 1) shipsP1.ship2--;
            else shipsP2.ship2--;
            break;
    }
}

// Check if all the ships are drowned
int allShipsDrowned(shipsPerPlayer ships){
    if(ships.ship5 == 0 && ships.ship4 == 0 && ships.ship3 == 0 && ships.ship2 == 0)
        return 1;
    return 0;
}

// Print matrix
// TODO: Print the matrix with colors
void printMatrix(char matrix[10][10]){
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

// Print opponent matrix
// TODO: Print the matrix with colors
void printBothMatrices(char matrixP1[10][10], char matrixP2[10][10]){
    // Print the numbers
    printf("\n    __Your board__           __Opponent board__\n");
    printf("  0 1 2 3 4 5 6 7 8 9        0 1 2 3 4 5 6 7 8 9\n");
    for(int i = 0; i < 10; i++){
        // Print the letters
        printf("%c ", i + 65);

        // Own matrix
        for(int j = 0; j < 10; j++)
            printf("%c ", matrixP1[i][j]);
        printf("     ");
        printf("%c ", i + 65);

        // Opponent matrix
        for(int j = 0; j < 10; j++)
            if(matrixP2[i][j] == 'X' || matrixP2[i][j] == 'O')
                printf("%c ", matrixP2[i][j]);
            else
                printf(". ");
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

// Convert a string number of one digit to a integer number
int toInt(char digit){
    return digit - '0';
}

int validateCoordinates(char x, int y){
    // Check if the coordinates are valid
    if(toNumber(x) < 0 || toNumber(x) > 9 || y < 0 || y > 9){
        printf("Invalid coordinates, try again\n\n");
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
            printf("\nInvalid orientation, try again: ");
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
        if((orientation == 1 && (9 - coordinates.y + 1) < n ) ||
            (orientation == 2 && (9 - toNumber(coordinates.x) + 1) < n)){
            printf("The ship does not fit in that position, try again\n\n");
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
                printf("There is a overlap with other ship, try again\n\n");
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
    printMatrix(matrix);
    return;
}

void placeShips(char matrix[10][10], int player){
    // Create the ships
    printf("\n------ Player %d, place your ships ------\n", player);
    printMatrix(matrix);

    printf("[Ship of size 5]:\n");
    fillEachShip(5, matrix, player);

    /* printf("\n[Ship of size 4]:\n");
    fillEachShip(4, matrix, player);

    printf("\n[Ship of size 3]:\n");
    fillEachShip(3, matrix, player);

    printf("\n[Ship of size 2]:\n");
    fillEachShip(2, matrix, player); */
    
    return;
}

// Atack the other player
void* playerInputThread(void* arg){

    // Read the input of the player
    while(1){
        // TODO: Add a conditional variable to end the game
        if(gameOver){
            pthread_exit(NULL);
        }

        if(sharedData.ready == 0){
            // Lock the mutex
            pthread_mutex_lock(&(sharedData.mutexThread));

            sleep(2);
            // Read the coordinates and validate them
            printf("\nEnter the row to attack (A-J): ");
            sharedData.coordinates.x = getchar();
            printf("Enter the column to attack (0-9): ");
            scanf("%d", &sharedData.coordinates.y);
            // Clear the buffer
            while(getchar() != '\n');

            // Validate the coordinates
            if(validateCoordinates(sharedData.coordinates.x, sharedData.coordinates.y)){
                
                // Critical section
                sharedData.ready = 1;
                //printf("The ready flag was set to 1\n");
                // End of critical section
                pthread_mutex_unlock(&(sharedData.mutexThread));
            }
            else{
                continue;
                // Unlock the mutex
                pthread_mutex_unlock(&(sharedData.mutexThread));
            }

        }
    }
    pthread_exit(NULL);
}

// Update the board of the player
void* playerUpdateThread(void* arg){
    // Updates the board of the player
    while(1){
        // TODO: Add a conditional variable to end the game
        if(gameOver){
            pthread_exit(NULL);
        }

        if(sharedData.ready == 1){
            // Check if the data is ready
            pthread_mutex_lock(&(sharedData.mutexThread));

            if(sharedData.ready){
                // Process the data
                printf("The other player attacked %c%d\n", sharedData.coordinates.x, sharedData.coordinates.y);

                // Check which player is playing
                if(sharedData.currentPlayer == 1){ // Player 1 attacked
                    char objective = boardP2[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y];

                    if(objective != '.' && objective != 'X' && objective != 'O'){
                        printf("Player 1 hit player 2\n");
                        addDrownedShip(objective, 2);
                        boardP2[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'X';
                    }
                    else{
                        printf("Player 1 missed\n");
                        boardP2[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'O';
                    }
                }
                else if(sharedData.currentPlayer == 2){ // Player 2 attacked
                    char objective = boardP1[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y];

                    if(objective != '.' && objective != 'X' && objective != 'O'){
                        printf("Player 2 hit player 1\n");
                        addDrownedShip(objective, 1);
                        boardP1[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'X';
                    }
                    else{
                        printf("Player 2 missed\n");
                        boardP1[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'O';
                    }
                }

                // Prepare for receiving new data
                sharedData.ready = 0;
                //printf("The ready flag was set to 0\n");
                pthread_mutex_unlock(&(sharedData.mutexThread));
            }
            else{
                pthread_mutex_unlock(&(sharedData.mutexThread));
            }

            if(sharedData.currentPlayer == 1)
                printBothMatrices(boardP1, boardP2);
            else
                printBothMatrices(boardP2, boardP1);
        }
    }

    pthread_exit(NULL);
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

    printf("\n------Hola, esta es la matriz del jugador 1------\n");
    printMatrix(boardP1);
    printf("\n------Hola, esta es la matriz del jugador 2------\n\n");
    printMatrix(boardP2);
    printf("Now the game can start:D\n\n\n\n");

    // Init the mutex
    //pthread_mutex_init(&(sharedData.mutexThread), NULL);

    // Declare the threads
    pthread_t threadInputP1, threadUpdateP1;
    pthread_t threadInputP2, threadUpdateP2;

    sharedData.currentPlayer = 1;

    // Create the threads
    pthread_create(&threadInputP1, NULL, playerInputThread, NULL);
    pthread_create(&threadUpdateP1, NULL, playerUpdateThread, NULL);
    
    // Wait for the threads to finish
    pthread_join(threadInputP1, NULL);
    pthread_join(threadUpdateP1, NULL);




    // Destroy the mutex
    //pthread_mutex_destroy(&(sharedData.mutexThread));

    return 0;
}