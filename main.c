// Creation of the Battleship game using threads and processes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
char boardP1[10][10] = {'.'}; // Create the board for player 1
char boardP2[10][10] = {'.'}; // Create the board for player 2

// Create a struct to store the coordinates of the ships
typedef struct{
    int x;
    int y;
}coord;

// Print matrix
void printMatrix(char matrix[10][10], int player){
    if(player == 1)
        printf("Player 1\n");
    else
        printf("Player 2\n");

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

int main(){
    initBoard(boardP1);
    initBoard(boardP2);
    printMatrix(boardP1, 1);

    return 0;
}