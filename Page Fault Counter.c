#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int arr[9999];         //array of pages sequence
static int pf=0;              //page fault counter
static int frames;            //number of frames
static int pages=0;           //number of pages
static int hitflag=0;         //if hit or not
static int added=0;           //how many pages added in frames
static int option;            //status of page
static int memory[9999];      //memory
static int fullframes=0;      //how many frames used
static int foundindex;        //index which will be used to select the page replacement
static int addflag=0;         //used to allow if page will be added to LRU array or not
static int lrupages=0;        //used in LRU array to count number of pages of inserted pages
static int lruarr[9999];      //LRU array to decide which has most priority
static int bits[9999];        //the array that decides what the pointer will do
static int pointer=0;         //moving pointer in clock and fifo
static int missflag=0;        //used in find optimal to see if page doesn't exist in the first place
static int maxcount=0;        //to save the max number of loops by page in optimal algo
static int init=0;            //used to fill in initial pages in frames


void printarr(int *print)
{

    int i;
    if(fullframes>frames)            //function to print contents of frames
        fullframes=frames;
    for(i=0; i<fullframes; i++)
    {
        printf("%02d ",print[i]);
    }
    printf("\n");
}

void checknumber(int *check,int val) //function to check the state of page whether the page already exists or not or still there are empty frames
{
    static int i;

    hitflag=0;



    for(i=0; i<frames; i++)
    {
        if(check[i] == val)
        {
            hitflag=1;                //if hit modifies option to 3 and hit flag
            option=3;
            break;
        }
    }

    if(val==0 && added<frames)       //small case when page=0 it always hits when frames are still empty
        hitflag=0;

    if(hitflag==0)                    //page not found!
    {


        if (added<frames)             //there are still empty frames as added pages<frames and modifies option=1
        {
            option=1;
        }
        else if(added>=frames)       //if all frames full search begins to decide if hit or not
        {
            option=2;                //if search failed hit flag is initialized to zero so it will modify option=2 and increase page fault counter
            pf++;
        }

    }

    hitflag=0;                                 //resets hit flag
}
void fifo()
{


    printf("Replacement Policy = FIFO\n");
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");

    int i;
    for(i=0; i<pages; i++)                     //going on every page one by one
    {

        checknumber(memory,arr[i]);            //calling function to detect the status of page and select the options


        if(option==1)                          //frames still not full!
        {

            printf("%02d     ",arr[i]);        //printing the page
            memory[init]=arr[i];               //just add! the page of array of pages in the memory
            init++;                            //filling initial pages
            added++;                           //to help in check number function
            fullframes++;                      //just to help print function when printing the memory content
            printarr(memory);                  //printing content of frames beside the page

        }
        if(option==2)                          //oops page doesn't exist...
        {

            printf("%02d ",arr[i]);           //printing the page
            memory[pointer]=arr[i];           //position of pointer that points to the first inserted element and keeps track of the elements after it
            printf("F   ");                   //Fault
            printarr(memory);                 //printing content of frames beside the page
            pointer++;                        //incrementing pointer to the next inserted elements as we move by FIFO algorithm

        }
        if(option==3)                        //page found hit!
        {
            printf("%02d     ",arr[i]);      //print the page
            printarr(memory);                //and the content of frames
        }

        if(pointer==frames)                 //resetting pointer position!
            pointer=0;


    }
    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n",pf);
}

void findOptimal(int stopindex)              //sending where we stopped in pages and the limit of array of pages
{


    static int i,j,loopcount=0;

    for(i=0; i<frames; i++)                  //nested loop to search the page in array of pages if the page doesn't exist in the first place
    {

        missflag=1;

        for(j=stopindex; j<pages; j++)
        {

            if(memory[i]==arr[j])            //compares pages in frames and pages in array of pages
            {
                missflag=0;

            }

        }
        if(missflag==1)
        {
            foundindex=i;                        //if page not found end of search as it means it will not be used in future as we start searching from where we stopped as appeared in attributes of function and return the page that will be replaced in frames
            goto endnested;
        }

    }



    for(i=0; i<frames; i++)                     //if miss search fails it means that pages in frames exist in future!
    {

        for(j=stopindex; j<pages; j++)
        {

            loopcount++;

            if(memory[i]==arr[j])
            {
                if(loopcount>maxcount)         //nested loop we see how many loops will the page cost to find it in future
                {
                    foundindex=i;              //we compare loop counts and save the bigger one so at the end we have the page with biggest loop count
                    maxcount=loopcount;
                }
                j=stopindex;                    //resets pages position
                loopcount=0;                    //reset the max loops counter
                break;
            }
        }


    }

    endnested:;
    maxcount=0;                                 //reset the max loops counter


}

void optimal()
{

    printf("Replacement Policy = OPTIMAL\n");
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");

    static int i,stopindex;

    for(i=0; i<pages; i++)                    //going on every page one by one
    {
        checknumber(memory,arr[i]);           //calling function to detect the status of page and select the options

        if(option==1)                         //frames still not full!
        {
            printf("%02d     ",arr[i]);       //printing the page
            memory[init]=arr[i];              //just add! the page of array of pages in the memory
            init++;                           //filling initial pages
            added++;                          //to help in check number function
            fullframes++;                     //just to help print function when printing the memory content
            printarr(memory);                 //printing content of frames beside the page

        }
        if(option==2)                          //oops page doesn't exist...
        {
            stopindex=i+1;                     //saving the position where we stopped in the pages in array of pages and incrementing by one to not begin with the waiting page
            findOptimal(stopindex);            //calling the function that will find the page that will be replaced
            printf("%02d F   ",arr[i]);        //printing the page
            memory[foundindex]=arr[i];         //replacing the found page
            added++;                           //to help in check number function
            fullframes++;                      //just to help print function when printing the memory content
            printarr(memory);                  //printing content of frames beside the page

        }
        if(option==3)                           //page found hit!
        {
            printf("%02d     ",arr[i]);         //print the page
            printarr(memory);                   //and the content of frames

        }


    }
    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n",pf);

}

void lruadd(int val)   //sending the page number
{

    int i=0,j,m;

    addflag=1;

    do                                    //nested loop to search if the page already exists or not in LRU page array
    {

        if(val==lruarr[i])                //if exists add it to the end where most recent page exist else leave addflag=1
        {

            addflag=0;

            j=i;
            m=i+1;

            do
            {
                lruarr[j]=lruarr[m];      //shift the array of LRU pages by 1 from the position of the new added page to update its postion
                j++;
                m++;
            }
            while(j<lrupages);

            lruarr[lrupages-1]=val;        //add the page at the end after shifting

            goto endsearch;                //ending search
        }

        i++;
    }
    while(i<lrupages);


endsearch:
    ;

    if(addflag==1)
    {
        lruarr[lrupages]=val;           //search failed addflag still 1 so add the page as most recent page in array of lru pages
        lrupages++;
    }

}

void findlru() //function to find the Least Used Page in LRU array
{
    int i,j;
    for(i=0; i<=lrupages; i++)
    {
        for(j=0; j<frames; j++)
        {
            if(memory[j]==lruarr[i])            //nested loop to compare between content of frames and array of LRU pages as we put pages least used in front of array
            {
                foundindex=j;                   //found the LRU page by order and sending index
                goto label;                     //end search
            }
        }
    }
label:
    ;
}

void lru()
{

    printf("Replacement Policy = LRU\n");
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");

    int i;

    for(i=0; i<pages; i++)                //going on every page one by one
    {

        checknumber(memory,arr[i]);       //calling function to detect the status of page and select the options


        if(option==1)                     //frames still not full!
        {
            lruadd(arr[i]);              //function to add the element to the least used pages array by order after using it
            printf("%02d     ",arr[i]);  //printing the page
            memory[init]=arr[i];            //just add! the page of array of pages in the memory
            added++;
            init++;                     //to help in check number function
            fullframes++;                //just to help print function when printing the memory content
            printarr(memory);            //printing content of frames beside the page

        }
        if(option==2)                    //oops page doesn't exist...
        {
            lruadd(arr[i]);              //function to add the page to the least used pages array by order after using it
            findlru();                   //function to find the least used page in least used pages array
            memory[foundindex]=arr[i];   //replacing the found page
            printf("%02d F   ",arr[i]);  //printing the page
            added++;                     //to help in check number function
            fullframes++;                //just to help print function when printing the memory content
            printarr(memory);            //printing content of frames beside the page

        }
        if(option==3)                     //page found hit!
        {
            lruadd(arr[i]);               //function to add the element to the least used pages array by order after using it
            printf("%02d     ",arr[i]);   //printing the page
            printarr(memory);             //printing content of frames beside the page

        }

    }


    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n",pf);

}

void addbits(int val)                  //function to change the bit of the page and sets to 1
{
    int i;
    for(i=0; i<frames; i++)
    {
        if(memory[i] == val)
        {
            bits[i]=1;
            break;
        }
    }
}

void clockfind()                        //function to see status of pointer and what it will do as pointer is a global variable
{

    while(1)                            //pointer in a loop checks every bit one by one in bit array
     {


         if(bits[pointer]==0)           //pointer checks if bit=0 then it has no chances therefore finds the page that will be replaced and sends the index then breaks also pointer increments so it goes to the next bit
         {
             foundindex=pointer;
             pointer++;
             goto endwhile;
         }
           if(bits[pointer]==1)           //pointer found the bit =1 so it resets to 0 and continues it's cycle and keeps searching after this page second chance lost!
         {
             bits[pointer]=0;
             pointer++;
         }
         if(pointer==frames)            //if pointer exceeds the frames number it resets to start searching from the begining
             pointer=0;
     }

    endwhile:;

    if(pointer==frames)            //if pointer exceeds the frames number it resets to start searching from the begining
            pointer=0;

}


void clock()
{


    printf("Replacement Policy = CLOCK\n");
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");

    int i;
    for(i=0; i<frames; i++)                //initializing bits array to ones with size equal to memory frames to connect them with each other
    {
        bits[i]=1;
    }

    for(i=0; i<pages; i++)                //going on every page one by one
    {

        checknumber(memory,arr[i]);       //calling function to detect the status of page and select the options


        if(option==1)                     //frames still not full!
        {
            printf("%02d     ",arr[i]);   //printing the page
            memory[init]=arr[i];
            init++;                       //just add! the page of array of pages in the memory
            added++;                      //to help in check number function
            fullframes++;                 //just to help print function when printing the memory content
            printarr(memory);             //printing content of frames beside the page
        }
        if(option==2)                     //oops page doesn't exist...
        {

            clockfind();                  //function to find where is the pointer to find the index
            memory[foundindex]=arr[i];    //replacing the found page
            bits[foundindex]=1;
            printf("%02d F   ",arr[i]);   //printing the page
            added++;                      //to help in check number function
            fullframes++;                 //just to help print function when printing the memory content
            printarr(memory);             //printing content of frames beside the page
        }
        if(option==3)
        {

            addbits(arr[i]);             //function to change bits of the current pointer to 1
            printf("%02d     ",arr[i]);  //printing the page
            printarr(memory);            //printing content of frames beside the page
        }

    }

    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n",pf);

}

int main()
{
    char str[20];
    int i=0;
    scanf("%d",&frames);
    scanf("%s",str);
    while(1)
    {
        scanf("%d",&arr[i]);
        if(arr[i]==-1)
            break;
        pages++;
        i++;
    }


    if (strcasecmp(str,"FIFO")==0)
        fifo();
    if (strcasecmp(str,"Optimal")==0)
        optimal();
    if (strcasecmp(str,"LRU")==0)
        lru();
    if (strcasecmp(str,"Clock")==0)
        clock();

    return 0;
}
