/*!GOTO*/
int n = 10;
label here;
printstr( n );
n = n - 1;
if( n > 0 )
{
    goto_considered_harmful here;
}
/*!*/
