#include <stdio.h>
#include <stdlib.h>
#include <time.h>//for timer and date saving
#include <string.h>//strcpy
#include <conio.h>//reading keyboard input (comment out if not supported and remove code for reading keyboard input)

//if running on online gdb, 1.change char *date[] to char date[],2. remove _kbhit() and _getch() calls

struct cell{
    int mine;
    int revealed;
    int value;// could have assigned a mine a value of 9 instead
}cell;//board cell declaration

typedef struct {
    char date[15];
    char difficulty[15];
    int threebv;
    int time;
} Entry;//data entry struct for saving times, also easier to sort as struct

Entry create_entry(char *date[], char difficulty[], int time, int threebv) {
    Entry entry;
    strcpy(entry.date, date);
    strcpy(entry.difficulty, difficulty);
    entry.time = time;
    entry.threebv = threebv;
    return entry;//creating an entry from inputs
}

void save_entry(Entry entry) {//writing an entry object to file
    FILE *fp;
    fp = fopen("entries.txt", "a");
    fprintf(fp, "%s %d %s %d\n", entry.date, entry.time, entry.difficulty, entry.threebv);
    fclose(fp);
}

int records(){//printing of times
        system("cls");
        Entry entries[500];
        int num_entries = 0,quit;
        FILE *fp;
        fp = fopen("entries.txt", "r");
        while (fscanf(fp, "%s %d %s %d", &entries[num_entries].date, &entries[num_entries].time, &entries[num_entries].difficulty, &entries[num_entries].threebv) != EOF)
            num_entries++;
        fclose(fp);
        for (int i = 0; i < num_entries - 1; i++) {//bubble sort ¯\_(ツ)_/¯
            for (int j = 0; j < num_entries - i - 1; j++) {
                if (entries[j].time> entries[j + 1].time) {
                    Entry temp = entries[j];
                    entries[j] = entries[j + 1];
                    entries[j + 1] = temp;
                }
            }
        }
        //printing times for each difficulty
        printf("Beginner:\n");
        for (int i = 0; i < num_entries; i++) {
            if (strcmp(entries[i].difficulty, "Beginner") == 0) {
                printf("Date:%s|Time:%d|3BV:%d\n", entries[i].date, entries[i].time,  entries[i].threebv);
            }
        }
        printf("\nIntermediate:\n");
        for (int i = 0; i < num_entries; i++) {
            if (strcmp(entries[i].difficulty, "Intermediate") == 0) {
                printf("Date:%s|Time:%d|3BV:%d\n", entries[i].date, entries[i].time, entries[i].threebv);
            }
        }
        printf("\nExpert:\n");
        for (int i = 0; i < num_entries; i++) {
            if (strcmp(entries[i].difficulty, "Expert") == 0) {
                printf("Date:%s|Time:%d|3BV:%d\n", entries[i].date, entries[i].time, entries[i].threebv);
            }
        }
        printf("Press E to exit\n");
        while(1)
        {
            if (_kbhit()) {  //exit times menu condition
                {
                    char c = _getch();
                    if(c=='e' || c=="E" )
                        return 1;
                }


            }
        }
}

int random(int min,int max){//random num in range
    return rand() % (min + 1 - max) + min;
}

int unrevealed;//global value

int show(int vsize, int hsize, struct cell *board, int x, int y,int bv) {
    //Spent 2 hours before i realized i had to do (hsize+2) instead of hsize, the reason seems obvious now, but i didnt really use pointers before
    if (x <1 || x > vsize || y <1|| y > hsize) {
        return -1; // out of bounds check
    }

    //check if the clicked cell has already been revealed
    if (board[x*(hsize+2) + y].revealed) {
        return 0; // Already revealed
    }

    //Reveal the clicked cell


    board[x*(hsize+2) + y].revealed = 1;

    //decrement counter and check for function context
    if(!bv)
        unrevealed--;

    //if the clicked cell is a mine, return -2 to indicate game over
    if (board[x*(hsize+2) + y].mine) {
        return -2; // Game over
    }

    //if the clicked cell has a value of 0, reveal its neighbors
    if (board[x*(hsize+2) + y].value == 0) {
        //reveal all of the 8 neighbors of the clicked cell
        show(vsize, hsize, board, x-1, y-1,bv);
        show(vsize, hsize, board, x-1, y,bv);
        show(vsize, hsize, board, x-1, y+1,bv);
        show(vsize, hsize, board, x, y+1,bv);
        show(vsize, hsize, board, x+1, y+1,bv);
        show(vsize, hsize, board, x+1, y,bv);
        show(vsize, hsize, board, x+1, y-1,bv);
        show(vsize, hsize, board, x, y-1,bv);
    }

    return 1;
}

int calculate3BV(struct cell *board, int rows, int cols) {
    int count = 0,i,j;
    struct cell newboard[rows+2][cols+2];
    memcpy(&newboard,board,(rows+2)*(cols+2)*sizeof(cell));//copying the game board into a new array
    for(i=1;i<=rows;i++)
        for(j=1;j<=cols;j++) {
        if(newboard[i][j].value==0 && !newboard[i][j].revealed)
        {
            show(rows,cols,newboard,i,j,1);//recursively revealing
            newboard[i][j].revealed=1;//marking cell
        }
        }
    for(i=1;i<=rows;i++)
        for(j=1;j<=cols;j++)
            if(!newboard[i][j].revealed && !newboard[i][j].mine)
                count++;//adding leftover cells to the count
    return count;
}//this function calculates the minimum number of "clicks" required to reveal all non-mine cells by revealing all the cells that can be recursively revealed with 1 click, it considers that whole area as one whole revealed block, then it counts the number of cells left that arent mines and adds them to the 3BV value (this calculation is not accurate in rare cases).

void start(int win){
    system("cls");
    if(win==1)
        printf("You won!\n");
    else if(win==2)
        printf("You lost!\n");

    time_t t,start_time,current_time;
    srand((unsigned) time(&t));
    int difficulty,vsize,hsize,mines=0,i,j,row,r,c,mc,first=1,bv3=0,args1;//vsize=rows,hsize=column
    char column;//using a char for later

    //difficulty selection, using the mines variable to check if a valid difficulty has been selected to avoid using another variable
    while(!mines){
        printf("%s","Enter difficulty:\n1.Beginner\n2.Intermediate\n3.Expert\nOr\n4.Show times\n");
        int args = scanf("%d", &difficulty);//copied this piece of code off google to stop malformed input from breaking the loop
        if (args != 1)
        {
            char dummy[20];
            scanf("%s", &dummy);
        }
        switch(difficulty){//setting mine and game board values
            case 1:
                vsize=hsize=9;
                mines=10;
                break;
            case 2:
                vsize=hsize=16;
                mines=40;
                break;
            case 3:
                vsize=16;
                hsize=30;
                mines=99;
                break;
            case 4:
                records();
                system("cls");
                break;
            default:
                printf("%s","Please select a valid difficulty\n");
        }

    }
    unrevealed=vsize*hsize;//game board size
    system("cls");

    struct cell board[vsize+2][hsize+2]; //declaration of game board
    for(i=0;i<=vsize+1;i++) // initialization of game board (not sure if this is the best way in c)
        for(j=0;j<=hsize+1;j++) // also bordering the matrix with 0s so i dont have to implement special cases later for edges and corners
        {
            board[i][j].revealed=0;
            board[i][j].mine=0;
            board[i][j].value=0;
        }

    while(1) {
        system("cls");
        printf("Timer:%d  ", current_time -
                             start_time); // could have used a non blocking read func instead of scanf to have a live timer
        if (bv3)
            printf("3BV:%d\n", bv3);//formatting
        else
            printf("%s\n", "3BV:Uncalculated");
        // Print column indices
        if (difficulty > 1)
            printf("C  ");
        else
            printf("C ");
        for (j = 1; j <= hsize; j++) {
            if (difficulty > 1 && j > 9)
                printf("%c ", 'A' - (10 - j));//printing indices for intermediate and expert
            else
                printf("%d ", j);
        }
        printf("\n");

        for (i = 1; i <= vsize; i++) {
            if (difficulty > 1 && i < 10)
                printf("%d  ", i);//formatting
            else
                printf("%d ", i);

            for (j = 1; j <= hsize; j++) {
                if (board[i][j].revealed && !board[i][j].mine)
                    printf("%d ", board[i][j].value);
                else
                    printf("%s", "- ");//formatting

            }
            printf("%s", "\n");
        }
        printf("%s", "Input row and column(ex:2 3):");

        row = column = 0;
while(1){
        args1 = scanf("%d %c", &row,&column);//copied this piece of code off google to stop malformed input from breaking the loop
        if (args1 != 2 || (row<1 || column<1)) {
            char dummy[20];
            scanf("%s", &dummy);
            printf("Please enter a valid input\n");
        }
            else
                break;
        }

        if(first){

            mc=0;
            while(mc<mines)//generating game board on first loop
            {
                r=random(1,vsize+2);
                c=random(1,hsize+2);
                if(!board[r][c].mine && r!=row && c!=column)
                {
                    board[r][c].mine=1;
                    mc++;
                }
            }
            first=0;
            for(i=1;i<=vsize;i++)
            {
                for(j=1;j<=hsize;j++) {
                    if(!board[i][j].mine)//precalculating cell values
                        board[i][j].value= board[i][j-1].mine+board[i-1][j-1].mine+board[i-1][j].mine+board[i-1][j+1].mine+board[i][j+1].mine+board[i+1][j+1].mine+board[i+1][j].mine+board[i+1][j-1].mine;
                }
            }
            bv3= calculate3BV(board,vsize,hsize);//3bv call
            start_time = time(NULL);
        }

        if(difficulty>1 && column>='A' && column<='Z')//converting indices from letter to integer for intermediate and expert
            column=column-'A'+10;
        else
            column = column - '0';
        if(show(vsize,hsize,board,row,column,0)==-2)
        {
            printf("%s","Game over\n");
            start(2);
            break;
        }
        current_time = time(NULL);
        if(unrevealed==mines)//win condition (might be broken, will fix)
        {
            printf("%s","You won!\n");
            struct tm *tm = localtime(&t);//getting current time
            char date[11];
            strftime(date, sizeof(date), "%Y-%m-%d", tm);//time formatting
            char difficulties[4][15]={"Beginner","Intermediate","Expert"};
            time_t time1=current_time-start_time;
            save_entry(create_entry(date,difficulties[difficulty-1],time1,bv3));//saving of time to file
            start(1);
            break;
        }

    }

}

int main() {
    start(0);
}
