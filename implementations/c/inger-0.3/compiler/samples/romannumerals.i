/*!ROMANNUMERALS*/
/* Write roman numerals for the powers of 2. */
module roman_numerals;

#import "stdio.ih"

start main: void -> void
{
	int x, y = 1;
	while( y <= 5000 )
    {
		x = y;
		printint( x );
		
		while( x >= 1000 ) do
        {
            printstr( "M" );
            x = x - 1000;
        }
        if(x >= 5000 )
        {
            printstr( "D" );
            x = x - 500;
        }
		while( x >= 100 ) do
        {
            printstr( "C" );
            x = x - 100;
        }
        if(x >= 50 )
        {
            printstr( "L" );
            x = x - 50;
        }
		while( x >= 10 ) do
        {
            printstr( "X" );
            x = x - 10;
        }
        if(x >= 5 )
        {
            printstr( "V" );
            x = x - 5;
        }
		while( x >= 1 ) do
        {
            printstr( "I" );
            x = x - 1;
        }
		printstr( "\n" );
		y = 2 * y;
    }
}
/*!*/
/*!ROMANNUMERALSOUTPUT*/
Output:
        1   I
        2   II
        4   IIII
        8   VIII
       16   XVI
       32   XXXII
       64   LXIIII
      128   CXXVIII
      256   CCLVI
      512   DXII
     1024   MXXIIII
     2048   MMXXXXVIII
     4096   MMMMLXXXXVI
/*!*/
