/*!PRINTINT_I*/
/*
 * printint.i
 *
 * Uses C-implementation of 
 * printint()
 */
module program;

#import "printint.ih"

int a,b;

start main: void -> void
{
    a = b = 1;
    printint( a + b );
}
/*!*/
