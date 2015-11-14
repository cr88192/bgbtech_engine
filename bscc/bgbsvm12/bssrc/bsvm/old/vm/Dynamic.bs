package bgb.vm;
/**
 * Support dynamic types.
 * Instances of Dynamic are not allowed as these are not real class/instance
 * Objects. All methods are static for similar reasons.
 */
public final class Dynamic
{
private Dynamic() {}

public native static boolean fixnumP(Dynamic val);
public native static boolean flonumP(Dynamic val);
public native static boolean ObjectP(Dynamic val);

public native static int toInt(Dynamic val);
public native static long toLong(Dynamic val);
public native static float toFloat(Dynamic val);
public native static double toDouble(Dynamic val);

public native static Dynamic fromInt(int val);
public native static Dynamic fromLong(long val);
public native static Dynamic fromFloat(float val);
public native static Dynamic fromDouble(double val);

/**
 * Coerce dynamic reference to Object.
 * obj needs to be a valid Object.
 */
public native static Object toObject(Dynamic obj);

/**
 * Coerce an Object to a dynamic reference.
 */
public native static Dynamic fromObject(Object obj);

/**
 * Call method using dynamic types.
 */
public native static Dynamic call(
	Dynamic obj, String name, Dynamic[] args);

public native static Dynamic call(
	Dynamic obj, String name);
public native static Dynamic call(
	Dynamic obj, String name, Dynamic a);
public native static Dynamic call(
	Dynamic obj, String name, Dynamic a, Dynamic b);
public native static Dynamic call(
	Dynamic obj, String name, Dynamic a, Dynamic b, Dynamic c);
public native static Dynamic call(
	Dynamic obj, String name, Dynamic a, Dynamic b, Dynamic c, Dynamic d);

}
