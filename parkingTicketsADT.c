#include "parkingTicketsADT.h"
#include <errno.h>
#include <strings.h>
#include <string.h>

/* List of infractions that contains their descriptions and
 * the total amount of fines (to be used in query 1)
 */
typedef struct nodeInfCount{
    char description[MAX_DESC];
    size_t count;
    struct nodeInfCount * tail;
} TNodeInfCount;
typedef TNodeInfCount * TListInfCount;


/* List of infractions that contains its description, the plate that committed this infraction the most and the amount
 * of times it did (to be used in query 3)
 */

typedef struct nodeInfAlpha{
    char description[MAX_DESC];
    char plate[MAX_PLATE];
    size_t plateCount;
    size_t minAmount;
    size_t maxAmount;
    size_t diff; // esto se podria calcular al imprimirlo pero lo guardo para separar backend
    struct nodeInfAlpha * tail;
}TNodeInfAlpha;
typedef TNodeInfAlpha * TListInfAlpha;


/* List of plates that committed a certain infraction */
typedef struct nodePlate{
    char plate[MAX_PLATE];
    size_t count;
    struct nodePlate * tail;
}TNodePlate;
typedef TNodePlate * TListPlate;


/* Infractions is a vector where each position corresponds with an infractionId. Each element of this vector has the
 * infraction description, the first element of the list of plates that committed this infraction. It also stores
 * the plate that committed the infraction the most and the amount of times it did.
 */

typedef struct infraction{
    char description[MAX_DESC];
    TListPlate firstPlate;
    char plate[MAX_PLATE];
    size_t maxPlateCount;
    size_t totalCount;

    size_t minAmount;//se actualizan en el addTicketToAgency
    size_t maxAmount;
}TInfraction;

/* List of agencies containing their names, the infractions emitted by them and the position where the most emitted
 * infraction is stored (to be used in query2)
 */
typedef struct nodeAg{
    char agency[MAX_AG];
    TInfraction * infractions;
    size_t size;
    size_t maxPosInfraction;
    struct nodeAg * tail;
} TNodeAg;
typedef TNodeAg * TListAg;


/* -------------------------------------------------  parkingTicketsCDT -------------------------------------------- */

struct parkingTicketsCDT{
    TListAg firstAgency;  //First element of the list of agencies (ordered alphabetically by agency)
    TListAg iterAg;

    TListInfCount firstCount; //First element of the list of infractions (ordered by infraction count)
    TListInfCount iterCount;

    TListInfAlpha firstAlpha; //First element of the list of plates with the most infractions (ordered alphabetically).
    TListInfAlpha iterAlpha;

    char ** descriptions;
    size_t size;
};

/* ------------------------------------------------------------------------------------------------------------------ */

parkingTicketsADT newParking(void) {
    errno = OK;
    parkingTicketsADT aux = calloc(1, sizeof(struct parkingTicketsCDT));

    if (aux == NULL || errno == ENOMEM) {
        errno = ERROR_MEM;
        return NULL;
    }
    return aux;
}

/* ------------------------------------------ Add functions --------------------------------------------------------- */

/* Generates a reference for the inputted infraction id and the infraction description. Should be filled before
 * inputting the tickets. Returns 1 if successfully done and 0 if not.
 */
int addInfraction(parkingTicketsADT p, size_t infractionId, const char *description) {
    if(infractionId >= p->size) {
        char ** temp = realloc(p->descriptions, (infractionId + 1) * sizeof(char *));
        if (temp == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return 0;
        }
        p->descriptions = temp;
        for (size_t i = p->size; i <= infractionId; i++) {
            p->descriptions[i] = NULL;
        }
        p->size = infractionId + 1;
    }
    if (p->descriptions[infractionId] == NULL) {
        p->descriptions[infractionId] = malloc((strlen(description) + 1)*sizeof(char));
        if(p->descriptions[infractionId] == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return 0;
        }
        strcpy(p->descriptions[infractionId], description);
    }
    return 1;
}

static TListPlate addPlateRec(TListPlate list, const char *plate, size_t *newCount){
    /*
    int c;
    if (list == NULL || (c = strcasecmp(list->plate, plate)) > 0) {
        TListPlate newPlate = malloc(sizeof(struct nodePlate));
        if (newPlate == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newPlate->plate, plate);
        newPlate->count = 1;
        *newCount = 1;
        newPlate->tail = list;
        return newPlate;
    }
    if (c == 0) {
        list->count++;
        *newCount = list->count;
        return list;
    }
    list->tail = addPlateRec(list->tail, plate, newCount);
     */
    return list;
}

static void updatePlate(TListAg list, size_t infractionId, size_t newCount, const char *plate) {

    // If there is a tie, we prioritize the plate that comes first alphabetically

    if(list->infractions[infractionId].maxPlateCount < newCount || (newCount == list->infractions[infractionId].maxPlateCount
            && strcasecmp(list->infractions[infractionId].plate, plate) > 0)) {

        list->infractions[infractionId].maxPlateCount = newCount;
        strcpy(list->infractions[infractionId].plate, plate);
    }
}

static void updateInfraction(TListAg list, size_t infractionId) {

    // If there is a tie, prioritize the infraction that comes first alphabetically

    if (list->infractions[list->maxPosInfraction].totalCount < list->infractions[infractionId].totalCount ||
    (list->infractions[list->maxPosInfraction].totalCount == list->infractions[infractionId].totalCount &&
    strcasecmp(list->infractions[list->maxPosInfraction].description, list->infractions[infractionId].description) > 0) ) {

        list->maxPosInfraction = infractionId;
    }
}

/* Adds a new ticket to the vector of infractions in the correspondent agency. Also adds the plate that committed it
 * by calling upon addPlateRec. If needed, updates the plate that committed the infraction the most and the most
 * popular infraction by agency.
 */
static void addTicketToAgency(TListAg list, const char *infractionDesc, size_t infractionId, const char *plate, size_t amount) {

    if(infractionId >= list->size) {
        TInfraction *temp = realloc(list->infractions, (infractionId + 1)*sizeof(TInfraction));
        if (temp == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return;
        }
        list->infractions = temp;
        for (size_t i = list->size; i <= infractionId; i++) {
            list->infractions[i].description[0] = '\0';
            list->infractions[i].firstPlate = NULL;
            list->infractions[i].plate[0] = '\0';
            list->infractions[i].maxPlateCount = 0;
            list->infractions[i].totalCount = 0;
        }
        list->size = infractionId + 1;
    }

    // If it is the first time the infraction is committed, copy the description
    if (list->infractions[infractionId].totalCount == 0) {
        strcpy(list->infractions[infractionId].description, infractionDesc);
    }
    list->infractions[infractionId].totalCount++;

    // Add the plate that committed the infraction
    size_t newCount = 0;
    list->infractions[infractionId].firstPlate = addPlateRec(list->infractions[infractionId].firstPlate, plate, &newCount);

    // Update the plate that committed the infraction the most
    updatePlate(list, infractionId, newCount, plate);

    // Update the most popular infraction by agency
    updateInfraction(list, infractionId);

    // Check minAmount and maxAmount ////////// para query5 ///////////////////////////////////////////
    if(list->infractions[infractionId].minAmount>amount){
        list->infractions[infractionId].minAmount=amount;
    }
    
    if(list->infractions[infractionId].maxAmount<amount){
        list->infractions[infractionId].maxAmount=amount;
    }
}

/* Searches for the given agency in the list of agencies. If it does not found it, it adds it to the list. In either
 * case, it registers the new ticket calling upon addTicketToAgency.
 */
static TListAg addTicketRec(TListAg list, const char *agency, const char *infractionDesc, size_t infractionId, const char *plate, int *flag, size_t amount) {
    int c;
    if (list == NULL || (c = strcasecmp(list->agency, agency)) > 0) { // Agency does not exist
        TListAg newAg = calloc(1, sizeof(TNodeAg));
        if(newAg == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newAg->agency, agency);
        addTicketToAgency(newAg, infractionDesc, infractionId, plate, amount);
        newAg->tail = list;
        *flag = 1;
        return newAg;
    }
    if(c == 0) {
        addTicketToAgency(list, infractionDesc, infractionId, plate, amount);
        *flag = 1;
        return list;
    }
    list->tail = addTicketRec(list->tail, agency, infractionDesc, infractionId, plate, flag, amount);
    return list;
}

/* Adds an infraction committed by a given plate to the correspondent agency that emitted it.
* If the agency did not exist, it adds it to the list as well. Returns 1 if successfully done
* and 0 if not.
*/
int addTicket(parkingTicketsADT p, const char *agency, size_t infractionId, const char *plate, size_t amount) {
    int flag = 0;
    if(agency == NULL || plate ==  NULL){
        errno = ERROR_ARG;
        return flag;
    }
    if(infractionId >= p->size || p->descriptions[infractionId] == NULL){
        flag = 1;
        return flag;  // If one infraction is present in the infractions.csv but not in the tickets.csv, we skip it and don't do anything
    }
    p->firstAgency = addTicketRec(p->firstAgency, agency, p->descriptions[infractionId], infractionId, plate, &flag, amount);
    return flag;
}

/* ------------------------------------------ Sort functions --------------------------------------------------------*/

static TListInfCount sortByCountRec(TListInfCount list, const char * infractionDesc, size_t count){
    if(list == NULL || (list->count < count)){
        TListInfCount newInfCount = malloc(sizeof(TNodeInfCount));
        if (newInfCount == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newInfCount->description, infractionDesc);
        newInfCount->count = count;
        newInfCount->tail = list;
        return newInfCount;
    }
    if(list->count == count){
        if(strcasecmp(list->description, infractionDesc) > 0){   //Should copy the first description in alphabetical order
            strcpy(list->description, infractionDesc);
        }
        return list;
    }
    list->tail = sortByCountRec(list->tail, infractionDesc, count);
    return list;
}

/* Generates a sorted list of infractions in descending order */
static void sortListCount(parkingTicketsADT p) {
    TListAg aux = p->firstAgency;
    while (aux != NULL) {
        for (size_t i = 0; i < aux->size; i++) {
            if(aux->infractions[i].description[0] != '\0'){
                p->firstCount = sortByCountRec(p->firstCount, aux->infractions[i].description, aux->infractions[i].totalCount);
            }
        }
        aux = aux->tail;
    }
}
static TListInfAlpha sortAlphaRec(TListInfAlpha list, const char *infractionDesc, size_t count, const char *plate,size_t minAmount, size_t maxAmount) {
    int c;
    if (list == NULL || (c = strcasecmp(list->description, infractionDesc)) > 0) {
        TListInfAlpha newInfAlpha = malloc(sizeof(TNodeInfAlpha));
        if (newInfAlpha == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newInfAlpha->description, infractionDesc);
        strcpy(newInfAlpha->plate, plate);
        newInfAlpha->plateCount = count;
        
        newInfAlpha->minAmount = minAmount;
        newInfAlpha->maxAmount = maxAmount;       
        newInfAlpha->diff = (maxAmount-minAmount);

        newInfAlpha->tail = list;
        return newInfAlpha;
    }
    if (c == 0) {
        return list;
    }
    list->tail = sortAlphaRec(list->tail, infractionDesc, count, plate);
    return list;
}

/* Generates a sorted list of plates with the most fines per
 * infraction, ordered alphabetically by infraction
 */
static void sortListAlpha(parkingTicketsADT p){
    TListAg aux = p->firstAgency;
    while (aux != NULL) {
        for(size_t i = 0; i < aux->size; i++) {
            if(aux->infractions[i].description[0] != '\0'){
                p->firstAlpha = sortAlphaRec(p->firstAlpha, aux->infractions[i].description, aux->infractions[i].maxPlateCount, aux->infractions[i].plate, aux->infractions[i].minAmount, aux->infractions[i].maxAmount);
            }
        }
        aux = aux->tail;
    }
}


/* -------------------------------------------------  Iterators -----------------------------------------------------*/


/* To iterate over the lists of agencies, sets iterator on the first element of such list. */
void toBeginAg(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    p->iterAg = p->firstAgency;
}

/* Returns 1 if there are more elements on the lists of agencies to iterate over, 0 is there are not. */
int hasNextAg(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    return p->iterAg != NULL;
}

/* Returns the name of the agency that emitted the infraction, and by parameters the description
* of the most popular infraction and the amount of times it was committed.
*/
char * nextAg(parkingTicketsADT p, char ** mostPopularInf, size_t * infractionCount){
    if (hasNextAg(p)) {
        char * ans = p->iterAg->agency;
        *mostPopularInf = p->iterAg->infractions[p->iterAg->maxPosInfraction].description;
        *infractionCount = p->iterAg->infractions[p->iterAg->maxPosInfraction].totalCount;
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
    }
    sortListCount(p);
    p->iterCount = p->firstCount;
}

/* To iterate over the lists of infractions ordered by count, sets iterator on
* the first element of such list.
*/
int hasNextCount(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    return p->iterCount != NULL;
}

/* To iterate over the lists of infractions ordered by count, sets iterator on
* the first element of such list.
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
    }
    sortListAlpha(p);
    p->iterAlpha = p->firstAlpha;
}

/* Returns 1 if there are more elements on the lists of infractions ordered alphabetically to
* iterate over, 0 is there are not.
*/
int hasNextAlpha(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
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


/* --------------------------------------------------- Frees ------------------------------------------------------- */

static void freeDescriptions(char ** descriptions, size_t size){
    for(size_t i = 0; i < size; i++){
        if(descriptions[i] != NULL){
            free(descriptions[i]);
        }
    }
}

static void freeListAlpha(TListInfAlpha list){
    if(list == NULL){
        return;
    }
    TListInfAlpha aux = list->tail;
    free(list);
    freeListAlpha(aux);
}

static void freeListCount(TListInfCount list){
    if(list == NULL){
        return;
    }
    TListInfCount aux = list->tail;
    free(list);
    freeListCount(aux);
}

static void freePlate(TListPlate list){
    if(list == NULL){
        return;
    }
    TListPlate aux = list->tail;
    free(list);
    freePlate(aux);
}

static void freeInfraction(TInfraction * infraction, size_t size){
    for(size_t i = 0; i < size; i++){
        freePlate(infraction[i].firstPlate);
    }
}

static void freeAgency(TListAg list){
    if(list == NULL){
        return;
    }
    freeInfraction(list->infractions, list->size);
    free(list->infractions);
    TListAg aux = list->tail;
    free(list);
    freeAgency(aux);
}

void freeParkingTickets(parkingTicketsADT p){
    freeAgency(p->firstAgency);
    freeListCount(p->firstCount);
    freeListAlpha(p->firstAlpha);
    freeDescriptions(p->descriptions, p->size);
    free(p->descriptions);
    free(p);
}



////////////////////////////////////////////////////cosas para query 5 no fucinan
////////////////////// PRUEBA q5 /////////////////////////

// void query5(parkingTicketsADT p){
//     FILE * query5File = fopen("query5.csv", "wt");
//     htmlTable table5 = newTable("query5.html", 4, "infraction", "minAmount", "maAmount","diffAmount");

//     if(query5File == NULL || table5 == NULL){
//         fprintf(stderr, "Error in file generation\n");
//         exit(ERROR_OPEN);
//     }

//     fputs("infraction;minAmont;maxAmount;diffAmount\n", query5File);

//     toBeginAlphaQ5(p);
//     if(errno != OK){
//         fprintf(stderr, "Error in toBeginAlpha\n");
//         exit(errno);
//     }

//     char count[MAX_COUNT];
//     while(hasNextAlphaQ5(p)){
//         if(errno != OK){
//             fprintf(stderr, "Error in hasNextAlpha\n");
//             exit(errno);
//         }
//         size_t minAmont;
//         size_t maxAmount;
//         size_t diffAmount;
//         char * infractionName = nextAlphaQ5(p, &minAmont, &maxAmount, &diffAmount);

//         if(infractionName != NULL) {
//             fprintf(query5File, "%s;%ld;%ld;%ld\n", infractionName, minAmont, maxAmount, diffAmount);
//             addHTMLRow(table5, infractionName, minAmont, minAmont, maxAmount, diffAmount);
//         }
//     }
//     fclose(query5File);
//     closeHTMLTable(table5);
// }
// void toBeginAlphaQ5(parkingTicketsADT p){
//     if(p == NULL){
//         errno = ERROR_ARG;
//     }
//     // sortListAlpha(p);
//     p->iterAlpha = p->firstAlpha;
// }
// int hasNextAlphaQ5(parkingTicketsADT p){
//     if(p == NULL){
//         errno = ERROR_ARG;
//     }
//     return p->iterAlpha != NULL;
// }
// char * nextAlphaQ5(parkingTicketsADT p, size_t* minAmont, size_t* maxAmount, size_t* diffAmount){
//     if(hasNextAlphaQ5(p)){
//         char * ans = p->iterAlpha->description;
//         *minAmont=p->iterAlpha->minAmont;
//         *maxAmount=p->iterAlpha->maxAmount;
//         *diffAmount=p->iterAlpha->diff;
//         p->iterAlpha = p->iterAlpha->tail;
//         return ans;
//     }
//     return NULL;
// }