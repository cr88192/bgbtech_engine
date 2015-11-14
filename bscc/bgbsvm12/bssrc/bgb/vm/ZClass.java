package bgb.vm;

public class ZClass extends Object
{
public native static ZClass forName(String className);
public native ClassLoader getClassLoader();
public native Class[] getInterfaces();
public native String getName();
public native Class getSuperclass();
public native boolean isInterface();
public native Object newInstance();
public native String toString();
}
