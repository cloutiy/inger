/*!GLOBVAR*/
/*
 * globvar.i - demonstration
 * of global variables.
 */
module globvar;

int i;
bool b;
char c;

g: void -> void
{
    i = 0;
    b = false;
    c = 'b';
}

start f: void -> void
{
    i = 1;
    b = true;
    c = 'a';
}
/*!*/
