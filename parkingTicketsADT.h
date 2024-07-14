#ifndef TPE_PI_1Q2024_PARKINGTICKETSADT_H
#define TPE_PI_1Q2024_PARKINGTICKETSADT_H

#include <stdlib.h>

#define MAX_PLATE 15
#define MAX_DESC 55
#define MAX_AG 40

typedef struct parkingTicketsCDT * parkingTicketsADT;

/* Generates a system to process the data of the different
* infractions committed.
*/
parkingTicketsADT newParking(void);


/* Generates a reference for the inputted infraction id and
* the infraction description. Should be filled before inputting
* the tickets. Returns 1 if successfully done and 0 if not.
*/
int addInfraction(parkingTicketsADT p, size_t infractionId, const char *description);


/* Adds an infraction committed by a given plate to the correspondent agency that emitted it.
* If the agency did not exist, it adds it to the list as well. Returns 1 if successfully done
* and 0 if not.
*/
int addTicket(parkingTicketsADT p, const char *agency, size_t infractionId, const char *plate);


/* Frees the memory reserved
 * by the ADT.
 */
void freeParkingTickets(parkingTicketsADT p);


/* ------------------ Functions to iterate over the three main lists meant to resolve the queries --------------------*/

/* To iterate over the lists of agencies, sets iterator on
* the first element of such list.
*/
void toBeginAg(parkingTicketsADT p);


/* Returns 1 if there are more elements on the lists of agencies to
* iterate over, 0 is there are not.
*/
int hasNextAg(parkingTicketsADT p);


/* Returns the name of the agency that emitted the infraction, and by parameters the description
* of the most popular infraction and the amount of times it was committed.
*/
char * nextAg(parkingTicketsADT p, char ** mostPopularInf, size_t * infractionCount);


/* To iterate over the lists of infractions ordered by count, sets iterator on
* the first element of such list.
*/
void toBeginCount(parkingTicketsADT p);


/* Returns 1 if there are more elements on the lists of infractions ordered by count to
* iterate over, 0 is there are not.
*/
int hasNextCount(parkingTicketsADT p);


/* Returns the description of the most committed infraction and by parameter
* the amount of times it was committed.
*/
char * nextCount(parkingTicketsADT p, size_t * count);


/* To iterate over the lists of infractions ordered alphabetically, sets iterator on
* the first element of such list.
*/
void toBeginAlpha(parkingTicketsADT p);


/* Returns 1 if there are more elements on the lists of infractions ordered alphabetically to
* iterate over, 0 is there are not.
*/
int hasNextAlpha(parkingTicketsADT p);


/* Returns the description of the infraction, and by parameters the plate that committed this infraction
* the most and the amount of times it did.
*/
char * nextAlpha(parkingTicketsADT p, char ** maxPlate, size_t * infractionCount);


/*--------------------------------------------------------------------------------------------------------------------*/

enum {OK = 0, ERROR_MEM, ERROR_TOK, ERROR_ARG, ERROR_OPEN, NEXT_ERROR};

/*
 * ERROR_MEM = Memory error
 * ERROR_TOK = Token error
 * ERROR_ARG = Argument error
 * ERROR_OPEN = Error in opening file
 * NEXT_ERROR = No next in iterator
 */

#endif //TPE_PI_1Q2024_PARKINGTICKETSADT_H
