#include <stdio.h>
#include <string.h>
#include "htmlTable.h"
#include "parkingTicketsADT.h"

#define MAX_CHARS 80
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

        line[strcspn(line, "\n")] = '\0';

        temp = strtok(line, DELIM);
        checkTok(temp);
        id = atoi(temp);

        temp = strtok(NULL, DELIM);

        strcpy(description, temp);

        ok = addInfraction(p, id, description);
        if(!ok) {
            fprintf(stderr, "Error addInfraction\n");
            exit(errno);
        }
    }
}

void readTickets(FILE * fileTickets, parkingTicketsADT p) {
    char text[MAX_CHARS];

    if(fscanf(fileTickets, "%s\n", text) != 1){
        fprintf(stderr, "Error reading first line\n");
        exit(ERROR_READ);
    }
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

            //issuePlate is not a value needed for the queries
            temp = strtok(NULL, DELIM);

            temp = strtok(NULL, DELIM);

            infractionId = atoi(temp);

            //fineAmount is not a value needed for the queries
            temp = strtok(NULL, DELIM);

            temp = strtok(NULL, END_OF_LINE);

            strcpy(issuingAgency, temp);

            ok = addTicket(p, issuingAgency, infractionId, plate);
            if(!ok){
                fprintf(stderr, "Error adding to Ticket\n");
                exit(errno);
            }
        }
    } else {
        char plateRedacted[MAX_PLATE];
        char unitDescription[MAX_AGENCY];
        size_t infractionCode;

        while(fgets(text, MAX_CHARS, fileTickets) != NULL){
            //issueDate is not a value needed for the queries
            temp = strtok(text, DELIM);
            checkTok(temp);

            temp = strtok(NULL, DELIM);

            strcpy(plateRedacted, temp);

            temp = strtok(NULL, DELIM);

            strcpy(unitDescription, temp);

            temp = strtok(NULL, DELIM);

            infractionCode = atoi(temp);
            ok = addTicket(p, unitDescription, infractionCode, plateRedacted);
            if(!ok){
                fprintf(stderr, "Error adding to Ticket\n");
                exit(errno);
            }
            //fineLevel1Amount is not needed for the queries,
            //therefore it skips the line completly
        }
    }
}

void query1(parkingTicketsADT p){
    FILE * query1File = fopen("query1.csv", "wt");
    htmlTable table1 = newTable("query1.html", 2, "infraction", "tickets");

    if(query1File == NULL || table1 == NULL){
        fprintf(stderr, "Error in file generation\n");
        exit(ERROR_OPEN);
    }

    fputs("infraction;tickets\n", query1File);

    toBeginCount(p);
    if (errno != OK) {
        fprintf(stderr, "Error in toBeginCount\n");
        exit(errno);
    }

    char count[MAX_COUNT];
    while(hasNextCount(p)){
        if (errno != OK) {
            fprintf(stderr, "Error hasNextCount\n");
            exit(errno);
        }
        size_t infractionCount;
        char * infractionName = nextCount(p, &infractionCount);

        if(infractionName != NULL){
            fprintf(query1File, "%s;%ld\n", infractionName, infractionCount);
            snprintf(count, MAX_COUNT, "%ld", infractionCount);
            addHTMLRow(table1, infractionName, count);
        }
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

    fputs("issuingAgency;infraction;tickets\n", query2File);

    char infraction[MAX_DESC];
    toBeginAg(p);
    if(errno != OK){
        fprintf(stderr, "Error in toBeginAg\n");
        exit(errno);
    }

    while(hasNextAg(p)) {
        if(errno != OK){
            fprintf(stderr, "Error in hasNextCount\n");
            exit(errno);
        }
        char * mostPopularInf;
        size_t infractionCount;
        char * agency = nextAg(p, &mostPopularInf, &infractionCount);
        if(agency != NULL) {
            fprintf(query2File, "%s;%s;%ld\n", agency, mostPopularInf, infractionCount);
            snprintf(infraction, MAX_DESC, "%ld", infractionCount);
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

    fputs("infraction;plate;tickets\n", query3File);

    char infraction[MAX_DESC];
    toBeginAlpha(p);
    if(errno != OK){
        fprintf(stderr, "Error in toBeginAlpha\n");
        exit(errno);
    }

    while(hasNextAlpha(p)){
        if(errno != OK){
            fprintf(stderr, "Error in hasNextAlpha\n");
            exit(errno);
        }
        char * maxPlate;
        size_t infractionCount;
        char * infractionName = nextAlpha(p, &maxPlate, &infractionCount);

        if(infractionName != NULL) {
            fprintf(query3File, "%s;%s;%ld\n", infractionName, maxPlate, infractionCount);
            snprintf(infraction, MAX_DESC, "%ld", infractionCount);
            addHTMLRow(table3, infractionName, maxPlate, infraction);
        }
    }
    fclose(query3File);
    closeHTMLTable(table3);
}


