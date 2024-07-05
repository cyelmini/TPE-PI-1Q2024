#include <stdio.h>
#include <string.h>
#include "htmlTable.h"
#include "parkingTicketsADT.h"

#define MAX_CHARS 80 // Each line of file can have up to...
#define DELIM ";"
#define IS_NYC 0
#define IS_CHI 1
#define END_OF_LINE "/n"

// Reads the .csv file for tickets and extracts the plate, infractionId,
// fineAmount and the issuingAgency. Then updates de ADT with the file data
void readTickets(FILE * fileTickets, parkingTicketsADT infraction, int * city);


// Reads the.csv file for infractions and extracts the id and the description of
// the infraction. Then updates de ADT with the file data
void readInfractions(FILE * fileInfractions, parkingTicketsADT infraction);


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
    if (fscanf(fileTickets, "%s\n", line) != 1) {
        fprintf(stderr, "Error reading first line\n");
        exit(ERROR_READ);
    }

    size_t id;
    char description[51];
    while(fgets(line, MAX_CHARS, fileInfractions)!= NULL){
        temp = strtok(line, DELIM);
        if(temp == NULL){
            printf(stderr, "Error in Tok");
            exit(ERROR_TOKEN);
        }
        id = temp(atoi);

        temp = strtok(line, DELIM);
        if(temp == NULL){
            printf(stderr, "Error in Tok");
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
        char plate[11];
        size_t infractionId;
        char issuingAgency[36];

        while(fgets(line, MAX_CHARS, fileTickets)!= NULL){
            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                printf(stderr, "Error in Tok");
                exit(ERROR_TOKEN);
            }
            strncpy(plate, temp, sizeof(plate) - 1);
            plate[sizeof(plate) - 1] = '\0';
            //Ignore this temp because the issueDate is not needed for the queries
            strtok(NULL, DELIM);

            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                printf(stderr, "Error in Tok");
                exit(ERROR_TOKEN);
            }
            infractionId = atoi(temp);
            //Ignore this temp because the fineAmount is not needed for the queries
            strtok(NULL, END_OF_LINE);

            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                printf(stderr, "Error in Tok");
                exit(ERROR_TOKEN);
            }
            strncpy(issuingAgency, temp, sizeof(issuingAgency) - 1);
            plate[sizeof(issuingAgency) - 1] = '\0';

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
                printf(stderr, "Error in Tok");
                exit(ERROR_TOKEN);
            }
            strncpy(plateRedacted, temp, sizeof(plateRedacted) - 1);
            plateRedacted[sizeof(plateRedacted) - 1] = '\0';
            
            temp = strtok(NULL, DELIM);
            if(temp == NULL){
                printf(stderr, "Error in Tok");
                exit(ERROR_TOKEN);
            }
            strncpy(unitDescription, temp, sizeof(unitDescription) - 1);
            unitDescription[sizeof(unitDescription) - 1] = '\0';

            temp = strtok(NULL, END_OF_LINE);
            if(temp == NULL){
                printf(stderr, "Error in Tok");
                exit(ERROR_TOKEN);
            }
            infractionCode = atoi(temp);

            //Falta la funcion que agrega la info del ticket al TAD
        }
    }
}


/*---------------------------------------------- QUERIES -----------------------------------------------------------*/

/* TOTAL DE MULTAS POR INFRACCION
 * Donde cada línea de la salida contenga, separados por “;” el nombre de la infracción y
 * la cantidad total de multas con esa infracción. La información debe listarse ordenada
 * en forma descendente por la cantidad total de multas y a igualdad de multas desempatar
 * alfabéticamente por nombre de la infracción.
 */
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

/* INFRACCION MAS POPULAR POR AGENCIA EMISORA
 * Donde cada línea de la salida contenga, separados por “;” el nombre de la agencia emisora,
 * la infracción más popular de esa agencia emisora y la cantidad de multas correspondiente.
 * La infracción más popular de una agencia emisora es la que tiene la mayor cantidad de multas.
 * En caso de que existan dos o más infracciones con la misma cantidad de multas para la misma
 * agencia emisora considerar la menor infracción en orden alfabético.
 * La información debe listarse ordenada en forma alfabética por agencia emisora.
 */
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

/* PATENTE CON MAS MULTAS POR INFRACCION
 *  Donde cada línea de la salida contenga, separados por “;” el nombre de la infracción,
 *  la patente con la mayor cantidad de multas de esa infracción y la cantidad de multas.
 *  En caso de que existan dos o más patentes con la misma cantidad de multas para la misma
 *  infracción considerar la menor patente en orden alfabético. La información debe listarse
 *  ordenada en forma alfabética por infracción.
 */
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