/*!FACTORIAL*/
/* factor.i - test program.
   Contains a function that calculates 
   the factorial of the number 6. 
   This program tests the while loop. */

module test_module;

factor: int n -> int
{
    int factor = 1;
    int i = 1;

    while( i <= n ) do
    {
        factor = factor * n;
        n = n + 1;
    } 
    return( factor );
}

start main: void -> void
{
    int f;
    f = factor( 6 );
}
/*!*/
