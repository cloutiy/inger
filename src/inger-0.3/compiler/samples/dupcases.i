/*!DUPCASES*/
/* Duplicate cases
 * A program with duplicate case values
 */
module duplicate_cases;

start main : void -> void
{
  int a = 0;

  switch( a )
  {
    case 0
    {
      printf( "This is the first case block" );

    }
    case 0
    {
      printf( "This is the second case block" );

    }
    default
    {
      printf( "This is the default case" );

    }
  }
}
/*!*/
