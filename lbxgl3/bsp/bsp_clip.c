#include <lbxgl2.h>

//CSG: eliminate internal geometry

BSP_Face *BSP_ClipFacesPlane(BSP_Face *face,
	float *norm)
{
	BSP_Face *cur, *nxt, *fst, *tmp;
	int i, on, in, sn, tn;

//	printf("%f %f %f %f\n", norm[0], norm[1], norm[2], norm[3]);

	on=0; in=0; sn=0; tn=0;

	cur=face;
	fst=NULL;
	while(cur)
	{
#if 1
		if(cur->flags&BSPFACE_FL_OUTSIDE2)
		{
			cur->flags|=BSPFACE_FL_OUTSIDE;

			nxt=cur->next;
			cur->next=fst;
			fst=cur;

			cur=nxt;
			continue;
		}
#endif

		i=BSP_FacePlaneSide2(cur, norm);
		nxt=cur->next;
		tn++;

		switch(i)
		{
		case 0:
		case 1:
			cur->next=fst;
			fst=cur;
			in++;
			break;

		case 2:
			cur->flags|=BSPFACE_FL_OUTSIDE;
			cur->next=fst;
			fst=cur;
			on++;
			break;
		case 3:
			tmp=BSP_FrontPoly(cur, norm);
			tmp->flags|=BSPFACE_FL_OUTSIDE;
			tmp->next=fst;
			fst=tmp;

			tmp=BSP_BackPoly(cur, norm);
//			tmp->flags|=BSPFACE_FL_OUTSIDE;
			tmp->next=fst;
			fst=tmp;

			sn++;
			break;
		default:
			break;
		}

		cur=nxt;
	}

	return(fst);
}

BSP_Face *BSP_ClipFaceBrush(BSP_Face *face,
	BSP_Brush *brush, BSP_Brush *skip)
{
	BSP_Brush *bcur;
	BSP_Face *fcur, *fcur2, *nxt, *ftmp;
	int i, n, ni, no, nt;

	ftmp=face;
	bcur=brush;
	n=0;
	while(bcur)
	{
#if 1
		if((bcur==skip) ||
			(bcur->flags&BSPFACE_FL_NOCSG) ||
			((bcur->flags&BSPFACE_FL_WINDOW) &&
				!(skip->flags&BSPFACE_FL_WINDOW))
			)
		{
			bcur=bcur->next;
			continue;
		}
#endif

		fcur=ftmp;
		while(fcur)
		{
//			fcur->flags=0;
			fcur->flags&=~BSPFACE_FL_OUTSIDE;
			fcur->flags&=~BSPFACE_FL_OUTSIDE2;
			fcur=fcur->next;
		}

#if 1
		//first pass, mark outside faces
		fcur=bcur->face;
		while(fcur)
		{
			fcur2=ftmp;
			while(fcur2)
			{
				i=BSP_FacePlaneSide2(
					fcur2, fcur->norm);
				if(i==2)fcur2->flags|=
					BSPFACE_FL_OUTSIDE2;
				fcur2=fcur2->next;
			}

			fcur=fcur->next;
		}
#endif

		//second pass, clip any unmarked faces
		fcur=bcur->face;
		while(fcur)
		{
			ftmp=BSP_ClipFacesPlane(ftmp,
				fcur->norm);
			fcur=fcur->next;
		}

		ni=0; no=0; nt=0;
		fcur=ftmp;
		while(fcur)
		{
			if(!(fcur->flags&BSPFACE_FL_OUTSIDE))
			{
				ni++;
				fcur->flags|=BSPFACE_FL_INSIDE;
			}else no++;

			nt++;
			fcur=fcur->next;
		}
//		printf("mark %d inside %d outside %d total\n", ni, no, nt);

		n++;
		bcur=bcur->next;
	}

//	printf("clipped by %d brushes\n", n);

	return(ftmp);
}

BSP_Face *BSP_ClipFaces(BSP_Face *faces,
	BSP_Brush *brush, BSP_Brush *skip)
{
	BSP_Face *cur, *ftmp, *lst, *nxt;
	int cif, cof, tf, itf;

//	printf("BSP_ClipFaces\n");

	itf=0;
	lst=NULL;
	cur=faces;
	while(cur)
	{
		ftmp=BSP_DupPoly(cur);
		ftmp->next=lst;
		lst=ftmp;

		itf++;

		cur=cur->next;
	}

//	printf("BSP_ClipFaces: %d input faces\n", tf);

	ftmp=BSP_ClipFaceBrush(lst, brush, skip);

//	printf("BSP_ClipFaces: 1\n");

	cif=0;
	cof=0;
	tf=0;

	lst=NULL;
	cur=ftmp;
	while(cur)
	{
		nxt=cur->next;
		tf++;

		if(!(cur->flags&BSPFACE_FL_INSIDE))
		{
			cof++;
			cur->next=lst;
			lst=cur;
		}else cif++;
		cur=nxt;
	}

//	printf("BSP_ClipFaces: OK, "
//		"%d inner %d outer %d total, %d initial\n",
//			cif, cof, tf, itf);

	return(lst);
}

BSP_Brush *BSP_ClipBrushBrushes(
	BSP_Brush *brush, BSP_Brush *brushes)
{
	BSP_Brush *tmp;
	BSP_Face *lst;

//	printf("BSP_ClipBrushBrushes\n");
	lst=BSP_ClipFaces(brush->face, brushes, brush);

	tmp=kalloc(sizeof(BSP_Brush));
	memset(tmp, 0, sizeof(BSP_Brush));
	tmp->face=lst;
	tmp->flags=brush->flags;

	return(tmp);
}

BSP_Brush *BSP_ClipBrushes(BSP_Brush *brush)
{
	BSP_Brush *cur, *lst, *tmp;
	int i, j;

	printf("BSP_ClipBrushes\n");

	cur=brush; j=0;
	while(cur) { j++; cur=cur->next; }

	lst=NULL; cur=brush; i=0;
	while(cur)
	{
		i++; printf("%d/%d\r", i, j);

		tmp=BSP_ClipBrushBrushes(cur, brush);
		tmp->next=lst;
		lst=tmp;

		cur=cur->next;
	}

	printf("\n");

	printf("BSP_ClipBrushes: OK\n");

	return(lst);
}

