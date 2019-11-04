/* paging.c */

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void wr_mem(int location, int value);
int rd_mem(int Location);
int PageFileFind(int location);
void PageFileWrite(int location, int value);
int LRU();


int pageFile; //disk
int memory[5]; //RAM
int pagetable[8]; //virtual memory

int whichMem[8]; // which memory location
int time[5];

int main()
{
  int loopcount;
  /* set up the page file */
  pageFile = open("pagefile.bin",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR | S_IWUSR);
  /* 8 bytes of 0 */
  for (loopcount = 0 ; loopcount < 8 ; loopcount++)
  {
    write(pageFile,"\0",1); /* cheap way to have an array with a 0 */
  }

  /* initialize all pages to contain a value */
  for (loopcount = 0 ; loopcount < 8 ; loopcount++)
  {
    /* mem[loopcount] = loopcount; */
    wr_mem(loopcount, loopcount);
  };

  /* cause some more paging */
  wr_mem(4, rd_mem(2) + rd_mem(4) ); /* mem[4] = mem[2] + mem[4] */
  wr_mem(6, rd_mem(2) + rd_mem(7) ); /* mem[6] = mem[2] + mem[7] */
  wr_mem(1, rd_mem(1) + rd_mem(3) ); /* mem[1] = mem[1] + mem[3] */
  wr_mem(5, rd_mem(3) + rd_mem(7) ); /* mem[5] = mem[3] + mem[7] */

  /* we are done the test */
  close(pageFile);
  return 0;
};

int PageFileFind(int location)
{ 
  char ch;
  lseek(pageFile,location,SEEK_SET);
  read(pageFile,&ch,1);
  return ch;
};

void PageFileWrite(int location, int value){ 
    lseek(pageFile,location,SEEK_SET);
    write(pageFile,&value,sizeof(int));
};

void wr_mem(int location, int value){
  /* Save LRU RAM value to disk */  
  int location_table;
  int bit = LRU(); //find LRU
  int temp_value = rd_mem(bit); //save the value of LRU from memory
  for(int i=0; i<8; i++){
      if(pagetable[i] == bit){
        location_table = i;
      }
  }
  PageFileWrite(location_table,temp_value); 
  whichMem[location_table] = 0; 
  
  /* Write new value to RAM */
  pagetable[location] = bit;
  memory[bit] = value;
  whichMem[location] = 1;

};

int rd_mem(int location){
  if(whichMem[location] == 0) { // Disk
    return PageFileFind(location);
  }
  else { //RAM/Memory
    return memory[pagetable[location]]; // pagetable[location]->memory[]->value
  }
};

int LRU(){
    int LRU_bit;

    int max = time[0];
    for (int i=0; i<5; i++){
        if(max <= time[i]){
            max = time[i];
            LRU_bit = i;
        }
    }
    /* Increment time */
    for (int i=0; i<5; i++){
        time[i]++;
    }
    time[LRU_bit] = 0; /* Now most recently used */ 
    return LRU_bit;
};