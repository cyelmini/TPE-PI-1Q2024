/* Main that calls upon parkingTicketsCHI and parkingTicketsNYC to process the correspondent data */
#include <stdio.h>
#include <string.h>
#include "htmlTable.h"
#include "parkingTicketsADT.h"

int main(int argc, char * argv[]){
    if (argc != 2){
        fprintf(sterr, "Invalid amount of agruments\n");
        exit(ERROR_ARG);
    }
    
    FILE * fileTickets = fopen(argv[0], "rt");
    FILE * fileInfractions = fopen(argv[1], "rt");

    if(fileTickets == NULL || fileInfractions == NULL){
        fprintf(sterr, 'Unable to open files\n');
        exit(ERROR_OPEN);
    }

    

    //parkingTicketsADT tickets = newTicket();
    
}