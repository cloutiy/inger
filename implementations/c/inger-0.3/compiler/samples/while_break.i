/*!WHILE_BREAK*/
int a = 10;

while( a > 0 )
{
    if( a == 5 )
    {
        break;
    }

    printfint( a );
}
/*!*/
/*!WHILE_BREAK_OUTPUT*/
10
9
8
7
6
/*!*/
/*!WHILE_CONTINUE*/
int a = 10;
while( a > 0 )
{
    if( a % 2 == 0 )
    {
        continue;
    }

    printint( a );
}
/*!*/
/*!WHILE_CONTINUE_OUTPUT*/
10
8
6
4
2
/*!*/

