struct main_entity_s *main_addsolid(int id)
{
	struct main_entity_s *tmp;
	struct main_entity_s *cur;
	int i;

	tmp=malloc(sizeof(struct main_entity_s));
	memset(tmp, 0, sizeof(struct main_entity_s));
	tmp->solid=id;
	tmp->next=main_ents;
	main_ents=tmp;

	cur=tmp;
	for(i=0; cur; i++)cur=cur->next;
	printf("addsolid: %d entities\n", i);

	return(tmp);
}

void unlink_ents_node(struct bsp_node_s *node)
{
	node->ents=NULL;
	if(node->lnode)unlink_ents_node(node->lnode);
	if(node->rnode)unlink_ents_node(node->rnode);
}

void link_ent_node(struct bsp_node_s *node, struct main_entity_s *ent)
{
	double d;

	if(!node->lnode)
	{
		ent->lnext=node->ents;
		node->ents=ent;
		return;
	}

	d=V3F_DOT(ent->org, node->norm)-node->norm[3];

#if 0
	if(fabs(d)<=1)
	{
		ent->lnext=node->ents;
		node->ents=ent;
		return;
	}
#endif

	if(d<0)
	{
		link_ent_node(node->lnode, ent);
	}else
	{
		link_ent_node(node->rnode, ent);
	}
}

void relink_ents()
{
	struct main_entity_s *cur;
	float d, f, lv, g;
	int i, j;

	unlink_ents_node(main_bsp);

	cur=main_ents;
	while(cur)
	{
		bsdeGetAttrFV(cur->solid, BSDE_ORIGIN, cur->org, 3);
		bsdeGetAttrFV(cur->solid, BSDE_ROTATION_MATRIX, cur->rot, 9);

		cur->gamma=0;
#if 1
		g=6.0;
		for(i=0; i<main_lights; i++)
		{
			lv=main_light_val[i*2+0];
			d=V3F_DIST(main_light_org+i*3, cur->org);
			f=(lv*g)/(d*d+1);
			if(f<4)continue;

			j=pdgl_main_tracenode(main_bsp,
				main_light_org+i*3, cur->org);
			if(j)continue;

			cur->gamma+=f/255.0;
		}

		for(i=0; i<main_dlights; i++)
		{
			lv=main_dlight_val[i*2+0];
			d=V3F_DIST(main_dlight_org+i*3, cur->org);
			f=(lv*g)/(d*d+1);
			if(f<4)continue;

			j=pdgl_main_tracenode(main_bsp,
				main_dlight_org+i*3, cur->org);
			if(j)continue;

			cur->gamma+=f/255.0;
		}

		if(cur->gamma>1)cur->gamma=1;
#endif

		link_ent_node(main_bsp, cur);
		cur=cur->next;
	}

}

void draw_ents(struct main_entity_s *first)
{
	static int fca[256], fva[4*256];
	static float va[3*256];

	struct main_entity_s *cur;
	float im1[4*4], iv[4], mins[3], maxs[3], f, g;
	int i, j, k;
	int nf, nv, tfv;

	cur=first;
	while(cur)
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslatef(cur->org[0], cur->org[1], cur->org[2]);

#if 0
		glColor4f(1, 1, 1, 1);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(cur->rot[0]*2, cur->rot[1]*2, cur->rot[2]*2);
		glVertex3f(0, 0, 0);
		glVertex3f(cur->rot[3]*2, cur->rot[4]*2, cur->rot[5]*2);
		glVertex3f(0, 0, 0);
		glVertex3f(cur->rot[6]*2, cur->rot[7]*2, cur->rot[8]*2);
		glEnd();
#endif

#if 1
		for(i=0; i<3; i++)for(j=0; j<3; j++)
			im1[i*4+j]=cur->rot[i*3+j];
		im1[0*4+3]=0; im1[1*4+3]=0; im1[2*4+3]=0;
		im1[3*4+0]=0; im1[3*4+1]=0; im1[3*4+2]=0;
		im1[3*4+3]=1;
		glMultMatrixf(im1);
#endif

		i=bsdeGetAttrI(cur->solid, BSDE_SOLIDTYPE);
		switch(i)
		{
		case BSDE_SOLID_AABB:
		case BSDE_SOLID_OBB:
			bsdeGetAttrFV(cur->solid, BSDE_MINS, mins, 3);
			bsdeGetAttrFV(cur->solid, BSDE_MAXS, maxs, 3);
			PD3D_DrawCube(mins[0], mins[1], mins[2],
				maxs[0], maxs[1], maxs[2]);
//			PD3D_DrawGammaCube(mins[0], mins[1], mins[2],
//				maxs[0], maxs[1], maxs[2], cur->gamma);
			break;
		case BSDE_SOLID_SPHERE:
			f=bsdeGetAttrF(cur->solid, BSDE_RADIUS);
			PD3D_DrawSphere(0, 0, 0, f);
//			PD3D_DrawGammaSphere(f, cur->gamma);
			break;
		case BSDE_SOLID_CYLINDER:
			f=bsdeGetAttrF(cur->solid, BSDE_RADIUS);
			g=bsdeGetAttrF(cur->solid, BSDE_HEIGHT);
			PD3D_DrawCylinder(0, 0, 0, g, f, f);
			PD3D_DrawDisk(g*0.5, 0, f);
			PD3D_DrawDisk(-g*0.5, 1, f);
			break;
		case BSDE_SOLID_CCYLINDER:
			f=bsdeGetAttrF(cur->solid, BSDE_RADIUS);
			g=bsdeGetAttrF(cur->solid, BSDE_HEIGHT);
			PD3D_DrawCylinder(0, 0, 0, g, f, f);
			PD3D_DrawSphere(0, 0, g*0.5, f);
			PD3D_DrawSphere(0, 0, -g*0.5, f);
			break;

		case BSDE_SOLID_PLANE:
			bsdeGetAttrFV(cur->solid, BSDE_DIRECTION, iv, 4);
			PD3D_DrawPlane(iv[0], iv[1], iv[2], iv[3], 999);
			break;

		case BSDE_SOLID_TRIMESH:
			nf=bsdeGetAttrI(cur->solid, BSDE_NUM_FACES);
			nv=bsdeGetAttrI(cur->solid, BSDE_NUM_VERTICES);

			bsdeGetAttrIV(cur->solid, BSDE_FACE_VERTICES,
				fva, nf*3);
			bsdeGetAttrFV(cur->solid, BSDE_VERTICES, va, nv*3);

			glColor4f(0, 1, 0, 0.75); 

			glBegin(GL_TRIANGLES);
			for(i=0; i<nf; i++)
			{
				glVertex3fv(va+fva[i*3+0]*3);
				glVertex3fv(va+fva[i*3+1]*3);
				glVertex3fv(va+fva[i*3+2]*3);
			}
			glEnd();

			break;

		case BSDE_SOLID_HULL:
			nf=bsdeGetAttrI(cur->solid, BSDE_NUM_FACES);
			nv=bsdeGetAttrI(cur->solid, BSDE_NUM_VERTICES);
			bsdeGetAttrIV(cur->solid, BSDE_FACE_COUNTS, fca, nf);
			tfv=0;
			for(i=0; i<nf; i++)tfv+=fca[i];

			bsdeGetAttrIV(cur->solid, BSDE_FACE_VERTICES,
				fva, tfv);
			bsdeGetAttrFV(cur->solid, BSDE_VERTICES, va, nv*3);

//			glColor4f(1, 0, 0, 0.75); 

			glColor4f((cur->solid&3)/3.0, (cur->solid&12)/12.0,
				(cur->solid&48)/48.0, 0.85);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			k=0;
			for(i=0; i<nf; i++)
			{
				glBegin(GL_POLYGON);
				for(j=0; j<fca[i]; j++)
					glVertex3fv(va+fva[k++]*3);
				glEnd();
			}

			glCullFace(GL_FRONT);

			glColor4f(0, 0, 0, 1);
			k=0;
			for(i=0; i<nf; i++)
			{
				glBegin(GL_POLYGON);
				for(j=0; j<fca[i]; j++)
					glVertex3fv(va+fva[k++]*3);
				glEnd();
			}

			glDisable(GL_CULL_FACE);

//			PD3D_DrawCylinder(0, 0, 0, 2, 1, 1);
//			PD3D_DrawDisk(1, 0, 1);
//			PD3D_DrawDisk(-1, 1, 1);
			break;

		default:
			break;
		}

		glPopMatrix();

		cur=cur->lnext;
	}
}

#if 0
void draw_ents(struct main_entity_s *first)
{
	struct main_entity_s *cur;
	float im1[4*4], iv[4], mins[3], maxs[3], f, g;
	int i, j;

	cur=first;
	while(cur)
	{
		cur=cur->lnext;
	}
}
#endif

void draw_ents2()
{
	struct main_entity_s *cur, *lst;
	int i, j;

	cur=main_ents; lst=NULL;
	while(cur)
	{
		bsdeGetAttrFV(cur->solid, BSDE_ORIGIN, cur->org, 3);
		bsdeGetAttrFV(cur->solid, BSDE_ROTATION_MATRIX, cur->rot, 9);
		cur->gamma=1;

		cur->lnext=lst;
		lst=cur;

		cur=cur->next;
	}

	draw_ents(lst);
}
