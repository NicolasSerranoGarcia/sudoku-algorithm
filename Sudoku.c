//This project was created by Nicolas Serrano Garcia. This piece of code aims to solve what's called sudoku, a game that consists of a 9x9
//grid filled with numbers from 1-9 that follows the next basic rules. All the numbers in a row are diferent from each other.
//All the numbers in a column are diferent from each other. All of the 3x3 subcells that form the whole square (9, 3x3 cells that are not conjoined)
//have numbers diferent from each other. Other rules are derived off of these ones
//
//terminology (refered in the file):
//- Trivial cell: a cell where there is only one possible correct number -> e.g. a cell whose row is already filled except for itself
//- posible Entry /posible Number: a way to save the positions where a number can be placed given the actual state of the sudoku
// e.g. if we have a 3x3 submatrix where there is only two cells left (say the numbers 2 and 5 are not placed yet in the submatrix), then two
//posible entries of each of those cells are 2 and 5. This numbers are saved in an associated array (struct -> posibleEntries)
//- free number : when a cell has a unique posible entry of its 3x3 SUBMATRIX. Implies that THAT cell number is the unique place where the number can be placed
//- Ghost number: when inside a 3x3 submatrix, if there are 2 or 3 cells in the same row or col holding the unique posible entries of the submatrix, that means that
//the 3 cells act as if there was a number in that row or column (depending on the orientation). e.g Imagine a 3x3 submatrix with 2 cells in the same row 
//that are not filled. In those 2 cells the number 3 is the only posible entry of all the submatrix. That means that, in that row, regardless of
//where the 3 is placed, the 3 will be in the same row. That means that there cannot be another 3 in that same row. In the file enlightening-drwaings.excalidraw, you have the second drawing
//which explains it visually

#include <stdio.h> //input - output
#include <stdbool.h> //booleans
#include <SDL2/SDL.h> //show solving process on intervals by internal ticks

typedef struct{
    int number;
    bool isFilled;
    int posibleEntries[9];
    int posibleEntriesLength;

}Tcell;

#define kRefreshRate 1000 //ms

void autofill(Tcell sudoku[9][9], int * remainingEmptyCells, char temp);
void fillSudoku(Tcell sudoku[9][9], int * remainingEmptyCells);
void printSudoku(Tcell sudoku[9][9]);
void checkRow(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol);
void checkCol(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol);
void checkSubcell(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol);
void checkFreeNumber(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol, int * remainingEmptyCells);
bool isAligned(int PositionArray[3][2], int PositionArrayLen, char * dir);
void checkGhostNumber(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol);


int main(){

    //initialize all cells to 0
    Tcell sudoku[9][9] = {0};

    //setup the sudoku

    printf("Fill the sudoku you want to solve (if you enter something strange the program won't work, I'm not a QA): \n\n");

    int emptyCells = 81;

    fillSudoku(sudoku, &emptyCells);

    //check that user didn't give us a filled sudoku beforehand
    if(emptyCells == 0){
        printf("Whoops, seems like you don't need my help! Your sudoku is already filled\n\n");
        printSudoku(sudoku);
        return 0;
    }

    //fill all the posible entries of each cell with all numbers up to 9
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(!sudoku[i][j].isFilled)
                for(int k = 0; k < 9; k++){
                    sudoku[i][j].posibleEntries[k] = k + 1;
                    sudoku[i][j].posibleEntriesLength += 1;
            }
        }
    }

    //main algorithm

    printf("The program will purposefuly run slower to show the solving process\n\n");
    printf("Working...\n\n");

    printSudoku(sudoku);
    
 
    Uint32 lastRefresh = SDL_GetTicks();
    Uint32 currentTick = SDL_GetTicks();

    bool solved = false;

    while(!solved){

        currentTick = SDL_GetTicks();

        if((currentTick - lastRefresh) > kRefreshRate){
            //this is the executed code each tick
            for(int i = 0; i < 9; i++){
                for(int j = 0; j < 9; j++){
                    if(!sudoku[i][j].isFilled){

                        //the solving algorithm works by narrowing the possible numbers that can be palced in a particular cell.
                        //Each cell has an associated array that saves all possible numbers that can be placed in it.
                        //When the cell has only one value, then the cell is solved
                        checkRow(sudoku, &sudoku[i][j], i, j);
                        checkCol(sudoku, &sudoku[i][j], i, j);
                        checkSubcell(sudoku, &sudoku[i][j], i, j);
                        checkFreeNumber(sudoku, &sudoku[i][j], i, j, &emptyCells);
                        checkGhostNumber(sudoku, &sudoku[i][j], i, j);                    
                        
                        //check if there is any trivial cell (recall trivial cell from the first comment in this file)
                        if((sudoku[i][j].posibleEntriesLength == 1) && !sudoku[i][j].isFilled){

                            //as a consequence of how I ordered the posibleEntries array, this number must be in the first index

                            sudoku[i][j].isFilled = true;
                            sudoku[i][j].number = sudoku[i][j].posibleEntries[0];
                            sudoku[i][j].posibleEntries[0] = 0;
                            sudoku[i][j].posibleEntriesLength -=1;
                            emptyCells -=1;
                        }
                    }
                }
            }
            printSudoku(sudoku);
            
            if(emptyCells == 0){
                solved = true;
            }

            lastRefresh = SDL_GetTicks();
        } else{
            currentTick = SDL_GetTicks();
        }
    }

    printf("This is your solved sudoku: \n\n");
    printSudoku(sudoku);

    return 0;
}

//used for developing the code (i'm lazy)
void autofill(Tcell sudoku[9][9], int *remainingEmptyCells, char temp){
    if(temp == 'a'){
    sudoku[0][0].number = 5;
    sudoku[0][1].number = 0;
    sudoku[0][2].number = 0;
    sudoku[0][3].number = 4;
    sudoku[0][4].number = 6;
    sudoku[0][5].number = 7;
    sudoku[0][6].number = 3;
    sudoku[0][7].number = 0;
    sudoku[0][8].number = 9;    

    sudoku[1][0].number = 9;
    sudoku[1][1].number = 0;
    sudoku[1][2].number = 3;
    sudoku[1][3].number = 8;
    sudoku[1][4].number = 1;
    sudoku[1][5].number = 0;
    sudoku[1][6].number = 4;
    sudoku[1][7].number = 2;
    sudoku[1][8].number = 7;    

    sudoku[2][0].number = 1;
    sudoku[2][1].number = 7;
    sudoku[2][2].number = 4;
    sudoku[2][3].number = 2;
    sudoku[2][4].number = 0;
    sudoku[2][5].number = 3;
    sudoku[2][6].number = 0;
    sudoku[2][7].number = 0;
    sudoku[2][8].number = 0;    

    sudoku[3][0].number = 2;
    sudoku[3][1].number = 3;
    sudoku[3][2].number = 1;
    sudoku[3][3].number = 9;
    sudoku[3][4].number = 7;
    sudoku[3][5].number = 6;
    sudoku[3][6].number = 8;
    sudoku[3][7].number = 5;
    sudoku[3][8].number = 4;    

    sudoku[4][0].number = 8;
    sudoku[4][1].number = 5;
    sudoku[4][2].number = 7;
    sudoku[4][3].number = 1;
    sudoku[4][4].number = 2;
    sudoku[4][5].number = 4;
    sudoku[4][6].number = 0;
    sudoku[4][7].number = 9;
    sudoku[4][8].number = 0;    

    sudoku[5][0].number = 4;
    sudoku[5][1].number = 9;
    sudoku[5][2].number = 6;
    sudoku[5][3].number = 3;
    sudoku[5][4].number = 0;
    sudoku[5][5].number = 8;
    sudoku[5][6].number = 1;
    sudoku[5][7].number = 7;
    sudoku[5][8].number = 2;    

    sudoku[6][0].number = 0;
    sudoku[6][1].number = 0;
    sudoku[6][2].number = 0;
    sudoku[6][3].number = 0;
    sudoku[6][4].number = 8;
    sudoku[6][5].number = 9;
    sudoku[6][6].number = 2;
    sudoku[6][7].number = 6;
    sudoku[6][8].number = 0;    

    sudoku[7][0].number = 7;
    sudoku[7][1].number = 8;
    sudoku[7][2].number = 2;
    sudoku[7][3].number = 6;
    sudoku[7][4].number = 4;
    sudoku[7][5].number = 1;
    sudoku[7][6].number = 0;
    sudoku[7][7].number = 0;
    sudoku[7][8].number = 5;    

    sudoku[8][0].number = 0;
    sudoku[8][1].number = 1;
    sudoku[8][2].number = 0;
    sudoku[8][3].number = 0;
    sudoku[8][4].number = 0;
    sudoku[8][5].number = 0;
    sudoku[8][6].number = 7;
    sudoku[8][7].number = 0;
    sudoku[8][8].number = 8;    
    } 
    else if(temp == 'b'){
        sudoku[0][0].number = 2;
        sudoku[0][1].number = 0;
        sudoku[0][2].number = 0;
        sudoku[0][3].number = 0;
        sudoku[0][4].number = 0;
        sudoku[0][5].number = 0;
        sudoku[0][6].number = 0;
        sudoku[0][7].number = 0;
        sudoku[0][8].number = 3;

        sudoku[1][0].number = 0;
        sudoku[1][1].number = 0;
        sudoku[1][2].number = 0;
        sudoku[1][3].number = 0;
        sudoku[1][4].number = 8;
        sudoku[1][5].number = 1;
        sudoku[1][6].number = 9;
        sudoku[1][7].number = 4;
        sudoku[1][8].number = 0;

        sudoku[2][0].number = 0;
        sudoku[2][1].number = 1;
        sudoku[2][2].number = 0;
        sudoku[2][3].number = 4;
        sudoku[2][4].number = 3;
        sudoku[2][5].number = 0;
        sudoku[2][6].number = 0;
        sudoku[2][7].number = 0;
        sudoku[2][8].number = 0;

        sudoku[3][0].number = 0;
        sudoku[3][1].number = 7;
        sudoku[3][2].number = 1;
        sudoku[3][3].number = 5;
        sudoku[3][4].number = 4;
        sudoku[3][5].number = 0;
        sudoku[3][6].number = 6;
        sudoku[3][7].number = 2;
        sudoku[3][8].number = 9;

        sudoku[4][0].number = 0;
        sudoku[4][1].number = 6;
        sudoku[4][2].number = 3;
        sudoku[4][3].number = 8;
        sudoku[4][4].number = 7;
        sudoku[4][5].number = 2;
        sudoku[4][6].number = 4;
        sudoku[4][7].number = 0;
        sudoku[4][8].number = 1;

        sudoku[5][0].number = 0;
        sudoku[5][1].number = 2;
        sudoku[5][2].number = 5;
        sudoku[5][3].number = 0;
        sudoku[5][4].number = 0;
        sudoku[5][5].number = 9;
        sudoku[5][6].number = 0;
        sudoku[5][7].number = 0;
        sudoku[5][8].number = 0;

        sudoku[6][0].number = 7;
        sudoku[6][1].number = 9;
        sudoku[6][2].number = 0;
        sudoku[6][3].number = 0;
        sudoku[6][4].number = 0;
        sudoku[6][5].number = 0;
        sudoku[6][6].number = 5;
        sudoku[6][7].number = 0;
        sudoku[6][8].number = 4;

        sudoku[7][0].number = 0;
        sudoku[7][1].number = 4;
        sudoku[7][2].number = 0;
        sudoku[7][3].number = 0;
        sudoku[7][4].number = 0;
        sudoku[7][5].number = 8;
        sudoku[7][6].number = 0;
        sudoku[7][7].number = 0;
        sudoku[7][8].number = 0;

        sudoku[8][0].number = 1;
        sudoku[8][1].number = 0;
        sudoku[8][2].number = 0;
        sudoku[8][3].number = 7;
        sudoku[8][4].number = 0;
        sudoku[8][5].number = 0;
        sudoku[8][6].number = 2;
        sudoku[8][7].number = 3;
        sudoku[8][8].number = 0;
    }
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            if(sudoku[i][j].number != 0){
                sudoku[i][j].isFilled = true;
            } else{
                *remainingEmptyCells = 25;
            }
        }
    }
}

void fillSudoku(Tcell sudoku[9][9], int * remainingEmptyCells){

    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            int temp;

            printf("Introduce the element on row %d and column %d(Enter 0 to leave it blank): ", i + 1, j + 1);
            scanf("%d", &temp);     
            fflush(stdin);

            if(!(temp == 0)){
                sudoku[i][j].isFilled = true;
                * remainingEmptyCells-=1;
            }

            sudoku[i][j].number = temp;
            printSudoku(sudoku);
        }
    }
}

void printSudoku(Tcell sudoku[9][9]){
    for(int i = 0; i < 9; i++){
        if(i == 0){
            printf("_______________________________________________________\n");
        }
        for(int j = 0; j < 9; j++){
            if(j == 0 ){
                printf("|  ");
            }
            sudoku[i][j].number == 0 ? printf("-") : printf("%d", sudoku[i][j].number);
            (j == 8) ? printf("  |") : ((j == 2) || (j == 5) ? printf("  |  ") : printf("     "));
        }
        (i == 2) || (i == 5)? printf("\n_______________________________________________________\n") : ((i == 8 ) ? printf("\n") : printf("\n|                 |                 |                 |\n"));
        if(i == 8){
            printf("_______________________________________________________");
        }
    }
    printf("\n\n");
}

void checkRow(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol){

    for(int j = 0; j < 9; j++){
        if(!(j == currentCellCol)){
            if(sudoku[currentCellRow][j].isFilled){
                for(int i = 0; i < currentCell->posibleEntriesLength; i++){

                    if(currentCell->posibleEntries[i] == sudoku[currentCellRow][j].number){
                        currentCell->posibleEntries[i] = 0;

                        //update the length of the possible entries array and compact
                        for(int k = i; k < currentCell->posibleEntriesLength; k++){
                            
                            if(k == currentCell->posibleEntriesLength - 1 /*out of bounds*/){
                                currentCell->posibleEntries[k] = 0;
                            } else{
                                currentCell->posibleEntries[k] = currentCell->posibleEntries[k+1];
                            }
                        }

                        //as we are changing the size of the array in execution time, we need to account for that by doing the same to the loop
                        currentCell->posibleEntriesLength -=1;
                        i-=1;
                    }
                }
            }
        }
    }

}

void checkCol(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol){
    for(int i = 0; i < 9; i++){
        if(!(i == currentCellRow)){
            if(sudoku[i][currentCellCol].isFilled){
                for(int j = 0; j < currentCell->posibleEntriesLength; j++){

                    if(currentCell->posibleEntries[j] == sudoku[i][currentCellCol].number){
                        currentCell->posibleEntries[j] = 0; 

                        //update the length of the possible entries array and compact
                        for(int k = j; k < currentCell->posibleEntriesLength; k++){
                            if(k == currentCell->posibleEntriesLength - 1/*out of bounds*/){
                                currentCell->posibleEntries[k] = 0;
                            } else{
                                currentCell->posibleEntries[k] = currentCell->posibleEntries[k+1];
                            }
                        }

                        //as we are changing the size of the array in execution time, we need to account for that by doing the same to the loop
                        currentCell->posibleEntriesLength -=1;
                        j-=1;
                    }
                }
            }
        }
    }
}

void checkSubcell(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol){
    //to check each 3x3 submatrix, iterate through all the first cells of each one (e.g. cell (0,0), cell (0,3), cell (3,3)) and then for each
    //on of them iterate 9 times to tarjet all the cells of that submatrix
    
    bool isTargetInside = false;

    for(int i = 0; (i < 9) && !isTargetInside; i+=3){
        for(int j = 0; (j < 9) && !isTargetInside; j+=3){
            
            //for each submatrix, save all the numbers that appear in it, and check if the target number is inside it
            int subMatrix[9];
            int subMatrixLen = 0;

            for(int k = 0; k < 3; k++){
                for(int t = 0; t < 3; t++){

                    if(!((i+k) == currentCellRow) || !((j+t) == currentCellCol)){
                        subMatrix[subMatrixLen] = sudoku[i+k][j+t].number;
                        subMatrixLen+=1;
                    }else{
                        isTargetInside = true;
                    }
                }
            }
            //in the case that the cell is inside the current submatrix, then take the numbers inside the subcell (subMatrix[9]) and
            //check if there is one that coincides with the possible entries of our cell
            if(isTargetInside){
                for(int k = 0; k < subMatrixLen; k++){
                    for(int t = 0; t < currentCell->posibleEntriesLength; t++){
                        if(subMatrix[k] == currentCell->posibleEntries[t]){
                            currentCell->posibleEntries[t] = 0;

                            //update the length of the possible entries array and compact
                            for(int l = t; l < currentCell->posibleEntriesLength; l++){
                                if(l == currentCell->posibleEntriesLength - 1 /*out of bounds*/){
                                    currentCell->posibleEntries[l] = 0;
                                } else{
                                    currentCell->posibleEntries[l] = currentCell->posibleEntries[l+1];
                                }
                            }

                            //as we are changing the size of the array in execution time, we need to account for that by doing the same to the loop
                            currentCell->posibleEntriesLength -=1;
                            t-=1;
                        }
                    }
                }
            }
        }
    }
}

void checkFreeNumber(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol, int * remainingEmptyCells){
    //first use the fact thaat we are inside the submatrix that contains the target cell to check if there is a free number
    //(recall the first comment of the file)

    bool isTargetInside = false;

    for(int i = 0; (i < 9) && !isTargetInside; i+=3){
        for(int j = 0; (j < 9) && !isTargetInside; j+=3){
            for(int k = 0; k < 3; k++){
                for(int t = 0; t < 3; t++){
                    if(((i+k) == currentCellRow) && ((j+t) == currentCellCol)){
                        isTargetInside = true;
                    }
                }
            }

            if(isTargetInside){
                //once the indexes i and j gives us the first element inside the 3x3 submatrix of the target cell
                //(previous loops) we need to get all the unique posible entries of all the cells inside the submatrix
                //and compare them with the posible entries of our target cell. If there is one posible entry of our
                //target cell that is not inside the other array, then the target cell has a free number

                int uniquePosibleEntries[9] = {0};
                int uniquePosibleEntriesLen = 0;

                for(int k = 0; (k < 3) && uniquePosibleEntriesLen == 0; k++){
                    for(int t = 0; ((t < 3) && (uniquePosibleEntriesLen == 0)); t++){
                        if((!((i + k) == currentCellRow) || !((j + t) == currentCellCol)) && !sudoku[i+k][j+t].isFilled && (sudoku[i+k][j+t].posibleEntriesLength != 0)){
                            uniquePosibleEntries[0] = sudoku[i+k][j+t].posibleEntries[0];
                            uniquePosibleEntriesLen += 1;
                        }

                    }
                }

                bool isInside = false;

                for(int k = 0; k < 3; k++){
                    for(int t = 0; t < 3; t++){
                        if(!((i + k) == currentCellRow && (j + t) == currentCellCol) && !sudoku[i+k][j+t].isFilled){

                            for(int l = 0; l < sudoku[i+k][j+t].posibleEntriesLength; l++){

                                int num = sudoku[i+k][j+t].posibleEntries[l];
                                isInside = false;

                                for(int p = 0; p < uniquePosibleEntriesLen; p++){
                                    if(uniquePosibleEntries[p] == num){
                                        isInside = true;
                                        break;
                                    }
                                }

                                if(!isInside){
                                    uniquePosibleEntries[uniquePosibleEntriesLen++] = num;
                                }
                            }
                        }
                    }
                }
                


                //asssume a given posible entry of our target cell 
                //is not inside the unique posible entries of the subarray

                for(int k = 0; k < currentCell->posibleEntriesLength; k++){
                    isInside = false;
                    int candidateNumber = currentCell->posibleEntries[k];
                
                    for(int t = 0; t < uniquePosibleEntriesLen; t++){
                        if(uniquePosibleEntries[t] == candidateNumber){
                            isInside = true;
                            break;
                        }
                    }
                
                    if(!isInside){
                        //if there is a unique entry place it
                        printf("We found that cell (%d,%d) is %d!\n\n", currentCellRow, currentCellCol, candidateNumber);
                
                        currentCell->number = candidateNumber;
                        currentCell->isFilled = true;
                        currentCell->posibleEntriesLength = 0;
                        
                        // clear the array for consistencies 
                        for(int l = 0; l < 9; l++){
                            currentCell->posibleEntries[l] = 0;
                        }
                
                        *remainingEmptyCells = *remainingEmptyCells - 1;
                
                        return;
                    }
                }                
            }
        }
    }
}

// used in checkGhostNumber()
bool isAligned(int PositionArray[3][2], int PositionArrayLen, char *dir) {
    if (PositionArrayLen == 2) {
        if (PositionArray[0][0] == PositionArray[1][0]) {
            *dir = 'r'; // same row means aligned
            return true;
        } else if (PositionArray[0][1] == PositionArray[1][1]) {
            *dir = 'c'; // same column means aligned
            return true;
        }
        return false; // not aligned
    } 
    else if (PositionArrayLen == 3) {
        if (PositionArray[0][0] == PositionArray[1][0] && PositionArray[1][0] == PositionArray[2][0]) {
            *dir = 'r'; // same row
            return true;
        } else if (PositionArray[0][1] == PositionArray[1][1] && PositionArray[1][1] == PositionArray[2][1]) {
            *dir = 'c'; // same column
            return true;
        }
        return false; // Not aligned
    }
    return false; 
}

void checkGhostNumber(Tcell sudoku[9][9], Tcell * currentCell, int currentCellRow, int currentCellCol){
    //the function will check for a ghost number in the current cell (recall ghost number from the first comment). It wont change anything from the 
    //current cell, but the consequences on the row or column will be updated. One thing to note is that the funciton will try to search 
    //for a ghost number with ALL the posible entries of the current Cell, and if it finds one, it will change the posible entries in that cell or row

    //start by getting inside the 3x3 submatrix

    bool isTargetInside = false;

    for(int i = 0; (i < 9) && !isTargetInside; i+=3){
        for(int j = 0; (j < 9) && !isTargetInside; j+=3){
            for(int k = 0; k < 3; k++){
                for(int t = 0; t < 3; t++){
                    //the for loops make it so that we are not checking the sudoku 1st row, change column, 2 row... Inside the englightening-drawings.excalidraw file, number 1 drawing lets you see visualy
                    
                    if(((i+k) == currentCellRow) && ((j+t) == currentCellCol)){
                        isTargetInside = true;
                    }
                }
            }
        
            if(isTargetInside){
                //when we get to this part, the indexes i, j will give us the first element of the submatrix with the cell inside
                
                int appearanceArr[3][2] = {0};
                int appearanceArrLen = 0;
                bool overflow = false;

                for(int k = 0; k < currentCell->posibleEntriesLength; k++){

                    //reset all the values when searching for a new ghost number in our cell
                    for(int t = 0; t < 3; t++){
                        for(int l = 0; l < 2; l++){
                            appearanceArr[t][l] = 0;
                        }
                    }
                    appearanceArrLen = 0;

                    //again, we search not by row and col, but by submatrices
                    for(int t = 0; !overflow && t < 3; t++){
                        for(int l = 0; !overflow && l < 3; l++){
                            //notice that we are also counting the case where t and l == currentCellRow and currentCellCol, as we want to save the number of total appearances of the submatrix
                            for(int p = 0; !overflow && p < (sudoku[i + t][j + l].posibleEntriesLength) && !sudoku[i + t][j + l].isFilled ; p++){
                                if(currentCell->posibleEntries[k] == sudoku[i + t][j + l].posibleEntries[p]){
                                    //save the position where there is a posible entry of our number. In the case there is more than 3 posible entries, the ghost number cannot be in one single column or 
                                    //row, so it is not posible to have one
                                    
                                    if (appearanceArrLen < 3) { 
                                        appearanceArr[appearanceArrLen][0] = i + t;
                                        appearanceArr[appearanceArrLen][1] = j + l;
                                        appearanceArrLen++;
                                    } else{
                                        overflow = true;
                                    }
                                    
                                }
                            }
                        }
                    }

                    //once we have got all the posible entries of the submatrix, its time to check if we really have a ghost number.
                    //if it happens to be that the submatrix has exactly one posible entry, we won't do nothing, because in the main 
                    //function, we will check (when all de "subalgorithms" end) for cells with 1 posible entry (means that the cell we are on is a trivial cell)

                    if(((appearanceArrLen == 2) || (appearanceArrLen == 3)) && !overflow){
                        // the function will save in 'direction' if the numbers are in the same row or column as 'r', 'c'
                        char direction = 'e';

                        bool aligned = isAligned(appearanceArr, appearanceArrLen, &direction);

                        //once we know if it is aligned and in which direction, just change the row or column it pertains to

                        if(aligned && (direction == 'r')){
                            //the cells of the 3x3 submatrix were the ghost number lives (t, t + 1, t + 2), are not counted

                            for(int t = 0; (t < 9) && (t != i) && (t != i + 1) && (t != i + 2) /*i references the first elem of the 3x3 subm*/; t++){
                                // To know the row where the ghost number is, I am using the first cell in the appearance aray (you could use the second also)
                                
                                for(int l = 0; l < (sudoku[appearanceArr[0][0]][t].posibleEntriesLength) && sudoku[appearanceArr[0][0]][t].isFilled; l++){
                                    if(sudoku[appearanceArr[0][0]][t].posibleEntries[l] == currentCell->posibleEntries[k]){
                                        sudoku[appearanceArr[0][0]][t].posibleEntries[l] = 0;

                                        //compact the array of posible entries
                                        for(int p = l; p < sudoku[appearanceArr[0][0]][t].posibleEntriesLength; p++){
                                            if(p == sudoku[appearanceArr[0][0]][t].posibleEntriesLength - 1/*out of bounds*/){
                                                sudoku[appearanceArr[0][0]][t].posibleEntries[p] = 0;
                                            } else{
                                                sudoku[appearanceArr[0][0]][t].posibleEntries[p] = sudoku[appearanceArr[0][0]][t].posibleEntries[p+1];
                                            }
                                        }
                
                                        //as we are changing the size of the array in execution time, we need to account for that by doing the same to the loop
                                        sudoku[appearanceArr[0][0]][t].posibleEntriesLength -= 1;
                                        l -= 1;
                                    }
                                }
                            }
                        } else if(aligned && (direction == 'c')){
                            for(int t = 0; (t < 9) && (t != i) && (t != i + 1) && (t != i + 2); t++){
                                for(int l = 0; l < (sudoku[t][appearanceArr[0][1]].posibleEntriesLength) && sudoku[t][appearanceArr[0][1]].isFilled; l++){
                                    if(sudoku[t][appearanceArr[0][1]].posibleEntries[l] == currentCell->posibleEntries[k]){
                                        sudoku[t][appearanceArr[0][1]].posibleEntries[l] = 0;

                                        //compact the array

                                        for(int p = l; p < sudoku[t][appearanceArr[0][1]].posibleEntriesLength; p++){
                                            if(p == sudoku[t][appearanceArr[0][1]].posibleEntriesLength - 1/*out of bounds*/){
                                                sudoku[t][appearanceArr[0][1]].posibleEntries[p] = 0;
                                            } else{
                                                sudoku[t][appearanceArr[0][1]].posibleEntries[p] = sudoku[t][appearanceArr[0][1]].posibleEntries[p+1];
                                            }
                                        }
                
                                        //as we are changing the size of the array in execution time, we need to account for that by doing the same to the loop
                                        sudoku[t][appearanceArr[0][1]].posibleEntriesLength -= 1;
                                        l -= 1;
                                    }
                                }
                            }
                        }
                    } 
                }
            }
        }
    }
}
//TODO