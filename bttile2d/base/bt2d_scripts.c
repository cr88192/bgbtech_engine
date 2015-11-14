${
	var tstVar=4;
}

int Tile2D_TstFunc()
{
	int x, y;
	x=3;
	
	y = ${ tstVar+x }:int;

	return y;
}
