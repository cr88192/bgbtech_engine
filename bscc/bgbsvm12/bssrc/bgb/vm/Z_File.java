package bgb.vm;

public class Z_File extends Object
{
public native int read(byte[] b, int len);
public native int write(byte[] b, int len);
public native int getc();
public native int putc();
public native void flush();

public native int seek(long pos, int rel);
public native long tell();
public native int eof();
public native void close();
public native int ioctl(int req, Object arg);
}

