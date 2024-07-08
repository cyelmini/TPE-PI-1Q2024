#include "parkingTicketsADT.h"
#include <errno.h>
#include <strings.h>

typedef struct nodePlate{
    char plate[MAX_PLATE];
    size_t count;
    struct nodePlate * tail;
}TNodePlate;
typedef TNodePlate * TListPlate;

typedef struct infraction{
    char description[MAX_DESC];
    TListPlate firstPlate; // Pointer to the first element of the list containing the different plates that committed the infraction
    char plate[MAX_PLATE]; // Plate that committed the infraction the most
    size_t maxPlateCount;  // Amount of times the plate committed the infraction
    size_t totalCount;     // Total amount of times the infraction was committed
}TInfraction;

//To be used in query2
typedef struct nodeAg{
    char agency[MAX_AG];
    TInfraction * infractions; // Vector containing the different infractions issued by the correspondent agency (each position in this vector corresponds with the infractionId)
    size_t size; // Reserved space for the infractions vector
    size_t maxPosInfraction; // Position where the most repeated infraction is stored in the vector
    struct nodeAg * tail;  // Pointer to the next agency
} TNodeAg;
typedef TNodeAg * TListAg;

//To be used in query1
typedef struct nodeInfCount{
    char description[MAX_DESC];
    size_t count;  // Total amount of infractions committed
    struct nodeInfCount * tail;
} TNodeInfCount;
typedef TNodeInfCount * TListInfCount;

// To be used in query3
typedef struct nodeInfAlpha{
    char description[MAX_DESC];
    char plate[MAX_PLATE]; //Plate that committed the infraction the most
    size_t plateCount; //Amount of times it committed it
    struct nodeInfAlpha * tail;
}TNodeInfAlpha;
typedef TNodeInfAlpha * TListInfAlpha;

struct parkingTicketsCDT{
    TListAg firstAgency;  //Pointer to the first element of the list of agencies, which is added in alphabetical order
    TListAg iterAg;
    TListInfCount firstCount; //Pointer to the first element of the list of infractions ordered by infraction count
    TListInfCount iterCount;
    TListInfAlpha firstAlpha; //Pointer to the first element of the list of infractions ordered alphabetically
    TListInfAlpha iterAlpha;
    char **idReference;   //Vector where the position corresponds with the infractionId
    size_t dimIdReference; //Dimension of the vector
};

parkingTicketsADT newParking(void) {
    errno = OK;
    parkingTicketsADT aux = calloc(1, sizeof(struct parkingTicketsCDT));

    if (aux == NULL || errno == ENOMEM) {
        errno = ERROR_MEM;
        return NULL;
    }
    return aux;
}

int addInfraction(parkingTicketsADT p, size_t infractionId, const char* description) {
    if(infractionId >= p->dimIdReference) {
        char ** temp = realloc(p->idReference, (infractionId + 1) * sizeof(char *));
        if (temp == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return 0;
        }
        p->idReference = temp;
        for (size_t i = p->dimIdReference; i <= infractionId; i++) {
            p->idReference[i] = NULL;
        }
        p->dimIdReference = infractionId + 1;
    }
    if (p->idReference[infractionId] == NULL) {
        p->idReference[infractionId] = malloc(MAX_DESC*sizeof(char));
        if(p->idReference[infractionId] == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return 0;
        }
        strcpy(p->idReference[infractionId], description);
    }
    return 1;
}

static TListPlate addPlateRec(TListPlate list, const char *plate, size_t *newCount, const char **samePlate){
    errno = OK;
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
    if (c == 0) {
        list->count++;
        *newCount = list->count;
        *samePlate = list->plate;
        return list;
    }
    return addPlateRec(list->tail, plate, newCount, samePlate);
}

static void updatePlate(TListAg list, size_t infractionId, size_t newCount, const char *samePlate, const char *plate) {
    if (list->infractions[infractionId].maxPlateCount < newCount) {
        list->infractions[infractionId].maxPlateCount = newCount;
        strcpy(list->infractions[infractionId].plate, plate);
    }

    // If there is a tie, prioritize the plate that comes first alphabetically
    if (newCount == list->infractions[infractionId].maxPlateCount && samePlate != NULL && strcasecmp(samePlate, plate) > 0) {
        strcpy(list->infractions[infractionId].plate, plate);
    }
}

static void updateInfraction(TListAg list, size_t infractionId) {
    if (list->infractions[list->maxPosInfraction].totalCount < list->infractions[infractionId].totalCount) {
        list->maxPosInfraction = infractionId;
    }
    // If there is a tie, prioritize the infraction that comes first alphabetically
    if (list->infractions[list->maxPosInfraction].totalCount == list->infractions[infractionId].totalCount && strcasecmp(list->infractions[list->maxPosInfraction].description, list->infractions[infractionId].description) > 0) {
        list->maxPosInfraction = infractionId;
    }
}

static void addTicketAux(TListAg list, const char *infractionDesc, size_t infractionId, const char *plate) {
    size_t newSize = infractionId + 1;

    // If the required size is greater than the current size, resize the infraction vector
    if (newSize > list->size) {
        TInfraction *temp = realloc(list->infractions, newSize * sizeof(TInfraction));
        if (temp == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return;
        }
        list->infractions = temp;
        // Initialize the new positions in the vector
        for (size_t i = list->size; i < newSize; i++) {
            list->infractions[i].description[0] = '\0';
            list->infractions[i].firstPlate = NULL;
            list->infractions[i].plate[0] = '\0';
            list->infractions[i].maxPlateCount = 0;
            list->infractions[i].totalCount = 0;
        }
        list->size = newSize;
    }

    // If it's the first time the infraction is committed, copy the description
    if (list->infractions[infractionId].totalCount == 0) {
        strcpy(list->infractions[infractionId].description, infractionDesc);
    }
    // Register a new occurrence of the infraction
    list->infractions[infractionId].totalCount++;

    // Add the plate that committed the infraction
    size_t newCount = 0;
    const char *samePlate = NULL;
    list->infractions[infractionId].firstPlate = addPlateRec(list->infractions[infractionId].firstPlate, plate, &newCount, &samePlate);

    // Update the plate that committed the infraction the most
    updatePlate(list, infractionId, newCount, samePlate, plate);

    // Update the most popular infraction by agency
    updateInfraction(list, infractionId);
}

static TListAg addTicketRec(TListAg list, const char *agency, const char *infractionDesc, size_t infractionId, const char *plate, int *flag) {
    int c;
    if (list == NULL || (c = strcasecmp(list->agency, agency)) > 0) { // Agency does not exist
        TListAg newAg = calloc(1, sizeof(TNodeAg));
        if(newAg == NULL || errno == ENOMEM) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newAg->agency, agency);
        addTicketAux(newAg, infractionDesc, infractionId, plate);
        newAg->tail = list;
        *flag = 1;
        return newAg;
    }
    if(c == 0) {
        addTicketAux(list, infractionDesc, infractionId, plate);
        *flag = 1;
        return list;
    }
    list->tail = addTicketRec(list->tail, agency, infractionDesc, infractionId, plate, flag);
    return list;
}

int addTicket(parkingTicketsADT p, const char *agency, size_t infractionId, const char *plate) {
    int flag = 0;
    if(agency == NULL || plate ==  NULL){
        errno = ERROR_ARG;
        return flag;
    }
    p->firstAgency = addTicketRec(p->firstAgency, agency, p->idReference[infractionId], infractionId, plate, &flag);
    return flag;
}

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
        if(strcasecmp(list->description, infractionDesc) > 0){   //should copy the first description in alphabetical order
            strcpy(list->description, infractionDesc);
        }
        return list;
    }
    list->tail = sortByCountRec(list->tail, infractionDesc, count);
    return list;
}

static TListInfAlpha sortAlphaRec(TListInfAlpha list, const char *infractionDesc, size_t count, const char *plate) {
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
        newInfAlpha->tail = list;
        return newInfAlpha;
    }
    if (c == 0) {
        return list;
    }
    list->tail = sortAlphaRec(list->tail, infractionDesc, count, plate);
    return list;
}

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

static void sortListAlpha(parkingTicketsADT p){
    TListAg aux = p->firstAgency;
    while (aux != NULL) {
        for(size_t i = 0; i < aux->size; i++) {
            if(aux->infractions[i].description[0] != '\0'){
                p->firstAlpha = sortAlphaRec(p->firstAlpha, aux->infractions[i].description, aux->infractions[i].maxPlateCount, aux->infractions[i].plate);
            }
        }
        aux = aux->tail;
    }
}

void toBeginAg(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    p->iterAg = p->firstAgency;
}

int hasNextAg(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    return p->iterAg != NULL;
}

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

void toBeginCount(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    sortListCount(p);
    p->iterCount = p->firstCount;
}

int hasNextCount(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    return p->iterCount != NULL;
}

char * nextCount(parkingTicketsADT p, size_t * count){
    if(hasNextCount(p)){
        char * ans = p->iterCount->description;
        *count = p->iterCount->count;
        p->iterCount = p->iterCount->tail;
        return ans;
    }
    return NULL;
}

void toBeginAlpha(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    sortListAlpha(p);
    p->iterAlpha = p->firstAlpha;
}

int hasNextAlpha(parkingTicketsADT p){
    if(p == NULL){
        errno = ERROR_ARG;
    }
    return p->iterAlpha != NULL;
}

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

static void freeIdReference(char ** idReference, size_t dimIdReference){
    for(size_t i = 0; i < dimIdReference; i++){
        if(idReference[i] != NULL){
            free(idReference[i]);
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
    freeIdReference(p->idReference, p->dimIdReference);
    free(p->idReference);
    free(p);
}
