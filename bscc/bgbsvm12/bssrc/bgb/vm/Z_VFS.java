package bgb.vm;

public class Z_VFS
{

// Volume operations
public static native long open(String name, String mode);
public static native int mount(String src, String dst,
	String fstype, String[] ops);
public static native int umount(String mnt);

// Directory operations
public static native long opendir(String name);
public static native void closedir(long dir);
public static native String readdir(long dir);
public static native void rewinddir(long dir);

public static native int statflags(String name);
//public int stat(String name, VFILE_Stat *st);
//public int setstat(String name, VFILE_Stat *st, int fl);

public static native int mkdir(String name);
public static native int rmdir(String name);
public static native int unlink(String name);
public static native int link(String oldname, String newname, int flags);
public static native int fscmd(int cmd, Object arg);


// File IO operations

public static native int read(byte[] b, int len, long fd);
public static native int write(byte[] b, int len, long fd);
public static native int getc(long fd);
public static native void putc(int val, long fd);
public static native void flush(long fd);

public static native int seek(long fd, long pos, int rel);
public static native long tell(long fd);
public static native int eof(long fd);
public static native void close(long fd);
public static native int ioctl(long fd, int req, Object arg);
}

