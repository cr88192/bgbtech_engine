package bgb.vm;

public final class Fixnum
{
private Fixnum() {}

public static boolean p(Dynamic val)
	{ return Dynamic.fixnumP(val); }

public static Dynamic v(int val)
	{ return Dynamic.fromInt(val); }
public static int v(Dynamic val)
	{ return Dynamic.toInt(val); }

public static int toInt(Dynamic val)
	{ return Dynamic.toInt(val); }
public static long toLong(Dynamic val)
	{ return Dynamic.toLong(val); }
}
