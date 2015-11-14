package bgb.vm;

public final class Z_SysUtil extends Object
{
public static native String IntToStr(long l, int radix);
public static native long StrToInt(String str, int radix);
public static native String FloatToStr(double l);
public static native double StrToFloat(String str);

public static native int getAddrByte(long addr);
public static native int getAddrSByte(long addr);
public static native int getAddrShort(long addr);
public static native int getAddrUShort(long addr);
public static native int getAddrInt(long addr);
public static native long getAddrLong(long addr);
public static native float getAddrFloat(long addr);
public static native double getAddrDouble(long addr);
public static native long getAddrPtr(long addr);

public static native void setAddrByte(long addr, int v);
public static native void setAddrSByte(long addr, int v);
public static native void setAddrShort(long addr, int v);
public static native void setAddrUShort(long addr, int v);
public static native void setAddrInt(long addr, int v);
public static native void setAddrLong(long addr, long v);
public static native void setAddrFloat(long addr, float v);
public static native void setAddrDouble(long addr, double v);
public static native void setAddrPtr(long addr, long v);
}
