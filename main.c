#include <stdio.h>
#include <string.h>
#include "htmlTable.h"
#include "parkingTicketsADT.h"

#define MAX_CHARS 80 // Each line of file can have up to 80 chars
#define MAX_T 10
#define DELIM ";"
#define IS_NYC 0
#define IS_CHI 1
#define END_OF_LINE "\n"

/*  Reads the .csv file for tickets and extracts the plate, infractionId,
* fineAmount and the issuingAgency. Then updates de ADT with the file data
*/
void readTickets(FILE * fileTickets, parkingTicketsADT infraction, int * city);

/* Reads the.csv file for infractions and extracts the id and the description of
* the infraction. Then updates de ADT with the file data
*/
void readInfractions(FILE * fileInfractions, parkingTicketsADT infraction);

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

    int city;
    parkingTicketsADT infraction = newParking();

    readTickets(fileTickets, infraction, &city);
    readInfractions(fileInfractions, infraction);
    
    fclose(fileTickets);
    fclose(fileInfractions);

    query1(infraction);
    query2(infraction);
    query3(infraction);

    freeParkingTickets(infraction);
}

void checkTok(char * temp){
    if(temp == NULL){
            fprintf(stderr, "Token error\n");
            exit(ERROR_TOK);
    }
}

void readInfractions(FILE * fileInfractions, parkingTicketsADT infraction) {
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

        ok = addInfraction(infraction, id, description);
        if(ok == 0) {
            fprintf(stderr, "Error addInfraction\n");
            exit(errno);
        }
    }
}

void readTickets(FILE * fileTickets, parkingTicketsADT infraction, int * city) {
    char line[MAX_CHARS];
    char * temp;

    if(fscanf(fileTickets, "%s\n", line) != 1) {
        fprintf(stderr, "Error reading first line\n");
        exit(ERROR_READ);
    }

    temp = strtok(line, DELIM);
    int ok;
    //Based on the first parameter, it decides whether it is NYC or CHI
    if(strcmp(temp, "plate") == IS_NYC){
        *city = IS_NYC;
        char plate[MAX_PLATE];
        size_t infractionId;
        char issuingAgency[MAX_AG];

        while(fgets(line, MAX_CHARS, fileTickets)!= NULL){
            temp = strtok(NULL, DELIM);
            checkTok(temp);
            strncpy(plate, temp, strlen(temp));
            plate[strlen(temp)] = '\0';
            //Ignore this temp because the issueDate is not needed for the queries
            strtok(NULL, DELIM);

            temp = strtok(NULL, DELIM);
            checkTok(temp);
            infractionId = atoi(temp);
            //Ignore this temp because the fineAmount is not needed for the queries
            strtok(NULL, DELIM);

            temp = strtok(NULL, END_OF_LINE);
            checkTok(temp);
            strncpy(issuingAgency, temp, strlen(temp));
            issuingAgency[strlen(temp)] = '\0';

            ok = addTicket(infraction, issuingAgency, infractionId, plate);
            if(ok == 0) {
                fprintf(stderr, "Error addInfraction\n");
                exit(errno);
            }
        }
    } else { //the file belongs to ticketsCHI
        *city = IS_CHI;
        char plateRedacted[MAX_PLATE];
        char unitDescription[MAX_AG];
        size_t infractionCode;

        while(fgets(line, MAX_CHARS, fileTickets)!= NULL){
            //Ignore this temp because the issueDate is not needed
            temp = strtok(NULL, DELIM);
            checkTok(temp);
            strncpy(plateRedacted, temp, strlen(temp));
            plateRedacted[strlen(temp)] = '\0';

            temp = strtok(NULL, DELIM);
            checkTok(temp);
            strncpy(unitDescription, temp, strlen(temp));
            unitDescription[strlen(temp)] = '\0';

            temp = strtok(NULL, END_OF_LINE);
            checkTok(temp);
            infractionCode = atoi(temp);

            ok = addTicket(infraction, unitDescription, infractionCode, plateRedacted);
            if(ok == 0) {
                fprintf(stderr, "Error addInfraction\n");
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

    char infraction[MAX_DESC];

    fputs("infraction;tickets\n", query1File);

    toBeginCount(p);
    while(hasNextCount(p)){
        size_t infractionCount;
        char * infractionName = nextCount(p, &infractionCount);
        if(infractionName != NULL) {
            fprintf(query1File, "%s;%zu\n", infractionName, infractionCount);
            snprintf(infraction, MAX_DESC, "%zu", infractionCount);
            addHTMLRow(table1, infractionName, infraction);
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

