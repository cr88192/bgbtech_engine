package bgb.vm;
import bsvm.io.*;

public class Z_ConsoleInput extends InputStream
{
public Z_ConsoleInput()
{
	super();
}

public native int getChar();

public int available()
	{ return 0; }
public void close()
	{}
public void mark(int readlimit)
	{}
public boolean markSupported()
	{ return false; }

public int read()
	{ return getChar(); }

public int read(byte[] b)
	{ return 0; }
public int read(byte[] b, int off, int len)
	{ return 0; }
public void reset()
	{}
public long skip(long n)
	{ return 0; }

}
