package fricore;

/** Represents a virtual pointer object */
public abstract class FrPointer
{
	public abstract int geti(int idx);
	public abstract long getl(int idx);
	public abstract float getf(int idx);
	public abstract double getd(int idx);
	public abstract int getsb(int idx);
	public abstract int getub(int idx);
	public abstract int getss(int idx);
	public abstract int getus(int idx);
	public abstract long getsi(int idx);
	public abstract long getui(int idx);
	public abstract long getsl(int idx);
	public abstract long getul(int idx);

	public abstract void seti(int idx, int val);
	public abstract void setl(int idx, long val);
	public abstract void setf(int idx, float val);
	public abstract void setd(int idx, double val);
	public abstract void setsb(int idx, int val);
	public abstract void setub(int idx, int val);
	public abstract void setss(int idx, int val);
	public abstract void setus(int idx, int val);

	public abstract FrPointer getp(int idx);
	public abstract void setp(int idx, FrPointer val);

	public abstract FrPointer leai(int idx);
	public abstract FrPointer leal(int idx);
	public abstract FrPointer leaf(int idx);
	public abstract FrPointer lead(int idx);
	public abstract FrPointer leab(int idx);
	public abstract FrPointer leas(int idx);
	public abstract FrPointer leap(int idx);

	public abstract FrMemSpace getSpace();
	public abstract long getAddress();
}
