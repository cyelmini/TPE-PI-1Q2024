#include "prueba.h"
#include <errno.h>
#include <strings.h>
#include <string.h>

//query 1
typedef struct nodeInfCount{
    char description[MAX_DESC];
    size_t count;
    struct nodeInfCount * tail;
} TNodeInfCount;
typedef TNodeInfCount * TListInfCount;

//query2
typedef struct nodeAg{
    char agency[MAX_AG];
    char mostPInfraction[MAX_DESC];
    size_t infractionCount;
    struct nodeAg * tail;
}TNodeAg;
typedef TNodeAg * TListAg;

//query3
typedef struct nodeInfAlpha{
    char description[MAX_DESC];
    char plate[MAX_PLATE];
    size_t plateCount;
    struct nodeInfAlpha * tail;
}TNodeInfAlpha;
typedef TNodeInfAlpha * TListInfAlpha;

//Para cargar los datos
typedef struct nodePlate{
    char plate[MAX_PLATE];
    size_t count;
    struct nodePlate * tail;
}TNodePlate;
typedef TNodePlate * TListPlate;

typedef struct infraction{
    char description[MAX_DESC];
    char agency[MAX_AG];
    TListPlate firstPlate;
    char plate[MAX_PLATE];
    size_t maxPlateCount;
    size_t totalCount;
}TInfraction;

struct parkingTicketsCDT {
   TInfraction * infractions;  //cada posicion del vector se corresponde con un infractionId
   size_t size; // size del vector de infracciones
   size_t maxPosInfraction; //posicion de la infraccion que mas se cometio

    TListInfCount firstCount; //lista ordenada por cantidad de veces cometida
    TListInfCount iterCount;

   TListAg firstAgency;  //lista ordenada por agencia
   TListAg iterAg;

    TListInfAlpha firstAlpha; //lista ordenada alfabeticamente
    TListInfAlpha iterAlpha;
};

/* Generates a system to process the data of the different
* infractions committed
*/
parkingTicketsADT newParking(void){
    errno = OK;
    parkingTicketsADT aux = calloc(1, sizeof(struct parkingTicketsCDT));

    if (aux == NULL || errno == ENOMEM) {
        errno = ERROR_MEM;
        return NULL;
    }
    return aux;
}

/* Generates a reference for the inputted infraction id and
* the infraction description. Should be filled before inputting
* the tickets. Returns 1 if successfully done and 0 if not.
*/
int addInfractionDesc(parkingTicketsADT p, size_t infractionId, const char *description){
    if(infractionId >= p->size){
        p->infractions = realloc(p->infractions, (infractionId + 1)*sizeof(TInfraction));
        if(p->infractions == NULL || errno == ENOMEM){
            errno = ERROR_MEM;
            return 0;
        }
        for(size_t i = p->size; i <= infractionId; i++){
            p->infractions[i].description[0] = '\0';
            p->infractions[i].agency[0] = '\0';
            p->infractions[i].firstPlate = 0;
            p->infractions[i].plate[0] = '\0';
            p->infractions[i].maxPlateCount = 0;
            p->infractions[i].totalCount = 0;
        }
        p->size = infractionId + 1;
    }
    if(p->infractions[infractionId].description[0] == '\0'){
        strcpy(p->infractions[infractionId].description, description);
    }
    return 1;
}

static void updateInfraction(parkingTicketsADT p, size_t infractionId) {
    if (p->infractions[p->maxPosInfraction].totalCount < p->infractions[infractionId].totalCount) {
        p->maxPosInfraction = infractionId;
    }
    // If there is a tie, prioritize the infraction that comes first alphabetically
    if (p->infractions[p->maxPosInfraction].totalCount == p->infractions[infractionId].totalCount && strcasecmp(p->infractions[p->maxPosInfraction].description, p->infractions[infractionId].description) > 0) {
        p->maxPosInfraction = infractionId;
    }
}

static void updatePlate(parkingTicketsADT p, size_t infractionId, size_t newCount, const char *samePlate, const char *plate) {
    if(p->infractions[infractionId].maxPlateCount < newCount) {
        p->infractions[infractionId].maxPlateCount = newCount;
        strcpy(p->infractions[infractionId].plate, plate);
    }

    // If there is a tie, prioritize the plate that comes first alphabetically
    if (newCount == p->infractions[infractionId].maxPlateCount && samePlate != NULL && strcasecmp(samePlate, plate) > 0) {
        strcpy(p->infractions[infractionId].plate, plate);
    }
}

static TListPlate addPlateRec(TListPlate list, const char *plate, char **samePlate, size_t *newCount) {
    int c;
    if (list == NULL || (c = strcasecmp(list->plate, plate)) > 0) {
        TListPlate newPlate = malloc(sizeof(struct nodePlate));
        if (newPlate == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newPlate->plate, plate);
        newPlate->count = 1;
        *newCount = newPlate->count;
        newPlate->tail = list;
        return newPlate;
    }
    else if(c == 0) {
        (list->count)++;
        *newCount = list->count;
        *samePlate = list->plate;
        return list;
    }
    else {
        return addPlateRec(list->tail, plate, samePlate, newCount);
    }
}

/* Adds an infraction committed by a given plate to the correspondent agency that emitted it.
 * Returns 1 if successfully done and 0 if not.
 */
int addTicket(parkingTicketsADT p, const char *agency, size_t infractionId, const char *plate){
    if(agency == NULL || plate ==  NULL){
        errno = ERROR_ARG;
        return 0;
    }

    if(infractionId >= p->size || p->infractions[infractionId].description[0] == '\0'){
        return 1;  // If one infraction is present in the infractions.csv but not in the tickets.csv, we skip it and don't do anything
    }

    //Copy agency that emitted it
    if(p->infractions[infractionId].agency[0] == '\0'){
        strcpy(p->infractions[infractionId].agency, agency);
    }

    //Add plate that committed it
    size_t newCount = 0;
    char * samePlate = NULL;
    p->infractions[infractionId].firstPlate = addPlateRec(p->infractions[infractionId].firstPlate, plate, &samePlate, &newCount);

    //Update the plate that committed the infraction the most
    updatePlate(p, infractionId, newCount, samePlate, plate);

    //Update the most committed infraction
    updateInfraction(p, infractionId);

    p->infractions[infractionId].totalCount++;
    return 1;
}

static TListInfCount sortByCountRec(TListInfCount list, const char *description, size_t count){
    if(list == NULL || (list->count < count)){
        TListInfCount newInfCount = malloc(sizeof(TNodeInfCount));
        if (newInfCount == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newInfCount->description, description);
        newInfCount->count = count;
        newInfCount->tail = list;
        return newInfCount;
    }
    if(list->count == count){
        if(strcasecmp(list->description, description) > 0){   //Should copy the first description in alphabetical order
            strcpy(list->description, description);
        }
        return list;
    }
    list->tail = sortByCountRec(list->tail, description, count);
    return list;
}

static void sortListCount(parkingTicketsADT p) {
    for(size_t i = 0; i < p->size; i++){
        if(p->infractions[i].description[0] != '\0'){
            p->firstCount = sortByCountRec(p->firstCount, p->infractions[i].description, p->infractions[i].totalCount);
        }
    }
}

/* Donde cada línea de la salida contenga, separados por “;” el nombre de la agencia emisora, la infracción más popular de esa agencia emisora y la cantidad de multas correspondiente.
La infracción más popular de una agencia emisora es la que tiene la mayor cantidad de multas. En caso de que existan dos o más infracciones con la misma cantidad de multas para la misma agencia emisora considerar la menor infracción en orden alfabético.
La información debe listarse ordenada en forma alfabética por agencia emisora.
*/
static TListAg sortByAgencyRec(TListAg list, const char *agency, const char *description, size_t count) {
    int c;
    if (list == NULL || (c = strcasecmp(list->agency, agency)) > 0) {
        TListAg newAg = malloc(sizeof(TNodeAg));
        if (newAg == NULL) {
            errno = ERROR_MEM;
            return list;
        }
        // Initialize the new node
        strcpy(newAg->agency, agency);
        strcpy(newAg->mostPInfraction, description);
        newAg->infractionCount = count;
        newAg->tail = list; // Link the new node to the current list
        return newAg;
    }
    // If the agency already exists, update the infraction if necessary
    if (c == 0) {
        if (list->infractionCount == count) { // Two infractions have the same count, need to break the tie
            if (strcasecmp(list->mostPInfraction, description) > 0) {
                strcpy(list->mostPInfraction, description);
                list->infractionCount = count;
            }
        } else if (list->infractionCount < count) {
            strcpy(list->mostPInfraction, description);
            list->infractionCount = count;
        }
        return list;
    }
    list->tail = sortByAgencyRec(list->tail, agency, description, count);
    return list;
}

static void sortAgency(parkingTicketsADT p) {
    for (size_t i = 0; i < p->size; i++) {
        if(p->infractions[i].agency[0] != '\0'&& p->infractions[i].description[0] != '\0'){
            p->firstAgency = sortByAgencyRec(p->firstAgency, p->infractions[i].agency, p->infractions[i].description, p->infractions[i].totalCount);
        }
    }
}

static TListInfAlpha sortAlphaRec(TListInfAlpha list, const char *description, const char *plate, size_t plateCount){
    int c;
    if (list == NULL || (c = strcasecmp(list->description, description)) > 0) {
        TListInfAlpha newInfAlpha = malloc(sizeof(TNodeInfAlpha));
        if (newInfAlpha == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newInfAlpha->description, description);
        strcpy(newInfAlpha->plate, plate);
        newInfAlpha->plateCount = plateCount;
        newInfAlpha->tail = list;
        return newInfAlpha;
    }
    if (c == 0) {
        return list;
    }
    list->tail = sortAlphaRec(list->tail, description, plate, plateCount);
    return list;
}

static void sortListAlpha(parkingTicketsADT p){
    for(size_t i = 0; i < p->size; i++){
        if(p->infractions[i].description[0] != '\0' && p->infractions[i].plate[0] != '\0') {
            p->firstAlpha = sortAlphaRec(p->firstAlpha, p->infractions[i].description, p->infractions[i].plate, p->infractions[i].maxPlateCount);
        }
    }
}

static void freeAlpha(TListInfAlpha list){
    if(list == NULL){
        return;
    }
    TListInfAlpha aux = list->tail;
    free(list);
    freeAlpha(aux);
}

static void freeAgency(TListAg list){
    if(list == NULL){
        return;
    }
    TListAg aux = list->tail;
    free(list);
    freeAgency(aux);
}

static void freeCount(TListInfCount list){
    if(list == NULL){
        return;
    }
    TListInfCount aux = list->tail;
    free(list);
    freeCount(aux);
}

static void freePlate(TListPlate list){
    if(list == NULL){
        return;
    }
    TListPlate aux = list->tail;
    free(list);
    freePlate(aux);
}

static void freeInfractions(TInfraction * infractions, size_t size){
    for(size_t i = 0; i < size; i++){
        freePlate(infractions[i].firstPlate);
    }
}

/* Frees the memory reserved by the ADT */
void freeParkingTickets(parkingTicketsADT p){
    freeInfractions(p->infractions, p->size);
    free(p->infractions);
    freeCount(p->firstCount);
    freeAgency(p->firstAgency);
    freeAlpha(p->firstAlpha);
    free(p);
}

/* ----------------- Functions to iterate over the three main lists meant to resolve the queries----------------------*/

/* To iterate over the lists of agencies, sets iterator on
* the first element of such list.
*/
void toBeginAg(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
        return;
    }
    sortAgency(p);
    p->iterAg = p->firstAgency;
}

/* Returns 1 if there are more elements on the lists of agencies to
* iterate over, 0 is there are not.
*/
int hasNextAg(parkingTicketsADT p){
    return p->iterAg != NULL;
}

/* Returns the name of the agency that emitted the infraction, and by parameters the description
* of the most popular infraction and the amount of times it was committed.
*/
char * nextAg(parkingTicketsADT p, char ** mostPopularInf, size_t * infractionCount){
    if(hasNextAg(p)){
        char * ans = p->iterAg->agency;
        *mostPopularInf = p->iterAg->mostPInfraction;
        *infractionCount = p->iterAg->infractionCount;
        p->iterAg = p->iterAg->tail;
        return ans;
    }
    return NULL;
}

/* To iterate over the lists of infractions ordered by count, sets iterator on
* the first element of such list.
*/
void toBeginCount(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
        return;
    }
    sortListCount(p);
    p->iterCount = p->firstCount;
}

/* Returns 1 if there are more elements on the lists of infractions ordered by count to
* iterate over, 0 is there are not.
*/
int hasNextCount(parkingTicketsADT p){
    return p->iterCount != NULL;
}

/* Returns the description of the most committed infraction and by parameter
* the amount of times it was committed.
*/
char * nextCount(parkingTicketsADT p, size_t * count){
    if(hasNextCount(p)){
        char * ans = p->iterCount->description;
        *count = p->iterCount->count;
        p->iterCount = p->iterCount->tail;
        return ans;
    }
    return NULL;
}

/* To iterate over the lists of infractions ordered alphabetically, sets iterator on
* the first element of such list.
*/
void toBeginAlpha(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
        return;
    }
    sortListAlpha(p);
    p->iterAlpha = p->firstAlpha;
}

/* Returns 1 if there are more elements on the lists of infractions ordered alphabetically to
* iterate over, 0 is there are not.
*/
int hasNextAlpha(parkingTicketsADT p){
    return p->iterAlpha != NULL;
}

/* Returns the description of the infraction, and by parameters the plate that committed this infraction
* the most and the amount of times it did.
*/
char * nextAlpha(parkingTicketsADT p, char ** maxPlate, size_t * infractionCount){
    if(hasNextAlpha(p)){
        char * ans = p->iterAlpha->description;
        *maxPlate = p->iterAlpha->plate;
        *infractionCount = p->iterAlpha->plateCount;
        p->iterAlpha = p->iterAlpha->tail;
        return ans;
    }
    return NULL;
}
