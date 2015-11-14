package bgb.vm;
import bsvm.util.*;
// import bsvm.io.*;

public class Z_System extends Object
{

public Z_IFileSystem fs;


public static void arraycopy(
	int[] src, int src_position, int[] dst, int dst_position,
	int length)
{
	int i;
	for(i=0; i<length; i++)dst[dst_position+i]=src[src_position+i];
}

public static void arraycopy(
	byte[] src, int src_position, byte[] dst, int dst_position,
	int length)
{
	int i;
	for(i=0; i<length; i++)dst[dst_position+i]=src[src_position+i];
}

public static void arraycopy(
	char[] src, int src_position, char[] dst, int dst_position,
	int length)
{
	int i;
	for(i=0; i<length; i++)dst[dst_position+i]=src[src_position+i];
}

public static void arraycopy(
	short[] src, int src_position, short[] dst, int dst_position,
	int length)
{
	int i;
	for(i=0; i<length; i++)dst[dst_position+i]=src[src_position+i];
}

public static native long currentTimeMillis();
public static native void exit(int status);
public static native void gc();

public static native String getenv(String name);
public static native Properties getProperties();
public static native String getProperty(String key);
public static native String getProperty(String key, String def);
public static native SecurityManager getSecurityManager();

public static native void load(String filename);
public static native void loadLibrary(String libname);
public static native void runFinalization();
public static native void setProperties(Properties props);
public static native void setSecurityManager(SecurityManager s);

}
