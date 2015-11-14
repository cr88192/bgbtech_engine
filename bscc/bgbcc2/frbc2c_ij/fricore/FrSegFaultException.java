package fricore;

import java.lang.*;

public class FrSegFaultException extends RuntimeException
{
	public FrSegFaultException()
		{ super(); }
	public FrSegFaultException(String s)
		{ super(s); }
}
