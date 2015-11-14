package bgb.vm;

public interface Z_IFile
{
public int read(byte[] b, int len);
public int write(byte[] b, int len);
public int getc();
public int putc();
public void flush();

public int seek(long pos, int rel);
public long tell();
public int eof();
public void close();
public int ioctl(int req, Object arg);
}

