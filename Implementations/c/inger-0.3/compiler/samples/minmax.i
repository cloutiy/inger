/*!MINMAX*/
minmax: int a[], n -> void
{
	int min, max, i, u, v;

	min = a[0]; max = min; i = 2;
	while( i < n-1 ) do
	{
		u = a[i]; v = a[i+1];
		if( u > v )
		{
			if( u > max ) { max = u; }
			if( v < min ) { min = v; }
		}
		else
		{
			if( v > max ) { max = v; }
			if( u < min ) { min = u; }
		}
		i = i + 2;
	}
	if( i == n )
	{
		if( a[n] > max )
		{
			max = a[n];
		}
		else if( a[n] < min )
		{
			min = a[n];
		}
	}
	printint( min );
	printint( max );
}
/*!*/