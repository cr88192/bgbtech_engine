function fib(x)
{
	if(x>2)return(fib(x-1)+fib(x-2));
	return(1);
}

function ltst(x)
{
	var i, j;
	j=0;
	for(i=0; i<x; i++)j++;

	println("ltst " i " " j);

	return(j);
}
