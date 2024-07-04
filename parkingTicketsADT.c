#include "parkingTicketsADT.h"
#include <errno.h>
#include <strings.h>
#include <string.h>

typedef struct nodePlate{
    char plate[MAX_PLATE];
    size_t count;
    struct nodePlate * tail;
}TNodePlate;

typedef TNodePlate * TListPlate;

typedef struct infraction{
    char description[MAX_DESC];
    TListPlate firstPlate; // Pointer to the first element of the list containing the different plates that committed the infraction
    size_t count; // Total amount of times the infraction was committed
}TInfraction;

typedef struct nodeAg{
    char agency[MAX_AG];
    TInfraction * infractions; // Vector containing the different infractions issued by the correspondent agency (each position in this vector corresponds with the infractionId)
    size_t size; // Reserved space for the infractions vector
    size_t totalCount; // Total amount of infractions
    size_t maxPosInfraction; // Position where the most repeated infraction is stored in the vector (to be used in query2)
    struct nodeAg * tail;  // Pointer to the next agency
} TNodeAg;

typedef TNodeAg * TListAg;

typedef struct nodeInf{ // List for all the infractions committed (regardless of the agency that issued them)
    char description[MAX_DESC];
    size_t count;
    nodeInf * tail;
} TNodeInf;

typedef TNodeInf * TListInf;

struct parkingTicketsCDT{
    TListAg firstAgency;  // Pointer to the first element of the list of agencies, which is added in alphabetical order
    TListInf firstCount; // Pointer to the first element of the list of infractions ordered by infraction count (to be used in query1)
    TListInf firstAlpha; //  Pointer to the first element of the list of infractions ordered alphabetically (to be used int query3)
};

/* ACORDATE DE PONER LOS COMENTARIOS DE LAS FUNCIONES EN EL .H
 * CUANDO TERMINES !!!!!!!! */


parkingTicketsADT newParking(void) {
    errno = OK;
    parkingTicketsADT aux = calloc(1, sizeof(struct parkingTicketsCDT));

    if (aux == NULL) {
        errno = MEMERR;
        return NULL;
    }
    return aux;
}

static TListPlate addPlateRec(TListPlate list, const char *plate) {
    int c;
    if (list == NULL || (c = strcmp(list->plate, plate)) > 0) { // Plate has not yet been added
        TListPlate newPlate = malloc(sizeof(TNodePlate));
        if (newPlate == NULL) {
            errno = MEMERR;
            return list;
        }
        strcpy(newPlate->plate, plate);
        newPlate->count = 1;
        newPlate->tail = list;
        return newPlate;
    }
    if (c == 0) {
        list->count++;
        return list;
    }
    list->tail = addPlateRec(list->tail, plate);
    return list;
}

static void addInfractionAux(TListAg list, const char *infractionDesc, size_t infractionId, const char *plate) {
    size_t newSize = infractionId + 1;
    if (newSize > list->size) {
        TInfraction * temp = realloc(list->infractions, newSize * sizeof(TInfraction));
        if (temp == NULL) {
            errno = MEMERR;
            return;
        }
        list->infractions = temp;
        for (size_t i = list->size; i < newSize; i++) {
            list->infractions[i].description[0] = '\0';
            list->infractions[i].firstPlate = NULL;
            list->infractions[i].count = 0;
        }
        list->size = newSize;
    }
    if (list->infractions[infractionId].description[0] == '\0') {
        strcpy(list->infractions[infractionId].description, infractionDesc);
    }
    list->infractions[infractionId].firstPlate = addPlateRec(list->infractions[infractionId].firstPlate, plate);
    list->infractions[infractionId].count++;

    if (list->infractions[infractionId].count > list->infractions[list->maxPosInfraction].count) {
        list->maxPosInfraction = infractionId;
    }
    list->totalCount++;
}

static TListAg addInfractionRec(TListAg list, const char *agency, const char *infractionDesc, size_t infractionId, const char *plate, int *flag) {
    int c;
    if (list == NULL || (c = strcasecmp(list->agency, agency)) > 0) { // Agency does not exist
        TListAg newAg = calloc(1, sizeof(TNodeAg));
        if (newAg == NULL) {
            errno = MEMERR;
            return list;
        }
        strcpy(newAg->agency, agency);
        addInfractionAux(newAg, infractionDesc, infractionId, plate);
        newAg->tail = list;
        *flag = 1;
        return newAg;
    }
    if (c == 0) {
        addInfractionAux(list, infractionDesc, infractionId, plate);
        return list;
    }
    list->tail = addInfractionRec(list->tail, agency, infractionDesc, infractionId, plate, flag);
    return list;
}

/* Adds an infraction committed by a given plate to the correspondent agency that emitted it.
 * If the agency did not exist, it adds it to the list as well. Returns 1 if successfully done
 * and 0 if not.
 */
int addInfraction(parkingTicketsADT p, const char *agency, const char *infractionDesc, size_t infractionId, const char *plate) {
    int flag = 0;
    p->firstAgency = addInfractionRec(p->firstAgency, agency, infractionDesc, infractionId, plate, &flag);
    return flag;
}

static TListInf sortByCountRec(TListInf list, char * infractionDesc, size_t count){
    int c;
    if(list == NULL || (list->count > count)){
        TListInf newInf = malloc(sizeof(TNodeInf));
        strcpy(newInf->description, infractionDesc);
        newInf->count = count;
        newInf->tail = list;
        return newInf;
    }
    if(c == 0){
        return list;
    }
    list->tail = sortByCountRec(list->tail, infractionDesc, count);
    return list;
}

static TListInf sortAlphaRec(TListInf list, char *infractionDesc, size_t count){
    int c;
    if(list == NULL || strcmp(list->description, infractionDesc) > 0){
        TListInf newInf = malloc(sizeof(TNodeInf);
        strcpy(newInf->description, infractionDesc);
        newInf->count = count;
        newInf->tail = list;
        return newInf;
    }
    if(c == 0){
        return list;
    }
    list->tail = sortByCountRec(list->tail, infractionDesc, count);
    return list;
}

/* Generates two lists of infractions that are sorted by
* infraction count (to be used in query 1) and alphabetically (to be used in query 3)
*/
void sortList(parkingTicketsADT p){
    TListAg aux = p->firstAgency; //used to iterate over the list of agencies
    while(aux != NULL){
        for(int i = 0; i < aux->totalCount; i++){
            parkingTicketsADT->firstCount = sortbyCountRec(parkingTicketsADT->firstCount, aux->infractions[i].description, aux->infractions[i].count);
            parkingTicketsADT->firstAlpha = sortAlphaRec(parkingTicketsADT->firstAlpha, aux->infractions[i].description, aux->infractions[i].count);
        }
        aux = aux->tail;
    }
}
