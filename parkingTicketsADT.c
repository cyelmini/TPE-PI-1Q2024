/* CDT and functions to be used in parkingTicketsCHI and parkingTicketsNYC */

#include "parkingTicketsADT.h"

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

typedef struct node{
    char agency[MAX_AG];
    TInfraction * infractions; // Vector containing the different infractions issued by the correspondent agency (each position in this vector corresponds with the infractionId)
    size_t size; // Reserved space for the vector
    size_t totalCount; // Total amount of infractions
    size_t maxPosInfraction; // Position where the most repeated infraction is stored in the vector
    struct node * tail;  // Pointer to the next agency
} TNode;

typedef TNode * TList;

struct parkingTicketsCDT{
    TList firstAgency;  // Pointer to the first element of the list of agencies, which is added in alphabetical order.
};

