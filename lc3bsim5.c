/*
    Name 1: Simoni Maniar
    UTEID 1: ssm3256
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_EXCV,
    LD_EXC,
    LD_VECTOR,
    LD_PSR15,
    LD_PSR,
    LD_SSP,
    LD_USP,
    LD_VA,
    LD_RET,
    LD_RW,
    LD_SAVEMDR,
    LD_PC,
    GATE_VECSHF,
    GATE_VECTOR,
    GATE_PTBR,
    GATE_VA,
    GATE_PSR,
    GATE_R6PLUS2,
    GATE_R6MINUS2,
    GATE_SSP,
    GATE_PCMINUS2,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    REGMUX,
    PSRMUX,
    VECMUX1, VECMUX0,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    EXCMUX,
    LSHF1,
    RETMUX3, RETMUX2, RETMUX1, RETMUX0,
    VAMUX,
    RWMUX,
    VARETMUX,
    MDRMUX1, MDRMUX0,
    CONTROL_STORE_BITS
    // add more
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2)+(x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
                                      (x[J3] << 3) + (x[J2] << 2) +
                                      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetLD_EXCV(int *x)       { return(x[LD_EXCV]);}
int GetLD_EXC(int *x)        { return(x[LD_EXC]);}
int GetLD_VECTOR(int *x)     { return(x[LD_VECTOR]);}
int GetLD_PSR15(int *x)      { return(x[LD_PSR15]);}
int GetLD_PSR(int *x)        { return(x[LD_PSR]);}
int GetLD_SSP(int *x)        { return(x[LD_SSP]);}
int GetLD_USP(int *x)        { return(x[LD_USP]);}
int GetGATE_VECSHF(int *x)   { return(x[GATE_VECSHF]);}
int GetGATE_VECTOR(int *x)   { return(x[GATE_VECTOR]);}
int GetGATE_PSR(int *x)      { return(x[GATE_PSR]);}
int GetGATE_R6PLUS2(int *x)  { return(x[GATE_R6PLUS2]);}
int GetGATE_R6MINUS2(int *x) { return(x[GATE_R6MINUS2]);}
int GetGATE_SSP(int *x)      { return(x[GATE_SSP]);}
int GetGATE_PCMINUS2(int *x) { return(x[GATE_PCMINUS2]);}
int GetREGMUX(int *x)        { return(x[REGMUX]);}      
int GetPSRMUX(int *x)        { return(x[PSRMUX]);}
int GetVECMUX(int *x)        { return((x[VECMUX1] << 1) + x[VECMUX0]); }
// new loads lab 5
int GetLD_VA(int *x)         { return(x[LD_VA]);}
int GetLD_RET(int *x)        { return(x[LD_RET]);}
int GetLD_RW(int *x)         { return(x[LD_RW]);}
int GetLD_SAVEMDR(int *x)    { return(x[LD_SAVEMDR]);}
// new gates lab 5.. Gate VA unused :(
int GetGATE_PTBR(int *x)     { return(x[GATE_PTBR]);}
// new muxes
int GetRETMUX(int *x)        { return((x[RETMUX3] << 3) + (x[RETMUX2] << 2) +(x[RETMUX1] << 1) + x[RETMUX0]);}
int GetVAMUX(int *x)         { return(x[VAMUX]);}
int GetRWMUX(int *x)         { return(x[RWMUX]);}
int GetVARETMUX(int *x)      { return(x[VARETMUX]);}
int GetMDRMUX(int *x)        { return((x[MDRMUX1] << 1) + x[MDRMUX0]); }
int GetEXCMUX(int *x)        { return(x[EXCMUX]);}

//tasks
// VAMUX
// update exception checking

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
int VECTOR;
int EXC;
int INTERRUPT;
int PSR; 
int USP;
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */

/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
int RET;
int RW;
int SAVEMDR;
/* MODIFY: you should add here any other registers you need to implement virtual memory */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  if(CYCLE_COUNT == 300){
    CURRENT_LATCHES.INTV = 0x01;
    CURRENT_LATCHES.INTERRUPT = 0x01;
  }

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  NEXT_LATCHES.PSR = Low16bits((NEXT_LATCHES.PSR & 0xFFF8) + (NEXT_LATCHES.N << 2) + (NEXT_LATCHES.Z << 1) + (NEXT_LATCHES.P));

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.USP = 0xFE00; /* Initial value of user stack pointer */
    CURRENT_LATCHES.PSR = 0x8002; /* Initial value of PSR */
    CURRENT_LATCHES.VECTOR = 0;
    CURRENT_LATCHES.INTERRUPT = 0;
    CURRENT_LATCHES.EXC = 0;

/* MODIFY: you can add more initialization code HERE */

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

/* send bit masked offset (leading 0's), returns sext offset */
int getSext(int offset, int bits){
    int m = 1U << (bits - 1); 
    offset = (offset ^ m) - m; 
    return offset;
}

void setcc(int sum){
    /* clear */
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
    /* set just 1 */
    if(sum == 0){
        NEXT_LATCHES.Z = 1;
    }
    else if((sum & 0x08000) != 0){
        NEXT_LATCHES.N = 1;
    }
    else{
        NEXT_LATCHES.P = 1;
    }
}

void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */
    // initial copy
    NEXT_LATCHES = CURRENT_LATCHES;

    // get next state
    // get j bits
    int j = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
    // get cond
    int cond = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
    // get ben
    int ben = CURRENT_LATCHES.BEN;
    // get ready
    int ready = CURRENT_LATCHES.READY;
    // get ir[11]
    int ir11 = (CURRENT_LATCHES.IR & (0x0800)) >> 11;
    // get ird
    int ird = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    // get ir[15:12]
    int  opcode = (CURRENT_LATCHES.IR & (0x0F000)) >> 12;
    // get interrupt
    int interrupt = CURRENT_LATCHES.INTERRUPT;
    // get exception
    int exc = CURRENT_LATCHES.EXC;
    // get va return
    int varetmux = GetVARETMUX(CURRENT_LATCHES.MICROINSTRUCTION);

    if(ird == 1){
        NEXT_LATCHES.STATE_NUMBER = opcode & 0x0F;
    }
    else{
        switch(cond){
            case 0:
                break;
            case 2:
                if(ben == 1){
                    j |= 4;
                }
                break;
            case 1:
                if(ready == 1){
                    j |= 2;
                }
                break;
            case 3:
                if(ir11 == 1){
                    j |= 1;
                }
                break;
            case 4:
                if(interrupt == 1){
                    j |= 12;
                    NEXT_LATCHES.INTERRUPT = 0;
                }
                break;
            case 5:
                if(exc == 1){
                    j |= 5;
                }
                break;
            default:
                break;
        }
        NEXT_LATCHES.STATE_NUMBER = j & 0x3F;
    }

    if(varetmux == 1){
        NEXT_LATCHES.STATE_NUMBER = CURRENT_LATCHES.RET;
    }

    // get next microinstruction
    //for(int i = 0; i < CONTROL_STORE_BITS; i++){
    //    printf("%x", CONTROL_STORE[CURRENT_LATCHES.STATE_NUMBER][i]);
    //}
    // get next microinstruction
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    //for(int i = 0; i < CONTROL_STORE_BITS; i++){
    //    printf("%x", CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER][i]);
    //}
    //printf("\n");

}

int memcnt = 1;
int memval = 0; 

void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */

   int datasize = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
   int rw = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
   int mar0 = CURRENT_LATCHES.MAR & 0x01;
   switch(rw){
        case 0:
            // read all 16 bits and load into mdr
            memval = Low16bits((MEMORY[CURRENT_LATCHES.MAR >> 1][0] & 0x0FF) + ((MEMORY[CURRENT_LATCHES.MAR >> 1][1] & 0x0FF) << 8));
            break;
        case 1:
            // write byte
            if(datasize == 0){
                if(mar0 == 0){
                // MDR 7:0 written to memory
                    MEMORY[CURRENT_LATCHES.MAR >> 1][0] = Low16bits(CURRENT_LATCHES.MDR & 0x00FF);
                }
                else if(mar0 == 1){
                // MDR 15:8 written to memory
                    MEMORY[CURRENT_LATCHES.MAR >> 1][1] = Low16bits((CURRENT_LATCHES.MDR & 0x0FF00) >> 8);
                }
            }  
            // write word
            else{
                if(mar0 == 1){
                    // illegal operand address exceptions
                }
                MEMORY[CURRENT_LATCHES.MAR >> 1][0] = Low16bits(CURRENT_LATCHES.MDR & 0x00FF);
                MEMORY[CURRENT_LATCHES.MAR >> 1][1] = Low16bits((CURRENT_LATCHES.MDR & 0x0FF00) >> 8); 
            }
            break;
   }


   int cond = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
   // if mem access
   if(cond == 1){
        // 4th cycle, set R to 1
        if(memcnt == 4){
            NEXT_LATCHES.READY = 1;
        }
        // loop for 5 times
        if(memcnt % 5 != 0){
            memcnt++;
        }
        else {
            // reset ready
            NEXT_LATCHES.READY = 0;
            memcnt = 1;
        }
   }

}

int PCtoBUS = 0;
int ALUtoBUS = 0;
int SHFtoBUS = 0;
int MARMUXtoBUS = 0;
int MDRtoBUS = 0;
int saveAddrResultforPCMUX = 0;
int R6MINUXtoBUS = 0;
int R6PLUStoBUS = 0;
int PCMINUStoBUS = 0;
int SSPtoBUS = 0;
int PSRtoBUS = 0;
int VECTORSHIFTEDtoBUS = 0;
int PTBRtoBUS = 0;

void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *         Gate_PC,
   *         Gate_ALU,
   *         Gate_SHF,
   *         Gate_MDR.
   */ 

   // PC
   PCtoBUS = Low16bits(CURRENT_LATCHES.PC); 

   // ALU
   int sr1 = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
   int A;
   switch(sr1){
        case 0:
            // get IR 11:9
            A = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 9];
            break;
        case 1:
            // get IR 8:6
            A = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x01C0) >> 6];
            break;
        case 2:
            A = CURRENT_LATCHES.REGS[6];
            break;
   }
   int B;
   int sr2 = (CURRENT_LATCHES.IR & 0x0020) >> 5;
   switch(sr2){
        case 0:
            // ir 2:0
            B = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x0007];
            break;
        case 1:
            // imm5
            B = getSext(CURRENT_LATCHES.IR & 0x01F, 5);
            break;
   }
   int aluk = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);
   switch(aluk){
        case 0:
            // add
            ALUtoBUS = Low16bits(A + B);
            break;
        case 1:
            // and
            ALUtoBUS = Low16bits(A & B);
            break;
        case 2:
            // xor
            ALUtoBUS = Low16bits(A ^ B);
            break;
        case 3:
            // pass a
            ALUtoBUS = Low16bits(A);
            break;
   }

   //SHF
   int shiftamt = CURRENT_LATCHES.IR & 0x0F;
   // left, right, arithmatic
   int AD = (CURRENT_LATCHES.IR & 0x030) >> 4;
   switch(AD){
        case 0:
            // left
            SHFtoBUS = Low16bits(A << shiftamt);
            break;
        case 1:
            // right
            SHFtoBUS = Low16bits(A >> shiftamt);
            break;
        case 3:
            // right, sign extend
            SHFtoBUS = Low16bits(getSext(A >> shiftamt, 16 - shiftamt));
            break;
   }

   //MARMUX
   int addr1mux = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
   int addr1muxResult;
   int addr2mux = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
   int addr2muxResult;
   int marmux = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
   int lshf = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);
   switch(addr1mux){
        case 0:
            // PC
            addr1muxResult = Low16bits(CURRENT_LATCHES.PC);
            break;
        case 1:
            addr1muxResult = Low16bits(A);
            break;
   }
   switch(addr2mux){
        case 0:
            // 0
            addr2muxResult = Low16bits(0);
            break;
        case 1:
            // offset6
            addr2muxResult = Low16bits(getSext(CURRENT_LATCHES.IR & 0x03F, 6));
            break;
        case 2:
            // offset9
            addr2muxResult = Low16bits(getSext(CURRENT_LATCHES.IR & 0x01FF, 9));
            break;
        case 3:
            // offset11
            addr2muxResult = Low16bits(getSext(CURRENT_LATCHES.IR & 0x07FF, 11));
            break;
   }
   if(lshf == 1){
        addr2muxResult = Low16bits(addr2muxResult << 1);
   }
   int fromAdder = Low16bits(addr2muxResult + addr1muxResult);
   saveAddrResultforPCMUX = fromAdder;
   int fromZEXT = Low16bits((CURRENT_LATCHES.IR & 0x0FF) << 1);
   switch(marmux){
        case 0:
            // ir 7:0
            MARMUXtoBUS = Low16bits(fromZEXT);
            break;
        case 1:
            // adder
            MARMUXtoBUS = Low16bits(fromAdder);
            break;
   }

   //MDR
   int datasize = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
   int mar0 = CURRENT_LATCHES.MAR & 0x01;
   int mdrLoad;
   switch(mar0){
        case 0:
            // bits 7:0
            mdrLoad = (CURRENT_LATCHES.MDR & 0x0FF);
            break;
        case 1:
            // bits 15:8
            mdrLoad = (CURRENT_LATCHES.MDR & 0x0FF00) >> 8;
            break;
   }
   switch(datasize){
        case 0:
            // byte
            MDRtoBUS = Low16bits(getSext(mdrLoad, 8));
            break;
        case 1:
            // word
            MDRtoBUS = Low16bits(CURRENT_LATCHES.MDR);
            break;
   }

   // R6-2
   R6MINUXtoBUS = Low16bits(CURRENT_LATCHES.REGS[6] - 2);
   // R6+2
   R6PLUStoBUS = Low16bits(CURRENT_LATCHES.REGS[6] + 2);
   // PC-2
   PCMINUStoBUS = Low16bits(CURRENT_LATCHES.PC - 2);
   // SSP-2
   SSPtoBUS = Low16bits(CURRENT_LATCHES.SSP - 2);
   // PSR
   PSRtoBUS = Low16bits(CURRENT_LATCHES.PSR);
   // Vector LSHF
   VECTORSHIFTEDtoBUS = Low16bits( (0x0200) + ((CURRENT_LATCHES.VECTOR & 0x0FF) << 1)) ;
   //PTBR
   PTBRtoBUS = Low16bits(CURRENT_LATCHES.PTBR + ((CURRENT_LATCHES.VA >> 9) << 1));
}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */  
   int gatePC = GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateALU = GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateSHF = GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateMARMUX = GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateMDR = GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION);
   int gatePCminus = GetGATE_PCMINUS2(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateVECSHF = GetGATE_VECSHF(CURRENT_LATCHES.MICROINSTRUCTION);
   int gatePSR = GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateR6plus = GetGATE_R6PLUS2(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateR6minus = GetGATE_R6MINUS2(CURRENT_LATCHES.MICROINSTRUCTION);
   int gateSSP = GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION);
   int gatePTBR = GetGATE_PTBR(CURRENT_LATCHES.MICROINSTRUCTION);
   
   
   if(gatePC == 1){
        BUS = Low16bits(PCtoBUS);
   }
   else if(gateALU == 1){
        BUS = Low16bits(ALUtoBUS);
   }
   else if(gateSHF == 1){
        BUS = Low16bits(SHFtoBUS);
   }
   else if(gateMARMUX == 1){
        BUS = Low16bits(MARMUXtoBUS);
   }
   else if(gateMDR == 1){
        BUS = Low16bits(MDRtoBUS);
   }
   else if(gatePCminus == 1){
        BUS = Low16bits(PCMINUStoBUS);
   }
   else if(gateR6minus == 1){
        BUS = Low16bits(R6MINUXtoBUS);
   }
   else if(gateR6plus == 1){
        BUS = Low16bits(R6PLUStoBUS);
   }
   else if(gatePSR == 1){
        BUS = Low16bits(PSRtoBUS);
   }
   else if(gateSSP == 1){
        BUS = Low16bits(SSPtoBUS);
   }
   else if(gateVECSHF == 1){
        BUS = Low16bits(VECTORSHIFTEDtoBUS);
   }
   else if(gatePTBR == 1){
        BUS = Low16bits(PTBRtoBUS);
   }
   else {
        BUS = Low16bits(0);
   }     
}


void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */
   int ldcc = GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldben = GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldmdr = GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldmar = GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldpc = GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldir = GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldreg = GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldexc = GetLD_EXC(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldexcv = GetLD_EXCV(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldvector = GetLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldpsr15 = GetLD_PSR15(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldpsr = GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldssp = GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldusp = GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldva = GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldret = GetLD_RET(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldrw = GetLD_RW(CURRENT_LATCHES.MICROINSTRUCTION);
   int ldsavemdr = GetLD_SAVEMDR(CURRENT_LATCHES.MICROINSTRUCTION);

   if(ldcc == 1){
        setcc(BUS);
   }
   if(ldben == 1){
        int irn = CURRENT_LATCHES.N & ((CURRENT_LATCHES.IR & 0x0800) >> 11);
        int irz = CURRENT_LATCHES.Z & ((CURRENT_LATCHES.IR & 0x0400) >> 10);
        int irp = CURRENT_LATCHES.P & ((CURRENT_LATCHES.IR & 0x0200) >> 9);
        NEXT_LATCHES.BEN = (irn | irz) | irp;
   }
   if(ldmdr == 1){
        int mioen = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
        int datasize = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
        int mioenresult = 0;
        int mdrmux = GetMDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(mioen){
                // load from bus
            case 0:
                if(datasize == 0){
                    // store 7:0 7:0 in mdr
                    mioenresult = Low16bits( (BUS & 0x0FF) | ((BUS & 0x0FF) << 8) );
                }
                else if(datasize == 1){
                    // store 15:0 in mdr
                    mioenresult = Low16bits(BUS);
                }
                break;
            case 1:
                // load from memory
                mioenresult= Low16bits(memval);
                break;
        }
        switch(mdrmux){
            case 0:
                // from mioen
                NEXT_LATCHES.MDR = Low16bits(mioenresult);
                break;
            case 1:
                // mdr(15:2) ' r.w ' 1
                NEXT_LATCHES.MDR = Low16bits((CURRENT_LATCHES.MDR & 0x0FFFC) + (CURRENT_LATCHES.RW << 1) + 1);
                break;
            case 2:
                // from savemdr
                NEXT_LATCHES.MDR = Low16bits(CURRENT_LATCHES.SAVEMDR);
                break;
        }
   }
   if(ldmar == 1){
        int vamux = GetVAMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(vamux){
            case 0:
                // from bus
                NEXT_LATCHES.MAR = Low16bits(BUS);
                break;
            case 1:
                // from vaddr and mdr (pte)
                NEXT_LATCHES.MAR = Low16bits( (CURRENT_LATCHES.MDR & 0x3E00) + (CURRENT_LATCHES.VA & 0x01FF) );
                break;
        }
   }
   if(ldpc == 1){
        int pcmux = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(pcmux){
            case 0:
                // pc + 2
                NEXT_LATCHES.PC =  Low16bits(CURRENT_LATCHES.PC + 2);
                break;
            case 1:
                // bus
                NEXT_LATCHES.PC = Low16bits(BUS);
                break;
            case 2:
                // adder
                NEXT_LATCHES.PC = Low16bits(saveAddrResultforPCMUX);
               break;
        }
   }
   if(ldir == 1){
        NEXT_LATCHES.IR = Low16bits(BUS);
   }
   if(ldreg == 1){
        int regmux = GetREGMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        int regvalue = 0;
        switch(regmux){
            case 0:
                // from bus
                regvalue = Low16bits(BUS);
                break;
            case 1:
                // from usp
                regvalue = Low16bits(CURRENT_LATCHES.USP);
                break;
        }
        int dr = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(dr){
            case 0:
                // ir 11:9
                NEXT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 9] = Low16bits(regvalue);
                break;
            case 1: 
                // r7
                NEXT_LATCHES.REGS[7] = Low16bits(regvalue);
                break;
            case 2:
                NEXT_LATCHES.REGS[6] = Low16bits(regvalue);
                break;
        }

   }
   if(ldexcv == 1){
        int excmux = GetEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION);

        switch(excmux){
            case 0:
                // ignore if STB/LDB
                // checking for unaligned accesses
                break;
            case 1:
                // checking for page fault / protected
                // protection gets priority
                break;
        }
        int lshf = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);
        int pcgate = GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION);

        NEXT_LATCHES.EXCV = Low16bits(0);
        if(BUS & 0x01 != 0 &&  lshf == 1){
            NEXT_LATCHES.EXCV = Low16bits(3);
        }
        if(BUS & 0x01 != 0 && pcgate == 1){
            NEXT_LATCHES.EXCV = Low16bits(3);
        }
        // replace if protection also occurs
        if(((CURRENT_LATCHES.PSR & 0x8000) != 0) && (Low16bits(BUS) < 0x3000)){
            NEXT_LATCHES.EXCV = Low16bits(2);
        }
   }
   if(ldexc == 1){
        if (NEXT_LATCHES.EXCV != 0){
            NEXT_LATCHES.EXC = Low16bits(1);
        }
        else{
            NEXT_LATCHES.EXC = Low16bits(0);
        }
   }
   if(ldvector == 1){
        int vecmux = GetVECMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(vecmux){
            case 0:
                NEXT_LATCHES.VECTOR = Low16bits(0x4);
                break;
            case 1:
                NEXT_LATCHES.VECTOR = Low16bits(CURRENT_LATCHES.INTV);
                break;
            case 2:
                NEXT_LATCHES.VECTOR = Low16bits(CURRENT_LATCHES.EXCV);
                break;
        }
    
   }
   if(ldpsr15 == 1){
        int psr15 = GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(psr15){
            case 0:
                NEXT_LATCHES.PSR = Low16bits(0);
                break;
            case 1:
                NEXT_LATCHES.PSR = Low16bits(0x8000);
                break;
        }
   }
   if(ldpsr == 1){
        NEXT_LATCHES.PSR = Low16bits(BUS);
        NEXT_LATCHES.N = Low16bits((BUS & 0x4) >> 2);
        NEXT_LATCHES.Z = Low16bits((BUS & 0x2) >> 1);
        NEXT_LATCHES.P = Low16bits((BUS & 0x1));
   }
   if(ldssp == 1){
        NEXT_LATCHES.SSP = Low16bits(CURRENT_LATCHES.REGS[6] + 2);
   }
   if(ldusp == 1){
        NEXT_LATCHES.USP = Low16bits(CURRENT_LATCHES.REGS[6]);
   }
   if(ldva == 1){
        NEXT_LATCHES.VA = Low16bits(NEXT_LATCHES.MAR);
   }
   if(ldret == 1){
        int retmux = GetRETMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(retmux){
            case 0:
                NEXT_LATCHES.RET = Low16bits(40);
                break;
            case 1:
                NEXT_LATCHES.RET = Low16bits(41);
                break;
            case 2:
                NEXT_LATCHES.RET = Low16bits(36);
                break;
            case 3:
                NEXT_LATCHES.RET = Low16bits(37);
                break;
            case 4:
                NEXT_LATCHES.RET = Low16bits(60);
                break;
            case 5:
                NEXT_LATCHES.RET = Low16bits(33);
                break;
            case 6:
                NEXT_LATCHES.RET = Low16bits(28);
                break;
            case 7:
                NEXT_LATCHES.RET = Low16bits(25);
                break;
            case 8:
                NEXT_LATCHES.RET = Low16bits(23);
                break;
            case 9:
                NEXT_LATCHES.RET = Low16bits(24);
                break;
            case 10:
                NEXT_LATCHES.RET = Low16bits(29);
                break;
        }
   }
   if(ldrw == 1){
        int rwmux = GetRWMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        switch(rwmux){
            case 0:
                NEXT_LATCHES.RW = Low16bits(0);
                break;
            case 1:
                NEXT_LATCHES.RW = Low16bits(1);
                break;
        }
   }
   if(ldsavemdr == 1){
        NEXT_LATCHES.SAVEMDR = Low16bits(CURRENT_LATCHES.MDR);
   }


    // MAKE SURE TO USE NEXT STATE FOR ALL OF THESE !!!!!!!!!!!!!!    

}