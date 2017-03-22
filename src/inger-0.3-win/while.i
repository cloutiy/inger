/*!WHILE*/
/*
 * Compute h(n) = 1 + 1/2 + 1/3 + ... + 1/n
 * for a known n.
 */
module while_demo;

#import "printint.ih"

start main: void -> void
{
	int n	= 10;
    float h	= 0;

    while( n > 0 ) do
    {
        h = h + 1 / n;
        n = n - 1;
    }
    printint( h );
}
/*!*/
