
#include "utils.h"



void *MallocEx( size_t size )
{
    void *ptr;
    ptr = malloc( size );

    if( ptr == NULL )
    {
        BAILOUT( ERR_NOMEM );
    }

    return( ptr ); 
}
