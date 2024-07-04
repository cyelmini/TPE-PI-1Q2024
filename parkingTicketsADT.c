/* CDT and functions to be used in parkingTicketsCHI and parkingTicketsNYC */

#include "parkingTicketsADT.h"
#include <errno.h>

/*
 * La idea es recibir la informacion de las infractions y cargarlas segun su id en el vector de las agencies.
 * Despues de que esa funcion sea llamada, otras dos funciones se encargan de crear las listas ordenadas de las formas pedidas
 * en las queries.
 */

typedef struct plate{
    char plate[MAX_PLATE];
    size_t count;
}TPlate;

typedef struct infraction{
    char description[MAX_DESC];
    TPlate * plates; // Vector containing the different plates that committed the infraction
    size_t maxPosPlate; // Position where the plate that repeated the infraction the most is stored in the vector
    size_t count; // Total amount of times the infraction was committed
}TInfraction;

typedef struct nodeAg{
    char agency[MAX_AG];
    TInfraction * infractions; // Vector containing the different infractions issued by the correspondent agency (each position in this vector corresponds with the infractionId)
    size_t size; // Reserved space for the vector
    size_t totalCount; // Total amount of infractions
    size_t maxPosInfraction; // Position where the most repeated infraction is stored in the vector
    struct node * tail;  // Pointer to the next agency
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
    TListInf firstAlpha; //  Pointer to the first element of the list of infractions ordered alphabetically (to be used int query2)
};


parkingTicketsADT newParking(void){
    errno = OK;
    parkingTicketsADT aux = calloc(1, sizeof(parkingTicketsCDT));

    if(errno == ENOMEM || aux == NULL){
        errno = MEMERR;
        return NULL;
    }
    return aux;
}



