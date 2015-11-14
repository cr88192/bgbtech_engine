#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbgc.h>
#include <bgbdy.h>

dycSlot tst_slot_sx, tst_slot_sy, tst_slot_sz;

double tst_method_foo(dycObject self)
{
	double f, g;
//	printf("Foo: %p %p\n", self, data);

//	f=dycGetSlotd(self, "x");
//	g=dycGetSlotd(self, "y");

	f=dycGetd(self, tst_slot_sx);
	g=dycGetd(self, tst_slot_sy);

//	printf("Foo: %p %f %f\n", self, f, g);

	return(f*f+g*g);

	return(0);
}

double tst_method_bar(dycObject self)
{
	return(0);
}

void tst_method_tst2(dycObject self, double f, double g)
{
	printf("Init: %p %f %f\n", self, f, g);
	dycSetSlotd(self, "x", f);
	dycSetSlotd(self, "y", g);
}

int tst_initclass()
{
	printf("Test: Init Classes\n");

	dycBeginClass("test0", NULL);
	dycSlotBasic("x", "d");
	dycSlotBasic("y", "d");
	dycEndClass();

	dycBeginClass("test1", "test0");
	dycSlotBasic("z", "d");
	dycEndClass();

	dycBeginClass("test2", "test1");
	dycMethodCSelf("test2", "(dd)v", tst_method_tst2);
	dycMethodCSelf("foo", "()d", tst_method_foo);
	dycMethodCSelf("bar", "()d", tst_method_bar);
	dycEndClass();

	dycBeginIface("iftest0", NULL);
	dycIfaceMethod("foo", "()d");
	dycIfaceMethod("bar", "()d");
	dycEndIface();
}

int tst_dummy()
{
}

int tst_dummy_mtx(dyt mtx)
{
	thLockFastMutex(mtx);
	thUnlockFastMutex(mtx);
}

int tst_basics()
{
	dycObject obj, obj1;
	dycClass inf;
	dycSlot sx, sy, sz;
	dyt mtx;
	int i, j, t, dt, n;
	s64 cn0, cn1, cn2, cn3;

	printf("Test: Object Basics\n");

	obj=dycNewClass("test0");
	printf("Obj OK: %p\n", obj);

	dycSetSlotd(obj, "x", 3.0);
	dycSetSlotd(obj, "y", 4.0);

	printf("Obj Slot: %f %f\n",
		dycGetSlotd(obj, "x"), dycGetSlotd(obj, "y"));

	inf=dycGetClass("test0");
	sx=dycIdxSlot(inf, "x");
	sy=dycIdxSlot(inf, "y");
	sz=dycIdxSlot(inf, "z");

	printf("Inf=%p Sx=%p Sy=%p, Sz(NULL)=%p\n", inf, sx, sy, sz);

	printf("Obj Slot2: %f %f\n",
		dycGetd(obj, sx), dycGetd(obj, sy));


	inf=dycGetClass("test1");
	sx=dycIdxSlot(inf, "x");
	sy=dycIdxSlot(inf, "y");
	sz=dycIdxSlot(inf, "z");

	printf("Inf=%p Sx=%p Sy=%p, Sz=%p\n", inf, sx, sy, sz);

	tst_slot_sx=sx;
	tst_slot_sy=sy;
	tst_slot_sz=sz;

	obj1=dycNew(inf);

	printf("Obj1: %p\n", obj1);

	dycSetd(obj1, sx, 3.0);
	dycSetd(obj1, sy, 4.0);
	dycSetd(obj1, sz, 5.0);

	printf("Obj1 Slot: %f %f %f\n",
		dycGetd(obj1, sx), dycGetd(obj1, sy), dycGetd(obj1, sz));

#if 1
//	n=(1<<24);
	n=(1<<26);
//	n=(1<<28);
	mtx=thFastMutex();

	t=clock();
	for(i=0; i<n; i++)
		tst_dummy();
	dt=clock()-t;
	printf("(dummy) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);

#if 0
	t=clock(); cn1=bgbrng_rdtsc(); cn2=0;
	for(i=0; i<n; i++)
	{
		cn3=bgbrng_rdtsc();
		cn0=cn3-cn1;
		if((cn0<0) || (cn0>10000))continue;
		cn2+=cn0; cn1=cn3;
	}
#endif

	t=clock();
	cn1=bgbrng_rdtsc();
	for(i=0; i<n; i++)
		tst_dummy();
	cn2=bgbrng_rdtsc()-cn1;
	dt=clock()-t;
	printf("(dummy) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);
	printf("%f\n", cn2/(dt/((double)CLOCKS_PER_SEC)));

	t=clock();
	for(i=0; i<n; i++)
		tst_dummy_mtx(mtx);
	dt=clock()-t;
	printf("(mutex) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);


	t=clock();
	for(i=0; i<n; i++)
		dycGetSlotd(obj, "x");
	dt=clock()-t;
	printf("(slot-name) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);

	t=clock();
	for(i=0; i<n; i++)
		dycGetd(obj, sx);
	dt=clock()-t;
	printf("(slot-hdl) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);

	t=clock();
	for(i=0; i<n; i++)
//		dycGetSlotd(obj, "z");
		dycIdxObjSlot(obj, "x");
	dt=clock()-t;
	printf("(idx slot pass) %fs %d, %fns\n",
		dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);

	t=clock();
	for(i=0; i<n; i++)
//		dycGetSlotd(obj, "z");
		dycIdxObjSlot(obj, "z");
	dt=clock()-t;
	printf("(idx slot fail) %fs %d, %f\n",
		dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);
#endif
}

int tst_methods()
{
	dycObject obj, obj1;
	dycClass inf, inf1;
	dycMethod mth, mth1;
	dycSlot sx, sy, sz;
	double f, g;
	int i, j, t, dt, n;

	printf("Test: Methods\n");

	obj=dycNewInitClass("test2", "(dd)v", 3.0, 4.0);

	f=dycCallSigd(obj, "foo", "()d");
	printf("(call-name) Method Ret=%f\n", f);

	mth=dycIdxObjMethod(obj, "foo", "()d");

	f=dycCalld(obj, mth);
	printf("(call-virt) Method Ret=%f\n", f);

	inf1=dycGetClass("iftest0");
	mth1=dycIdxMethod(inf1, "foo", "()d");

	f=dycCalld(obj, mth1);
	printf("(call-iface) Method Ret=%f\n", f);

	mth=dycIdxObjMethod(obj, "bar", "()d");
	mth1=dycIdxMethod(inf1, "bar", "()d");

#if 1
//	n=1<<20;
//	n=(1<<24);
	n=(1<<26);
//	n=(1<<28);

	t=clock();
	for(i=0; i<n; i++)
		dycCallSigd(obj, "foo", "()d");
	dt=clock()-t;
	printf("(call-name) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);

	t=clock();
	for(i=0; i<n; i++)
		dycCalld(obj, mth);
	dt=clock()-t;
	printf("(call-virt-hdl) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);

	t=clock();
	for(i=0; i<n; i++)
		dycCalld(obj, mth1);
	dt=clock()-t;
	printf("(call-iface-hdl) %fs %d, %fns\n", dt/((double)CLOCKS_PER_SEC), dt,
		(dt/((double)CLOCKS_PER_SEC))/n*1000000000);
#endif
}

int main(int argc, char *argv[])
{
//	gc_init(&argc);
	dy_init(&argc);
	thSleep(0);

	tst_initclass();
	tst_basics();

	tst_methods();

	printf("Slot: %d/%d\n", BGBDYC_SlotHashUsed(), BGBDYC_SlotHashSize());
	BGBDYC_DumpSlotHash();

	printf("Minf: %d/%d\n", BGBDYC_MinfHashUsed(), BGBDYC_MinfHashSize());
	BGBDYC_DumpMinfHash();

//	*(int *)-1=-1;

	return(0);
}
