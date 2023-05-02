// Creation of the Battleship game using threads and processes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#define BUFFER_SIZE 1024 // Buffer size
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

char boardP1[10][10], boardP2[10][10]; // Create the board for player 1 and 2
int hit = 0; // Variable to check if the player hit a ship
int winner = 0; // Variable to check who won the game
int setAlarm = 0; // Variable to check if the alarm was set

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

int gameEnded(){
    if(allShipsDrowned(shipsP1)){
        winner = 2;
        return 1;
    }
    else if(allShipsDrowned(shipsP2)){
        winner = 1;
        return 1;
    }

    return 0;
}

// Print matrix
void printMatrix(char matrix[10][10]){
    // Print the numbers
    printf("          0 1 2 3 4 5 6 7 8 9\n");
    for(int i = 0; i < 10; i++){
        // Print the letters
        printf("        %c ", i + 65);
        for(int j = 0; j < 10; j++)
            printf("%c ", matrix[i][j]);
        printf("\n");
    }
    printf("\n");
}

// Print opponent matrix
void printBothMatrices(char matrixP1[10][10], char matrixP2[10][10]){
    // Print the numbers
    printf("\n    ==Your board==           ==Opponent board==\n\n");
    printf("  0 1 2 3 4 5 6 7 8 9        0 1 2 3 4 5 6 7 8 9\n");
    for(int i = 0; i < 10; i++){
        // Print the letters
        printf("%c ", i + 65);

        // Own matrix
        for(int j = 0; j < 10; j++){
            if(!(matrixP1[i][j] == '.' )){
                if(matrixP1[i][j] == 'X' )
                    printf(RED "%c ", matrixP1[i][j]);
                else if(matrixP1[i][j] == 'O')
                    printf(GREEN "%c ", matrixP1[i][j]);
                else
                    printf(CYAN "%c ", matrixP1[i][j]);

                printf(RESET);
            }
            else
                printf("%c ", matrixP1[i][j]);
        }
        printf("     ");
        printf("%c ", i + 65);

        // Opponent matrix
        for(int j = 0; j < 10; j++){
            if(!(matrixP2[i][j] == '.' )){
                if(matrixP2[i][j] == 'X' )
                    printf(RED "%c ", matrixP2[i][j]);
                else if(matrixP2[i][j] == 'O')
                    printf(GREEN "%c ", matrixP2[i][j]);
                else
                    printf(RESET ". ");

                printf(RESET);
            }
            else
                printf(". ");
        }
        printf("\n");
    }
}

void clearTerminal(){
    system("clear");
    fflush(stdout);
}

// Initialize the board
void initBoard(char matrix[10][10]){
    for(int i = 0; i < 10; i++)
        for(int j = 0; j < 10; j++)
            matrix[i][j] = '.';
}

void pressAnyKey(int signum){
    printf("\n\nPRESS ANY KEY TO CONTINUE...");
    getchar();
}

void showInstructions(){
    clearTerminal();
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    // Open the file
    int fd = open("instructions.txt", 0);

    // Check if the file was opened correctly
    if(fd == -1){
        perror("Error opening the file");
        return;
    }

    // Read the file
    while((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0){
        write(1, buffer, bytesRead);
    }

    // Check if the file was read correctly
    if(bytesRead == -1){
        perror("Error reading the file");
        return;
    }

    close(fd);

    pause();
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
        printf("Invalid coordinates, try again\n\n");
        return 0;
    }

    return 1;
}

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
    printf("\n");
    printMatrix(matrix);
    return;
}

void placeShips(char matrix[10][10], int player){
    clearTerminal();

    // Create the ships
    printf("\n  === PLAYER %d, PLACE YOUR SHIPS ===  \n\n", player);
    printMatrix(matrix);

    printf("[Ship of size 5]:\n\n");
    fillEachShip(5, matrix, player);

    printf("\n[Ship of size 4]:\n\n");
    fillEachShip(4, matrix, player);

    printf("\n[Ship of size 3]:\n\n");
    fillEachShip(3, matrix, player);

    printf("\n[Ship of size 2]:\n\n");
    fillEachShip(2, matrix, player);

    sleep(3);
    
    return;
}

// Atack the other player
void* playerInputThread(void* arg){

    // Read the input of the player
    while(1){
        if(gameEnded()){
            clearTerminal();
            if(winner == 1)
                execl("./gameOver", "gameOver", "1", NULL);
            else
                execl("./gameOver", "gameOver", "2" , NULL);
            exit(1);
        }

        if(sharedData.ready == 0){
            // Lock the mutex
            pthread_mutex_lock(&(sharedData.mutexThread));

            sleep(1);
            // Read the coordinates and validate them
            printf("\nEnter the row to attack (A-J): ");
            sharedData.coordinates.x = getchar();
            sleep(0.5);
            printf("Enter the column to attack (0-9): ");
            scanf("%d", &sharedData.coordinates.y);
            // Clear the buffer
            while(getchar() != '\n');
            printf("\n");

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
        if(gameEnded()){
            clearTerminal();
            if(winner == 1)
                execl("./gameOver", "gameOver", "1", NULL);
            else
                execl("./gameOver", "gameOver", "2" , NULL);
            exit(1);
        }

        if(sharedData.ready == 1){
            // Check if the data is ready
            pthread_mutex_lock(&(sharedData.mutexThread));

            // Check which player is playing
            if(sharedData.currentPlayer == 1){ // Player 1 attacked
                char objective = boardP2[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y];

                if(objective != '.' && objective != 'X' && objective != 'O'){
                    addDrownedShip(objective, 2);
                    boardP2[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'X';
                    hit = 1;
                }
                else{
                    printf("You missed\n");
                    boardP2[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'O';
                    hit = 0;
                }
            }
            else if(sharedData.currentPlayer == 2){ // Player 2 attacked
                char objective = boardP1[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y];

                if(objective != '.' && objective != 'X' && objective != 'O'){
                    addDrownedShip(objective, 1);
                    boardP1[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'X';
                    hit = 1;
                }
                else{
                    printf("You missed\n");
                    boardP1[toNumber(sharedData.coordinates.x)][sharedData.coordinates.y] = 'O';
                    hit = 0;
                }
            }

            // Prepare for other turn
            sharedData.ready = -1;
            pthread_mutex_unlock(&(sharedData.mutexThread));
        }

    }

    pthread_exit(NULL);
}

void processP1(){
    sleep(4);
    clearTerminal();
    printf("\n            ===== PLAYER 1 TURN =====\n");
    printBothMatrices(boardP1, boardP2);

    hit = 0;
    do{
        // Start the turn
        sharedData.ready = 0;

        if(gameEnded()){
            clearTerminal();
            if(winner == 1)
                execl("./gameOver", "gameOver", "1", NULL);
            else
                execl("./gameOver", "gameOver", "2" , NULL);
            exit(1);
        }

        // Wait 
        while(sharedData.ready == 0 || sharedData.ready == 1){
            sleep(1);
        }

        if(hit == 1){
            clearTerminal();
            printf("You hit the other player!, you can play again\n");
            printBothMatrices(boardP1, boardP2);
            sleep(1);
        }
    }while(hit);

    // Give the turn to player 2
    sharedData.currentPlayer = 2;
}

void processP2(){
    sleep(4);
    clearTerminal();
    printf("\n            ===== PLAYER 2 TURN =====\n");
    printBothMatrices(boardP2, boardP1);

    hit = -1;
    do{
        // Start the turn
        sharedData.ready = 0;
        
        if(gameEnded()){
            clearTerminal();
            if(winner == 1)
                execl("./gameOver", "gameOver", "1", NULL);
            else
                execl("./gameOver", "gameOver", "2" , NULL);
            exit(1);
        }

        // Wait 
        while(sharedData.ready == 0 || sharedData.ready == 1){
            sleep(1);
        }

        if(hit == 1){
            clearTerminal();
            printf("You hit the other player, you can play again\n\n");
            printBothMatrices(boardP2, boardP1);
            sleep(1);
        }
    }while(hit);

    // Give the turn to player 1
    sharedData.currentPlayer = 1;
}

int main(){
    
    // Show the instructions
    signal(SIGALRM, pressAnyKey);
    alarm(5);
    showInstructions();

    int status;
    pid_t player1Process, player2Process;

    player1Process = fork();
    if(player1Process == 0){
        // Initialize the board
        initBoard(boardP1);

        // Initialize the ship
        initShipsPerPlayer(&shipsP1);

        // Place the ships
        placeShips(boardP1, 1);
    }

    // Wait for the player 1 to finish initializing
    waitpid(player1Process, &status, 0);

    player2Process = fork();
    if(player2Process == 0){
        // Initialize the board
        initBoard(boardP2);

        // Initialize the ship
        initShipsPerPlayer(&shipsP2);

        // Place the ship
        placeShips(boardP2, 2);
    }

    // Wait for the player 2 to finish initializing
    waitpid(player2Process, &status, 0);


    // Init the shared data
    sharedData.ready = -1;
    sharedData.currentPlayer = 1;

    // Declare the threads
    pthread_t threadInput, threadUpdate;

    // Create the threads
    pthread_create(&threadInput, NULL, playerInputThread, NULL);
    pthread_create(&threadUpdate, NULL, playerUpdateThread, NULL);
    

    while(1){
        if(gameEnded()){
            clearTerminal();
            if(winner == 1)
                execl("./gameOver", "gameOver", "1", NULL);
            else
                execl("./gameOver", "gameOver", "2" , NULL);
            exit(1);
        }

        if(sharedData.currentPlayer == 1){
            processP1();
        }
        else if (sharedData.currentPlayer == 2){
            processP2();
        }
    }

    // Wait for the threads to finish
    pthread_join(threadInput, NULL);
    pthread_join(threadUpdate, NULL);


    return 0;
}