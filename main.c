#include <stdio.h>
#include <string.h>
#include "htmlTable.h"
#include "parkingTicketsADT.h"

#define MAX_CHARS 80 // Each line of file can have up to 80 chars
#define MAX_AGENCY 37
#define DELIM ";"
#define END_OF_LINE "\n"
#define MAX_COUNT 11
#define HEADER "plate;issueDate;infractionId;fineAmount;issuingAgency"

/*  Reads the .csv file for tickets and extracts the plate, infractionId,
* fineAmount and the issuingAgency. Then updates de ADT with the file data
*/
void readTickets(FILE * fileTickets, parkingTicketsADT p);

/* Reads the.csv file for infractions and extracts the id and the description of
* the infraction. Then updates de ADT with the file data
*/
void readInfractions(FILE * fileInfractions, parkingTicketsADT p);

// Checks that the function strtok returns a valid argument
void checkTok(char * temp);

/*------------------------------------------------ QUERIES -----------------------------------------------------------*/

/* TOTAL FINES PER INFRACTION
 * Each line of the output should contain, separated by “;”, the name of the infraction and
 * the total number of fines for that infraction.
 */
void query1(parkingTicketsADT p);

/* MOST POPULAR INFRACTION BY ISSUING AGENCY
 * Each line of the output should contain, separated by “;”, the name of the issuing agency,
 * the most popular infraction from that issuing agency, and the corresponding number of fines.
 */
void query2(parkingTicketsADT p);

/* LICENSE PLATE WITH MOST FINES PER INFRACTION
 * Each line of the output should contain, separated by “;”, the name of the infraction,
 * the license plate with the highest number of fines for that infraction, and the number of fines.
 */
void query3(parkingTicketsADT p);

/*--------------------------------------------------------------------------------------------------------------------*/

int main(int argc, char * argv[]){

    if(argc != 3){
        fprintf(stderr, "Invalid amount of arguments\n");
        exit(ERROR_ARG);
    }
    
    FILE * fileTickets = fopen(argv[1], "rt");
    FILE * fileInfractions = fopen(argv[2], "rt");

    if(fileTickets == NULL || fileInfractions == NULL){
        fprintf(stderr, "Unable to open files\n");
        exit(ERROR_OPEN);
    }

    parkingTicketsADT p = newParking();

    readInfractions(fileInfractions, p);
    readTickets(fileTickets, p);
    
    fclose(fileTickets);
    fclose(fileInfractions);

    query1(p);
    query2(p);
    query3(p);

    freeParkingTickets(p);
}

void checkTok(char * temp){
    if(temp == NULL){
            fprintf(stderr, "Token error\n");
            exit(ERROR_TOK);
    }
}

/* Esta funcion parece andar bien???? */
void readInfractions(FILE * fileInfractions, parkingTicketsADT p) {
    char line[MAX_CHARS];
    char * temp;
    int ok;

    if (fscanf(fileInfractions, "%s\n", line) != 1) {
        fprintf(stderr, "Error reading first line\n");
        exit(ERROR_READ);
    }

    size_t id;
    char description[MAX_DESC];
    while(fgets(line, MAX_CHARS, fileInfractions) != NULL){
        temp = strtok(line, DELIM);
        checkTok(temp);
        id = atoi(temp);

        temp = strtok(NULL, DELIM);
        checkTok(temp);
        strncpy(description, temp, strlen(temp));
        description[strlen(temp)] = '\0';

        ok = addInfraction(p, id, description);
        if(!ok) {
            fprintf(stderr, "Error addInfraction\n");
            exit(errno);
        }
    }
}

void readTickets(FILE * fileTickets, parkingTicketsADT p) {
    char text[MAX_CHARS];

    fscanf(fileTickets, "%s\n", text);
    char * temp;
    int ok;

    if(strcmp(text, HEADER) == 0){
        char plate[MAX_PLATE];
        char issuingAgency[MAX_AGENCY];
        size_t infractionId;

        while(fgets(text, MAX_CHARS, fileTickets) != NULL){
            temp = strtok(text, DELIM);
            checkTok(temp);
            strcpy(plate, temp);
        

            temp = strtok(NULL, DELIM);
            checkTok(temp);

            temp = strtok(NULL, DELIM);
            checkTok(temp);
            infractionId = atoi(temp);
        

            temp = strtok(NULL, DELIM);
            checkTok(temp);

            temp = strtok(NULL, END_OF_LINE);
            checkTok(temp);
            strcpy(issuingAgency, temp);
        
            ok = addTicket(p, issuingAgency, infractionId, plate);
            if(!ok){
                fprintf(stderr, "Error adding to Ticket\n");
                exit(errno);
            }
        }
    } else{
        char plateRedacted[MAX_PLATE];
        char unitDescription[MAX_AGENCY];
        size_t infractionCode;

        while(fgets(text, MAX_CHARS, fileTickets) != NULL){
            temp = strtok(text, DELIM);
            checkTok(temp);

            temp = strtok(NULL, DELIM);
            checkTok(temp);
            strcpy(plateRedacted, temp);

            temp = strtok(NULL, DELIM);
            checkTok(temp);
            strcpy(unitDescription, temp);

            temp = strtok(NULL, DELIM);
            checkTok(temp);
            infractionCode = atoi(temp);
            ok = addTicket(p, unitDescription, infractionCode, plateRedacted);
            if(!ok){
                fprintf(stderr, "Error adding to Ticket\n");
                exit(errno);
            }
        }
    }
}


void query1(parkingTicketsADT p){
    FILE * query1File = fopen("query1.csv", "wt");
    htmlTable table1 = newTable("query1.html", 2, "infraction", "Tickets");

    if(query1File == NULL || table1 == NULL){
        fprintf(stderr, "Error in file generation\n");
        exit(ERROR_OPEN);
    }

    fputs("infraction;tickets\n", query1File);

    toBeginCount(p);
    if (errno != 0) {
        fprintf(stderr, 'Error toBeginCount\n');
        exit(errno);
    }
    
    char count[MAX_COUNT];

    while(hasNextCount(p)){
        if (errno != 0) {
            fprintf(stderr, 'Error hasNextCount\n');
            exit(errno);
        }
        size_t infractionCount;
        char * infractionName = nextCount(p, &infractionCount);

            fprintf(query1File, "%s;%zu\n", infractionName, infractionCount);
            sprintf(count, "%zu", infractionCount);
            addHTMLRow(table1, infractionName, count);
    }
    fclose(query1File);
    closeHTMLTable(table1);
}

void query2(parkingTicketsADT p){
    FILE * query2File = fopen("query2.csv", "wt");
    htmlTable table2 = newTable("query2.html", 3, "issuingAgency", "infraction", "tickets");

    if(query2File == NULL || table2 == NULL){
        fprintf(stderr, "Error in file generation\n");
        exit(ERROR_OPEN);
    }

    char infraction[MAX_DESC];

    fputs("issuingAgency;infraction;tickets\n", query2File);

    toBeginAg(p);
    while(hasNextAg(p)) {
        char * mostPopularInf;
        size_t infractionCount;
        char * agency = nextAg(p, &mostPopularInf, &infractionCount);
        if(agency != NULL) {
            fprintf(query2File, "%s;%s;%zu\n", agency, mostPopularInf, infractionCount);
            snprintf(infraction, MAX_DESC, "%zu", infractionCount);
            addHTMLRow(table2, agency, mostPopularInf, infraction);
        }
    }
    fclose(query2File);
    closeHTMLTable(table2);
}

void query3(parkingTicketsADT p){
    FILE * query3File = fopen("query3.csv", "wt");
    htmlTable table3 = newTable("query3.html", 3, "infraction", "plate", "tickets");

    if(query3File == NULL || table3 == NULL){
        fprintf(stderr, "Error in file generation\n");
        exit(ERROR_OPEN);
    }

    char infraction[MAX_DESC];

    toBeginAlpha(p);
    while(hasNextAlpha(p)){
        char * maxPlate;
        size_t infractionCount;
        char * infractionName = nextAlpha(p, &maxPlate, &infractionCount);
        if(infractionName != NULL) {
            fprintf(query3File, "%s;%s;%zu\n", infractionName, maxPlate, infractionCount);
            snprintf(infraction, MAX_DESC, "%zu", infractionCount);
            addHTMLRow(table3, infractionName, maxPlate, infraction);
        }
    }
    fclose(query3File);
    closeHTMLTable(table3);
}

