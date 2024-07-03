/* CDT and functions to be used in parkingTicketsCHI and parkingTicketsNYC */

#include "parkingTicketsADT.h"

typedef struct parkingTicketsCDT{
    //size_t size;
    struct nodoq13* firstQ1;
    struct nodoq2* firstQ2;
    struct nodoq13* firstQ3;
    char** ReferenceInfraction;
}parkingTicketsCDT;

typedef struct nodoq2{
    char* AgencyName;
    struct ndodoq2* tail;
}nodoq2;

/*
typedef struct nodoq13{
    char* infractionName;
    struct ndodoq13* tailQ1;
    struct ndodoq13* tailQ3;
}nodoq13;
*/
   /// alternativa con 3 listas 
typedef struct nodoq1{
    char* infractionName;
    struct ndodoq1* tailQ1;
}nodoq1;

typedef struct nodoq3{
    char* infractionName;
    struct ndodoq3* tailQ3;
}nodoq3;



nodoq13* addinfraccAlpha(,){


}



