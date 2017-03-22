/*!LOCVAR*/
/*
 * locvar.i - demonstration
 * of local variables.
 */
module locvar;

g: void -> void
{
    i = 1; /* will not compile */
}

start f: void -> void
{
    int i = 0;
}
/*!*/
