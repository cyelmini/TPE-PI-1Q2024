#include <stdio.h>
#include <string.h>
#include "htmlTable.h"
#include "parkingTicketsADT.h"

#define MAX_CHARS 80 // Each line of file can have up to 80 chars
#define DELIM ";"
#define IS_NYC 0
#define IS_CHI 1
#define END_OF_LINE "/n"

/*  Reads the .csv file for tickets and extracts the plate, infractionId,
* fineAmount and the issuingAgency. Then updates de ADT with the file data
*/
void readTickets(FILE * fileTickets, parkingTicketsADT infraction, int * city);

/* Reads the.csv file for infractions and extracts the id and the description of
* the infraction. Then updates de ADT with the file data
*/
void readInfractions(FILE * fileInfractions, parkingTicketsADT infraction);


/*------------------------------------------------ QUERIES -----------------------------------------------------------*/

/* TOTAL FINES PER INFRACTION
 * Each line of the output should contain, separated by “;”, the name of the infraction and
 * the total number of fines for that infraction. The information should be listed in
 * descending order by the total number of fines, and if there is a tie, sort alphabetically
 * by the name of the violation.
 */
void query1(parkingTicketsADT p);

/* MOST POPULAR INFRACTION BY ISSUING AGENCY
 * Each line of the output should contain, separated by “;”, the name of the issuing agency,
 * the most popular infraction from that issuing agency, and the corresponding number of fines.
 * The most popular infraction of an issuing agency is the one with the highest number of fines.
 * In case there are two or more infractions with the same number of fines for the same
 * issuing agency, consider the infraction with the lowest alphabetical order.
 * The information should be listed in alphabetical order by issuing agency.
 */
void query2(parkingTicketsADT p);

/* LICENSE PLATE WITH MOST FINES PER INFRACTION
 * Each line of the output should contain, separated by “;”, the name of the infraction,
 * the license plate with the highest number of fines for that infraction, and the number of fines.
 * In case there are two or more license plates with the same number of fines for the same
 * infraction, consider the license plate with the lowest alphabetical order. The information should be listed
 * in alphabetical order by infraction.
 */
void query3(parkingTicketsADT p);

/*--------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char * argv[]){

    if (argc != 2){
        fprintf(sterr, "Invalid amount of arguments\n");
        exit(ERROR_ARG);
    }
    
    FILE * fileTickets = fopen(argv[1], "rt");
    FILE * fileInfractions = fopen(argv[2], "rt");

    if(fileTickets == NULL || fileInfractions == NULL){
        fprintf(stderr, "Unable to open files\n");
        exit(ERROR_OPEN);
    }

    int city;
    parkingTicketsADT infraction = newParking();

    readTickets(fileTickets, infraction, &city);
    readInfractions(fileInfractions, infraction);
    
    freeParkingTickets(infraction);
}

void readInfractions(FILE * fileInfractions, parkingTicketsADT infraction){
    char line[MAX_CHARS];
    char * temp;

    //me parece que esta condicion no hay que chequearla por enunciado
    if (fscanf(fileInfractions, "%s\n", line) != 1) {
        fprintf(stderr, "Error reading first line\n");
        exit(ERROR_READ);
    }

    size_t id;
    char description[51];
    while(fgets(line, MAX_CHARS, fileInfractions)!= NULL){
        temp = strtok(line, DELIM);
        if(temp == NULL){
            fprintf(stderr, "Token error\n");
            exit(ERROR_TOKEN);
        }
        id = temp(atoi);

        temp = strtok(line, DELIM);
        if(temp == NULL){
            fprintf(stderr, "Token error\n");
            exit(ERROR_TOKEN);
        }
        strncpy(description, temp, sizeof(description) - 1);
        plate[sizeof(description) - 1] = '\0';

        //Falta la funcion que agrega la info de la infraccion al TAD
    }
}

void readTickets(FILE * fileTickets, parkingTicketsADT infraction, int * city){
    char line[MAX_CHARS];
    char * temp;

    if (fscanf(fileTickets, "%s\n", line) != 1) {
        fprintf(stderr, "Error reading first line\n");
        exit(ERROR_READ);
    }

    temp = strtok(line, DELIM);
    
    //Based on the first parameter, it decides whether it is NYC or CHI
    if(strcmp(temp, "plate") == IS_NYC){
        *city = IS_NYC
        char plate[MAX_PLATE];
        size_t infractionId;
        char issuingAgency[MAX_AG];

        while(fgets(line, MAX_CHARS, fileTickets)!= NULL){
            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                fprintf(stderr, "Token error\n");
                exit(ERROR_TOKEN);
            }
            strncpy(plate, temp, sizeof(plate) - 1);
            plate[sizeof(plate) - 1] = '\0';
            //Ignore this temp because the issueDate is not needed for the queries
            strtok(NULL, DELIM);

            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                fprintf(stderr, "Token error");
                exit(ERROR_TOKEN);
            }
            infractionId = atoi(temp);
            //Ignore this temp because the fineAmount is not needed for the queries
            strtok(NULL, END_OF_LINE);

            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                fprintf(stderr, "Token error");
                exit(ERROR_TOKEN);
            }
            strncpy(issuingAgency, temp, sizeof(issuingAgency) - 1);
            issuingAgency[strlen(issuingAgency) - 1] = '\0';

            //Falta la funcion que agrega la info del ticket al TAD
            
        }
    } else{ //the file belongs to ticketsCHI
        *city = IS_CHI;
        char plateRedacted[11];
        char unitDescription[14];
        size_t infractionCode;

        while(fgets(line, MAX_CHARS, fileTickets)!= NULL){
            //Ignore this temp beacuse the issueDate is not needed
            temp = strtok(NULL, DELIM);

            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                fprintf(stderr, "Token error");
                exit(ERROR_TOKEN);
            }
            strncpy(plateRedacted, temp, sizeof(plateRedacted) - 1);
            plateRedacted[sizeof(plateRedacted) - 1] = '\0';
            
            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                fprintf(stderr, "Token error");
                exit(ERROR_TOKEN);
            }
            strncpy(unitDescription, temp, sizeof(unitDescription) - 1);
            unitDescription[sizeof(unitDescription) - 1] = '\0';

            temp = strtok(NULL, END_OF_LINE);
            if(temp == NULL){
                fprintf(stderr, "Token error");
                exit(ERROR_TOKEN);
            }
            infractionCode = atoi(temp);

            //Falta la funcion que agrega la info del ticket al TAD
        }
    }
}

void query1(parkingTicketsADT p){
    FILE * query1 = fopen("query1.csv", "wt");
    if(query1 == NULL){
        fprintf(stderr, "Error in file generation\n");
        exit(ERROR_OPEN);
    }
    toBeginCount(p);
    while(hasNextCount(p)){
        size_t infractionCount;
        char * infractionName = nextCount(p, &infractionCount);
        if(infractionName != NULL) {
            fprintf(query1, "%s;%zu\n", infractionName, infractionCount);
        }
    }
    fclose(query1);
}

void query2(parkingTicketsADT p){
    FILE * query2 = fopen("query2.csv", "wt");
    if(query2 == NULL){
        fprintf(stderr, "Error in file generation\n");
        exit(ERROR_OPEN);
    }
    toBeginAg(p);
    while (hasNextAg(p)) {
        char * mostPopularInf;
        size_t infractionCount;
        char * agency = nextAg(p, &mostPopularInf, &infractionCount);
        if(agency != NULL) {
            fprintf(query2, "%s;%s;%zu\n", agency, mostPopularInf, infractionCount);
        }
    }
    fclose(query2);
}

void query3(parkingTicketsADT p){
    FILE * query3 = fopen("query3.csv", "wt");
    toBeginAlpha(p);
    while(hasNextAlpha(p)){
        char * maxPlate;
        size_t infractionCount;
        char * infractionName = nextAlpha(p, &maxPlate, &infractionCount);
        if(infractionName != NULL) {
            fprintf(query3, "%s;%s;%zu\n", infractionName, maxPlate, infractionCount);
        }
    }
    fclose(query3);
}