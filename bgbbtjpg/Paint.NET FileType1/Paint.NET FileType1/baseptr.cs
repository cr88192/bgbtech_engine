using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BGBTechBTJ
{
    public struct BytePtr
    {
        public byte[] buf;
        public int pos;
        public bool readOnly;

        public BytePtr(byte[] arr)
        {
            buf = arr; pos = 0; readOnly = false;
        }

        public BytePtr(byte[] arr, int idx)
        {
            buf = arr; pos = idx; readOnly = false;
        }

        public BytePtr(byte[] arr, int idx, bool ro)
        {
            buf = arr; pos = idx; readOnly = ro;
        }

        public BytePtr(int sz)
        {
            buf = new byte[sz]; pos = 0; readOnly = false;
        }

        public int this[int idx]
        {
            get { return buf[pos+idx]; }
            set {
                if (readOnly) throw new Exception();
                buf[pos+idx] = (byte)value;
            }
        }

        public static BytePtr operator +(BytePtr ptr, int idx)
        {
            return new BytePtr(ptr.buf, ptr.pos + idx, ptr.readOnly);
        }

        public static int operator -(BytePtr ptra, BytePtr ptrb)
        {
            if (ptra.buf != ptrb.buf)
                throw new Exception();
            return ptra.pos - ptrb.pos;
        }

        public static bool operator <(BytePtr ptra, BytePtr ptrb)
        {
            if (ptra.buf != ptrb.buf)
                throw new Exception();
            return ptra.pos < ptrb.pos;
        }

        public static bool operator >(BytePtr ptra, BytePtr ptrb)
        {
            if (ptra.buf != ptrb.buf)
                throw new Exception();
            return ptra.pos > ptrb.pos;
        }

        public static bool operator <=(BytePtr ptra, BytePtr ptrb)
        {
            if (ptra.buf != ptrb.buf)
                throw new Exception();
            return ptra.pos <= ptrb.pos;
        }

        public static bool operator >=(BytePtr ptra, BytePtr ptrb)
        {
            if (ptra.buf != ptrb.buf)
                throw new Exception();
            return ptra.pos >= ptrb.pos;
        }

        public static BytePtr operator ++(BytePtr ptr)
        {
            return new BytePtr(ptr.buf, ptr.pos + 1);
        }

        public string getString()
        {
            char[] tmp = new char[4096];
            int i;
            for (i = 0; this[i]!=0; i++)
            {
                tmp[i] = (char)this[i];
            }
            return (new String(tmp, 0, i));
        }

        public void EmitFourcc(string str)
        {
            int i;

            for (i = 0; i < 4; i++)
                emit(str[i]);
        }

        public static BytePtr EmitString(BytePtr ptr, string str)
        {
            int i;

            for (i = 0; i < str.Length; i++)
                ptr.emit(str[i]);
            ptr.emit(0);
            return ptr;
        }

        public void EmitString(string str)
        {
            int i;

            for (i = 0; i < str.Length; i++)
                emit(str[i]);
            emit(0);
        }

        public bool strcmp(string str)
        {
            int i;
            for (i = 0; i < str.Length; i++)
            {
                if (str[i] != this[i])
                    return true;
            }
            if (this[i] != 0)
                return (true);
            return false;
        }

        public bool memcmp(string str, int sz)
        {
            int i;
            for (i = 0; i < sz; i++)
            {
                if (str[i] != this[i])
                    return true;
            }
            return false;
        }

        public int strlen()
        {
            int i;
            for (i = 0; ; i++)
            {
                if (this[i] == 0)
                    return i;
            }

        }

        public void memset(int value, int sz)
        {
            int i;
            if (readOnly)
                throw new Exception();
            for (i = 0; i < sz; i++)
                this[i] = value;
        }

        public static void memset(byte[] buf, int value, int sz)
        {
            int i;
            for (i = 0; i < sz; i++)
                buf[i] = (byte)value;
        }

        public static void memcpy(BytePtr ptra, BytePtr ptrb, int sz)
        {
            int i;

            if (ptra.readOnly)
                throw new Exception();
            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public static void memcpy(byte[] ptra, BytePtr ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = (byte)ptrb[i];
        }

        public static void memcpy(BytePtr ptra, byte[] ptrb, int sz)
        {
            int i;

            if (ptra.readOnly)
                throw new Exception();

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public static void memcpy(byte[] ptra, byte[] ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public int next()
        {
            return buf[pos++];
        }

        public void emit(byte value)
        {
            if (readOnly)
                throw new Exception();
            buf[pos++] = value;
        }

        public void emit(int value)
        {
            if (readOnly)
                throw new Exception();
            buf[pos++] = (byte)value;
        }

        public void EmitWord16BE(int value)
        {
            this.emit((value >> 8) & 255);
            this.emit(value & 255);
            buf[pos++] = (byte)value;
        }

        public void EmitUVLI(ulong value)
        {
            if (value < 128)
            {
                emit((byte)(value & 127));
            }
            else if (value < 16384)
            {
                emit((byte)(0x80 | ((value >> 8) & 63)));
                emit((byte)(value & 255));
            }
            else if (value < 2097152)
            {
                emit((byte)(0xC0 | ((value >> 16) & 31)));
                emit((byte)((value >> 8) & 255));
                emit((byte)(value & 255));
            }
            else if (value < 268435456)
            {
                emit((byte)(0xE0 | ((value >> 24) & 15)));
                emit((byte)((value >> 16) & 255));
                emit((byte)((value >> 8) & 255));
                emit((byte)(value & 255));
            }
            else if (value < 34359738368)
            {
                emit((byte)(0xF0 | ((value >> 32) & 7)));
                emit((byte)((value >> 24) & 255));
                emit((byte)((value >> 16) & 255));
                emit((byte)((value >> 8) & 255));
                emit((byte)(value & 255));
            }
        }

        public void EmitSVLI(long v)
        {
            ulong lv;
            lv = (ulong)((v >= 0) ? (v << 1) : (((-v) << 1) - 1));
            EmitUVLI(lv);
        }

        public ulong ReadUVLI()
        {
            ulong lv;
            int i;
            i = next();
            if ((i & 0x80) == 0)
            {
                lv = (ulong)(i & 127);
                return lv;
            }
            else if ((i & 0xC0) == 0x80)
            {
                lv = (ulong)(i & 63);
                lv = (lv << 8) + (ulong)next();
                return lv;
            }
            else if ((i & 0xE0) == 0xC0)
            {
                lv = (ulong)(i & 31);
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                return lv;
            }
            else if ((i & 0xF0) == 0xE0)
            {
                lv = (ulong)(i & 15);
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                return lv;
            }
            else if ((i & 0xF8) == 0xF0)
            {
                lv = (ulong)(i & 7);
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                return lv;
            }
            else if ((i & 0xFC) == 0xF8)
            {
                lv = (ulong)(i & 3);
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                return lv;
            }
            else if ((i & 0xFE) == 0xFC)
            {
                lv = (ulong)(i & 1);
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                return lv;
            }
            else if ((i & 0xFF) == 0xFE)
            {
                lv = 0;
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                return lv;
            }

            i = next();
            if ((i & 0x80) == 0)
            {
                lv = (ulong)(i & 127);
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                lv = (lv << 8) + (ulong)next();
                return lv;
            }

            return 0;
        }

        public long ReadSVLI()
        {
            ulong lv;
            long v;

            lv = ReadUVLI();
            v = ((lv & 1) != 0) ? (-(long)((lv + 1) >> 1)) : (long)(lv >> 1);
            return v;
        }

        public bool isNull { get { return buf == null; } }

        //        public int get(int idx)
        //            { return buf[idx]; }
        //        public void set(int idx, int val)
        //          { buf[idx] = (byte)val; }
    }

    public struct ShortPtr
    {
        short[] buf;
        int pos;

        public ShortPtr(short[] arr)
        {
            buf = arr; pos = 0;
        }

        public ShortPtr(short[] arr, int idx)
        {
            buf = arr; pos = idx;
        }

        public ShortPtr(ShortPtr arr, int idx)
        {
            buf = arr.buf; pos = arr.pos + idx;
        }

        public ShortPtr(int sz)
        {
            buf = new short[sz]; pos = 0;
        }

        public int this[int idx]
        {
            get { return buf[pos+idx]; }
            set { buf[pos+idx] = (short)value; }
        }

        public static ShortPtr operator +(ShortPtr ptr, int idx)
        {
            return new ShortPtr(ptr.buf, ptr.pos + idx);
        }

        public static void memcpy16(ShortPtr dst, ShortPtr src)
        {
            int i;
            for (i = 0; i < 8; i++)
                dst[i] = src[i];
        }

        public static void memset(short[] buf, int value, int sz)
        {
            int i;
            for (i = 0; i < sz; i++)
                buf[i] = (short)value;
        }

        public static void memcpy(ShortPtr ptra, ShortPtr ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public static void memcpy(short[] ptra, ShortPtr ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = (short)ptrb[i];
        }

        public static void memcpy(ShortPtr ptra, short[] ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public static void memcpy(short[] ptra, short[] ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public void emit(int value)
        {
            buf[pos++] = (short)value;
        }

        public bool isNull { get { return buf == null; } }
    }

    public struct IntPtr
    {
        int[] buf;
        int pos;

        public IntPtr(int[] arr)
        {
            buf = arr; pos = 0;
        }

        public IntPtr(int[] arr, int idx)
        {
            buf = arr; pos = idx;
        }

        public IntPtr(int sz)
        {
            buf = new int[sz]; pos = 0;
        }

        public int this[int idx]
        {
            get { return buf[pos+idx]; }
            set { buf[pos+idx] = (int)value; }
        }

        public static IntPtr operator +(IntPtr ptr, int idx)
        {
            return new IntPtr(ptr.buf, ptr.pos + idx);
        }

        public bool isNull { get { return buf == null; } }
    }

    public struct FloatPtr
    {
        float[] buf;
        int pos;

        public FloatPtr(float[] arr)
        {
            buf = arr; pos = 0;
        }

        public FloatPtr(float[] arr, int idx)
        {
            buf = arr; pos = idx;
        }

        public FloatPtr(int sz)
        {
            buf = new float[sz]; pos = 0;
        }

        public float this[int idx]
        {
            get { return buf[pos+idx]; }
            set { buf[pos+idx] = (float)value; }
        }

        public static FloatPtr operator +(FloatPtr ptr, int idx)
        {
            return new FloatPtr(ptr.buf, ptr.pos + idx);
        }

        public bool isNull { get { return buf == null; } }
    }

    public struct TypePtr<T>
    {
        T[] buf;
        int pos;

        public TypePtr(T[] arr)
        {
            buf = arr; pos = 0;
        }

        public TypePtr(T[] arr, int idx)
        {
            buf = arr; pos = idx;
        }

        public TypePtr(TypePtr<T> arr, int idx)
        {
            buf = arr.buf; pos = arr.pos + idx;
        }

        public TypePtr(int sz)
        {
            buf = new T[sz]; pos = 0;
        }

        public T this[int idx]
        {
            get { return buf[pos + idx]; }
            set { buf[pos + idx] = (T)value; }
        }

        public static TypePtr<T> operator +(TypePtr<T> ptr, int idx)
        {
            return new TypePtr<T>(ptr.buf, ptr.pos + idx);
        }

        public static void memcpy16(TypePtr<T> dst, TypePtr<T> src)
        {
            int i;
            for (i = 0; i < 8; i++)
                dst[i] = src[i];
        }

        public static void memset(T[] buf, T value, int sz)
        {
            int i;
            for (i = 0; i < sz; i++)
                buf[i] = value;
        }

        public static void memcpy(TypePtr<T> ptra, TypePtr<T> ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public static void memcpy(T[] ptra, TypePtr<T> ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = (T)ptrb[i];
        }

        public static void memcpy(TypePtr<T> ptra, T[] ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public static void memcpy(T[] ptra, T[] ptrb, int sz)
        {
            int i;

            for (i = 0; i < sz; i++)
                ptra[i] = ptrb[i];
        }

        public void emit(T value)
        {
            buf[pos++] = (T)value;
        }

        public bool isNull { get { return buf == null; } }
    }
}
