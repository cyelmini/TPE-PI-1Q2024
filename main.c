/* Main that calls upon parkingTicketsCHI and parkingTicketsNYC to process the correspondent data */
#include <stdio.h>
#include <string.h>
#include "htmlTable.h"
#include "parkingTicketsADT.h"

#define MAX_CHARS 50 //each line of file can have up to 50 chars
#define DELIM ";"
#define IS_NYC 1
#define IS_CHI 0-

// Reads the .csv file for tickets and extracts the plate, infraction Id, 
// fineAmount and the issuingAgency and returns an ADT with the processed data
parkingTicketsADT readTickets(FILE * fileTickets, parkingTicketsADT infraction);

// Reads the .csv file for infractions and extracts the id and the descrption of
// the infraction, returns a new ADT with the processed data
parkingTicketsADT readInfractions(FILE * fileInfractions, parkingTicketsADT infraction);

int readCity(char * line);

int main(int argc, char * argv[]){

    if (argc != 2){
        fprintf(sterr, "Invalid amount of agruments\n");
        exit(ERROR_ARG);
    }
    
    FILE * fileTickets = fopen(argv[1], "rt");
    FILE * fileInfractions = fopen(argv[2], "rt");

    if(fileTickets == NULL || fileInfractions == NULL){
        fprintf(sterr, 'Unable to open files\n');
        exit(ERROR_OPEN);
    }

    parkingTicketsADT infraction = newParking();
}

int readTicketCity(char * line){
    char * temp = strtok(line, DELIM);
    return temp == "plate" ? IS_NYC : IS_CHI;
}



parkingTicketsADT readTickets(FILE * fileTickets, parkingTicketsADT infraction){
    char line[MAX_CHARS];
    int flag;
    char * temp;

    //Reads the header of the file and decides if it is NYC or CHI 
    fscanf(fileTickets, "%s\n", line); 
    flag = isCity(line);

    char * plate;
    size_t infractionId;
    char * issuingAgency;
    if(flag == IS_NYC){
       while(fgets(line, MAX_CHARS, fileTickets) != NULL){
        temp = strtok(line, DELIM);
        if(temp == NULL){
            fprintf(stderr, "Error in Tok");
            exit(ERROR_TOKEN);
        }
       }
    }
    if(flag == IS_CHI){
       //I skip the issueDate
    }
}
