/* Header with the functions used in the ADT and defined variables */

#ifndef TPE_PI_1Q2024_PARKINGTICKETSADT_H
#define TPE_PI_1Q2024_PARKINGTICKETSADT_H

#include <stdlib.h>

#define MAX_PLATE 10
#define MAX_DESC 50
#define MAX_AG 35
#define BLOCK 100

typedef struct parkingTicketsCDT * parkingTicketsADT;

/* Adds an infraction committed by a given plate to the correspondent agency that emitted it.
 * If the agency did not exist, it adds it to the list as well. Returns 1 if successfully done
 * and 0 if not.
 */
int addInfraction(parkingTicketsADT p, const char *agency, const char *infractionDesc, size_t infractionId, const char *plate);

/* Generates two lists of infractions that are sorted by
* infraction count (to be used in query 1) and alphabetically (to be used in query 3)
*/
void sortList(parkingTicketsADT p)


enum {OK = 0, ERROR_MEM, ERROR_TOK, ERROR_ARG, ERROR_OPEN};


/*
 * MEMERR = Memory error
 * TOKENERR = Token error
 */

#endif //TPE_PI_1Q2024_PARKINGTICKETSADT_H
