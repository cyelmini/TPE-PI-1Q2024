#include "parkingTicketsADT.h"
#include <errno.h>
#include <strings.h>
#include <stdio.h>

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

//To be used in query1
typedef struct nodeInfCount{
    char description[MAX_DESC];
    size_t count;  // Total amount of infractions committed
    struct nodeInfCount * tail;
} TNodeInfCount;
typedef TNodeInfCount * TListInfCount;

//To be used in query2
typedef struct nodeAg{
    char agency[MAX_AG];
    TInfraction * infractions; // Vector containing the different infractions issued by the correspondent agency (each position in this vector corresponds with the infractionId)
    size_t size; // Reserved space for the infractions vector
    size_t totalCount; // Total amount of infractions
    size_t maxPosInfraction; // Position where the most repeated infraction is stored in the vector
    struct nodeAg * tail;  // Pointer to the next agency
} TNodeAg;
typedef TNodeAg * TListAg;

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
    char** idReference;   //Vector where the position corresponds with the infractionId
    size_t dimIdReference; //Dimension of the vector
};

parkingTicketsADT newParking(void) {
    errno = OK;
    parkingTicketsADT aux = calloc(1, sizeof(struct parkingTicketsCDT));

    if (aux == NULL) {
        errno = ERROR_MEM;
        return NULL;
    }
    aux->dimIdReference=0;
    aux->idReference=NULL;
    return aux;
}

int addInfraction(parkingTicketsADT p, size_t infractionId, const char* description) {
    int ret = 1;
    if (infractionId >= p->dimIdReference) {
        char ** temp = realloc(p->idReference, (infractionId + 1) * sizeof(char *));
        if (temp == NULL) {
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
        if(p->idReference[infractionId] == NULL) {
            errno = ERROR_MEM;
            return 0;
        }
        strcpy(p->idReference[infractionId], description);
    }
    return ret;
}

static TListPlate addPlateRec(TListPlate list, const char *plate, size_t * newCount) {
    int c;
    if (list == NULL || (c = strcasecmp(list->plate, plate)) > 0) { // Plate has not yet been added
        TListPlate newPlate = malloc(sizeof(TNodePlate));
        if (newPlate == NULL) {
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
        return list;
    }
    list->tail = addPlateRec(list->tail, plate, newCount);
    return list;
}

static void addTicketAux(TListAg list, const char *infractionDesc, size_t infractionId, const char *plate) {
    size_t newSize = infractionId + 1;
    if (newSize > list->size) {
        TInfraction * temp = realloc(list->infractions, newSize * sizeof(TInfraction));
        if(temp == NULL) {
            errno = ERROR_MEM;
            return;
        }
        list->infractions = temp;
        for (size_t i = list->size; i < newSize; i++) {  //Initialize the new reserved spaces on the vector
            list->infractions[i].description[0] = '\0';
            list->infractions[i].firstPlate = NULL;
            list->infractions[i].plate[0] = '\0';
            list->infractions[i].maxPlateCount = 0;
            list->infractions[i].totalCount = 0;
        }
        list->size = newSize;
    }
    if(list->infractions[infractionId].totalCount == 0) { //Copy the new infraction if it is the first time it was committed
        strcpy(list->infractions[infractionId].description, infractionDesc);
    }

    list->infractions[infractionId].totalCount++; //Register a new apparition

    //If it was not the first time then we only have to add the plate that did it
    size_t newCount = 0;
    list->infractions[infractionId].firstPlate = addPlateRec(list->infractions[infractionId].firstPlate, plate, &newCount);
    if(newCount > list->infractions[infractionId].maxPlateCount){ //Update the plate that committed the infraction the most

        /* FALTA DESEMPATE ALFABETICO: "En caso de que existan dos o más patentes con la misma cantidad de multas para
         * la misma infracción considerar la menor patente en orden alfabético." */

        list->infractions[infractionId].maxPlateCount = newCount;
        strcpy(list->infractions[infractionId].plate, plate);
    }

    if(list->infractions[infractionId].totalCount > list->infractions[list->maxPosInfraction].totalCount) { //Update the most popular infraction by agency

        /* FALTA DESEMPATE ALFABETICO: "En caso de que existan dos o más infracciones con la misma cantidad de multas para la
         * misma agencia emisora considerar la menor infracción en orden alfabético. */

                list->maxPosInfraction = infractionId;
    }
}

static TListAg addTicketRec(TListAg list, const char *agency, const char *infractionDesc, size_t infractionId, const char *plate, int *flag) {
    int c;
    if (list == NULL || (c = strcasecmp(list->agency, agency)) > 0) { // Agency does not exist
        TListAg newAg = calloc(1, sizeof(TNodeAg));
        if (newAg == NULL) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newAg->agency, agency);
        addTicketAux(newAg, infractionDesc, infractionId, plate);
        newAg->tail = list;
        *flag = 1;
        return newAg;
    }
    if (c == 0) {
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

/* REVISAR: "La información debe listarse ordenada en forma descendente por la cantidad
 * total de multas y a igualdad en la cantidad de multas desempatar alfabéticamente por nombre de la infracción. */
static TListInfCount sortByCountRec(TListInfCount list, const char * infractionDesc, size_t count){
    if(list == NULL || (list->count > count)){
        TListInfCount newInfCount = malloc(sizeof(TNodeInfCount));
        if (newInfCount == NULL) {
            errno = ERROR_MEM;
            return list;
        }
        strcpy(newInfCount->description, infractionDesc);
        newInfCount->count = count;
        newInfCount->tail = list;
        return newInfCount;
    }
    if(list->count == count){  /* DEBERIA COMPARAR ACA LAS DESCRIPCIONES ????? REVISAR !!!!!!!!!! */
        return list;
    }
    list->tail = sortByCountRec(list->tail, infractionDesc, count);
    return list;
}

static TListInfAlpha sortAlphaRec(TListInfAlpha list, const char *infractionDesc, size_t count, const char *plate) {
    int c;
    if (list == NULL || (c = strcasecmp(list->description, infractionDesc)) > 0) {
        TListInfAlpha newInfAlpha = malloc(sizeof(TNodeInfAlpha));
        if (newInfAlpha == NULL) {
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
            p->firstCount = sortByCountRec(p->firstCount, aux->infractions[i].description, aux->infractions[i].totalCount);
        }
        aux = aux->tail;
    }
}

static void sortListAlpha(parkingTicketsADT p){
    TListAg aux = p->firstAgency;
    while (aux != NULL) {
        for (size_t i = 0; i < aux->size; i++) {
            p->firstAlpha = sortAlphaRec(p->firstAlpha, aux->infractions[i].description, aux->infractions[i].maxPlateCount, aux->infractions[i].plate);
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

static void freeListAlphaRec(TListInfAlpha list){
    if(list == NULL){
        return;
    }
    TListInfAlpha aux = list;
    list = list->tail;
    free(aux);
    freeListAlphaRec(list);
}

static void freeListCountRec(TListInfCount list){
    if(list == NULL){
        return;
    }
    TListInfCount aux = list;
    list = list->tail;
    free(aux);
    freeListCountRec(list);
}

static void freePlateRec(TListPlate list){
    if(list == NULL){
        return;
    }
    TListPlate aux = list;
    list = list->tail;
    free(aux);
    freePlateRec(list);
}

static void freeInfraction(TInfraction * infractions, int size){
    for(int i = 0; i < size; i++){
        freePlateRec(infractions[i].firstPlate);
    }
}

static void freeListAgRec(TListAg list){
    if(list == NULL){
        return;
    }
    freeInfraction(list->infractions, list->size);
    free(list->infractions);
    TListAg aux = list;
    list = list->tail;
    free(aux);
    freeListAgRec(list);
}

void freeVecReference(parkingTicketsADT p) {
    for (size_t i = 0; i < p->dimIdReference; i++) {
        if (p->idReference[i] != NULL) {
            free(p->idReference[i]);
        }
    }
    free(p->idReference);
}

void freeParkingTickets(parkingTicketsADT p){
    freeListAgRec(p->firstAgency);
    freeListCountRec(p->firstCount);
    freeListAlphaRec(p->firstAlpha);
    freeVecReference(p);
    free(p);
}
