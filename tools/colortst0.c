#include <stdio.h>
#include <math.h>

int main()
{
	int cr, cg, cb;
	int dr, dg, db;
	int cy, cu, cv, csc;
	int tr, tg, tb, t, t0, t1, t2;
	double er, eg, eb, et;
	
	er=0; eg=0; eb=0;
	for(cr=0; cr<256; cr++)
		for(cg=0; cg<256; cg++)
			for(cb=0; cb<256; cb++)
	{
#if 1	//BLCT1
//		cy=(cr+2*cg+cb)/4;
		cy=cg+(cb+cr-2*cg)/4;
		cu=((cb-cg)/2)+128;
		cv=((cr-cg)/2)+128;
#endif

#if 0	//YCoCg
		cu=cr-cb;
		t=cb+(cu/2);
		cv=cg-t;
		cy=t+(cv/2);
		cu=(cu/2)+128;
		cv=(cv/2)+128;
#endif

#if 0	//BLCT2
		cy=(cr+2*cg+cb)/4;
//		cy=cg+(cb+cr-2*cg)/4;
//		cy=(cr+2*cg+cb+2)/4;
//		cy=cg+(cb+cr-2*cg+2)/4;
		cu=((cb-cr)/2)+128;
		cv=(cr+cb)/2;
//		cy=(cg+cv)/2;
//		cy=2*cg-cv;
#endif

#if 0	//BLCT2-1
		cy=(cr+2*cg+cb)/4;
//		cy=cg+(cb+cr-2*cg)/4;
//		cy=(cr+2*cg+cb+2)/4;
//		cy=cg+(cb+cr-2*cg+2)/4;
		cu=((cb-cr)/2)+128;
		cv=(cr+cb)/2;
		cv=((cv-cy)/2)+128;
//		cy=(cg+cv)/2;
//		cy=2*cg-cv;
#endif

#if 1	//JXR
//		cu=cg-cr-(cb-cr)/2;
		cy=cg-(2*cg-cb-cr)/4;
		cu=(2*cg-cb-cr)/4+128;
		cv=((cb-cr)/2)+128;
//		cv=((cb-cr-1)/2)+128;
//		cv=128-((cr-cb)/2+1);
#endif

#if 0	//JXR2
		cv=cb-cr;
//		cv=cr-cb;
		t=cb+(cu/2);
		cu=cg-t;
		cy=t+(cu/2);
		cu=(cu/2)+128;
		cv=(cv/2)+128;
#endif

#if 1
		t0=cu-128; t1=cv-128;
		t2=t0*t0+t1*t1;
		t=256/sqrt(t2+1);
//		cu=(((cu-128)*t)>>8)+128;
//		cv=(((cv-128)*t)>>8)+128;
//		csc=256/t;
//		csc=sqrt(t2);

		csc=256; t=4;
#if 1
		while((64<cu) && (cu<192) &&
			(64<cv) && (cv<192) && (t>0))
		{
			cu=((cu-128)*2)+128;
			cv=((cv-128)*2)+128;
			csc=csc/2;
			t--;
		}
#endif
#endif

#if 1	//quantize
//		cy=((cy+2)>>2)<<2;
//		cu=((cu+4)>>3)<<3;
//		cv=((cv+4)>>3)<<3;

		cy=cy;
//		cu=((cu+4)>>3)<<3;
//		cv=((cv+2)>>2)<<2;
		cv=((cv+4)>>3)<<3;
		cu=((cu+2)>>2)<<2;

		if(cy<0)cy=0;
		if(cu<0)cu=0;
		if(cv<0)cv=0;

		if(cy>255)cy=255;
		if(cu>255)cu=255;
		if(cv>255)cv=255;
#endif

#if 1
		cu=(((cu-128)*csc+128)>>8)+128;
		cv=(((cv-128)*csc+128)>>8)+128;
#endif


#if 0	//BLCT1
//		dg=(2*cy-2*(cu-128)-2*(cv-128))/4;
		dg=cy-(cu+cv-256)/2;
//		dg=cy-(cu+cv-256)/2.0;
		db=dg+2*(cu-128);
		dr=dg+2*(cv-128);
#endif

#if 0	//YCoCg
		cu=(cu-128)*2;
		cv=(cv-128)*2;
		t=cy-(cv/2);
		dg=cv+t;
		db=t-(cu/2);
		dr=cu+db;
#endif

#if 0	//BLCT2
		dr=cv-(cu-128);
		dg=2*cy-cv;
		db=cv+(cu-128);
#endif

#if 0	//BLCT2-1
		cv=cy+(cv-128);
		dr=cv-(cu-128);
		dg=2*cy-cv;
		db=cv+(cu-128);
#endif

#if 1	//JXR
//		dr=cv-(cu-128);
//		dg=2*cy-cv;
//		db=cv+(cu-128);

//		cy=cg-(2*cg-cb-cr)/4;
//		cu=(2*cg-cb-cr)/4+128;
//		cv=((cb-cr)/2)+128;

		dg=cy+(cu-128);
		dr=dg-(cu-128)*2-(cv-128);
//		db=dr+2*(cv-128);
		db=dg-(cu-128)*2+(cv-128);
#endif

#if 0	//RGB 5:6:5
		dr=((cr+4)>>3)<<3;
		dg=((cg+2)>>2)<<2;
		db=((cb+4)>>3)<<3;
#endif

//		dr=cr;
//		dg=cg;
//		db=cb;

//		dr=cr+1;
//		dg=cg+1;
//		db=cb+1;
		
		tr=dr-cr;
		tg=dg-cg;
		tb=db-cb;
		
		er+=tr*tr;
		eg+=tg*tg;
		eb+=tb*tb;
	}
	
	et=sqrt((er+eg+eb)/(3*16777216.0));
	er=sqrt(er/16777216.0);
	eg=sqrt(eg/16777216.0);
	eb=sqrt(eb/16777216.0);

//	er=sqrt(er)/16777216.0;
//	eg=sqrt(eg)/16777216.0;
//	eb=sqrt(eb)/16777216.0;
	printf("Er=%f Eg=%f Eb=%f Et=%f\n", er, eg, eb, et);
}
