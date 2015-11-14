#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_mdl;
LBXGL_API LBXGL_BrushWorld *lbxgl_brushmesh_world;
LBXGL_API int lbxgl_brushmesh_mode;
LBXGL_API int lbxgl_brushmesh_cmd;

int lbxgl_mapedit_dirty;
int lbxgl_mapedit_lutime;
LBXGL_BrushWorld *lbxgl_mapedit_undo[16];
LBXGL_BrushWorld *lbxgl_mapedit_redo[16];

#if 1
LBXGL_API LBXGL_BrushWorld *MapEdit_CloneWorld(LBXGL_BrushWorld *world)
{
	return(LBXGL_Brush_CloneWorld(world));
}

LBXGL_API void MapEdit_DestroyWorld(LBXGL_BrushWorld *world)
{
	LBXGL_Brush_FreeWorld(world);
}

LBXGL_API void MapEdit_RelinkMesh()
{
	LBXGL_Brush *cur;

	if(!lbxgl_brushmesh_mdl)
		lbxgl_brushmesh_mdl=gctalloc("lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));
	lbxgl_brushmesh_mdl->prim=NULL;
	
	cur=lbxgl_brushmesh_world->brush;
	while(cur)
	{
		if(cur->mesh)
		{
			cur->mesh->next=lbxgl_brushmesh_mdl->prim;
			lbxgl_brushmesh_mdl->prim=cur->mesh;
		}
		cur=cur->next;
	}
}

LBXGL_API void MapEdit_SetDirty()
{
	if(lbxgl_mapedit_dirty &&
			((lbxgl_state->time_f*1000)>(lbxgl_mapedit_lutime+1000)))
		lbxgl_mapedit_dirty=0;

	if(lbxgl_mapedit_dirty)return;
	MapEdit_AddUndo();

	lbxgl_mapedit_dirty=1;
	lbxgl_mapedit_lutime=lbxgl_state->time_f*1000;
}

LBXGL_API void MapEdit_AddUndo()
{
	int i;

	printf("AddUndo\n");

	for(i=0; i<16; i++)
	{
		if(lbxgl_mapedit_redo[i])
			MapEdit_DestroyWorld(lbxgl_mapedit_redo[i]);
		lbxgl_mapedit_redo[i]=NULL;
	}

	if(lbxgl_mapedit_undo[15])
		MapEdit_DestroyWorld(lbxgl_mapedit_undo[15]);
	for(i=15; i>0; i--)
		lbxgl_mapedit_undo[i]=lbxgl_mapedit_undo[i-1];
	lbxgl_mapedit_undo[0]=MapEdit_CloneWorld(lbxgl_brushmesh_world);
	lbxgl_mapedit_dirty=0;
}

LBXGL_API void MapEdit_Undo()
{
	int i;

	if(!lbxgl_mapedit_undo[0])
		return;

	printf("Undo\n");

	MapEdit_ClearSel();

	if(lbxgl_mapedit_redo[15])
		MapEdit_DestroyWorld(lbxgl_mapedit_redo[15]);
	for(i=15; i>0; i--)
		lbxgl_mapedit_redo[i]=lbxgl_mapedit_redo[i-1];

	lbxgl_mapedit_redo[0]=lbxgl_brushmesh_world;
	lbxgl_brushmesh_world=lbxgl_mapedit_undo[0];
	lbxgl_brushmesh_world=lbxgl_brushmesh_world;

	for(i=0; i<15; i++)
		lbxgl_mapedit_undo[i]=lbxgl_mapedit_undo[i+1];
	lbxgl_mapedit_undo[15]=NULL;

	MapEdit_ClearSel();
	MapEdit_RelinkMesh();
}

LBXGL_API void MapEdit_Redo()
{
	int i;

	if(!lbxgl_mapedit_redo[0])
		return;

	printf("Redo\n");

	MapEdit_ClearSel();

	if(lbxgl_mapedit_undo[15])
		MapEdit_DestroyWorld(lbxgl_mapedit_undo[15]);
	for(i=15; i>0; i--)
		lbxgl_mapedit_undo[i]=lbxgl_mapedit_undo[i-1];

	lbxgl_mapedit_undo[0]=lbxgl_brushmesh_world;
	lbxgl_brushmesh_world=lbxgl_mapedit_redo[0];
	lbxgl_brushmesh_world=lbxgl_brushmesh_world;

	for(i=0; i<15; i++)
		lbxgl_mapedit_redo[i]=lbxgl_mapedit_redo[i+1];
	lbxgl_mapedit_redo[15]=NULL;

	MapEdit_ClearSel();
	MapEdit_RelinkMesh();
}

// LBXGL_API void MapEdit_CmdUndo()
//	{ MapEdit_Undo(); }
// LBXGL_API void MapEdit_CmdRedo()
//	{ MapEdit_Redo(); }

//determine if a given mesh is attached to a brush
LBXGL_API int MapEdit_CheckPrimHasBrush(LBXGL_MeshPrim *prim)
{
	LBXGL_Brush *bcur;
	
	bcur=lbxgl_brushmesh_world->brush;
	while(bcur)
	{
		if(bcur->mesh==prim)
			return(1);
		bcur=bcur->next;
	}
	return(0);
}

LBXGL_API void MapEdit_CmdSel(int cmd)
{
	LBXGL_MeshPrim *pcur;
	LBXGL_Brush *brsh;

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_BRUSH)
	{
	}else
	{
		LBXGL_BrushWorld_DrawFlush(lbxgl_brushmesh_world);
		MapEdit_SetDirty();
//		MapEdit_AddUndo();
		LBXGL_BrushMesh_CmdSel(cmd);
		
		pcur=lbxgl_brushmesh_mdl->prim;
		while(pcur)
		{
			//add any new meshes to world
			if(!MapEdit_CheckPrimHasBrush(pcur))
			{
				brsh=LBXGL_BrushMesh_MakeBrushUsingPrim(pcur);
				LBXGL_BrushWorld_AddBrush(
					lbxgl_brushmesh_world, brsh);
			}

			pcur=pcur->next;
		}
	}
}

LBXGL_API void MapEdit_ClearSel()
{
	LBXGL_BrushMesh_ClearSel();
	lbxgl_brushmesh_world->brush_sel=NULL;
	lbxgl_brushmesh_world->entity_sel=NULL;
}
#endif


LBXGL_API int MapEdit_NewWorld(LBXGL_BrushWorld *world)
{
	float v[3];
	LBXGL_Brush *brsh;

	LBXGL_BrushWorld_DrawFlush(world);

//	map_name=NULL;
	world->entity=NULL;
	world->brush=NULL;

	world->entity_psel=LBXGL_SEnt_NewEntity(&world->entity, "worldspawn");

//	world->brush=make_cube_default();

	brsh=LBXGL_Brush_MakeCube(0, 0, -64, 128, 128, 6, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(0, 0, 64, 128, 128, 6, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(-128, 0, 0, 6, 128, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(128, 0, 0, 6, 128, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(0, -128, 0, 128, 6, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	brsh=LBXGL_Brush_MakeCube(0, 128, 0, 128, 6, 64, world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);


	V3F_ZERO(v);
	LBXGL_SEnt_NewBasicLight(&world->entity, v, 1000);

	V3F_SET(v, 0, -100, -64+36);
	LBXGL_SEnt_NewBasicEntity(&world->entity, "info_player_start", v);
	
	return(0);
}

LBXGL_API void MapEdit_CreateCube(float x, float y, float z,
	float xs, float ys, float zs, char *tex)
{
	LBXGL_Brush *tmp;

	tmp=LBXGL_Brush_MakeCubeExtents(
		x-xs, y-ys, z-zs, x+xs, y+ys, z+zs, tex);
	LBXGL_BrushWorld_AddBrush(
		lbxgl_brushmesh_world, tmp);
}

LBXGL_API void MapEdit_CreateRoomExtents(
	float mx, float my, float mz,
	float nx, float ny, float nz,
	char *floortex, char *walltex, char *ceiltex)
{
	LBXGL_Brush_MakeRoomExtents(lbxgl_brushmesh_world,
		mx, my, mz, nx, ny, nz, floortex, walltex, ceiltex);
}

LBXGL_API void MapEdit_CreateRoom(float x, float y, float z,
	float xs, float ys, float zs,
	char *floortex, char *walltex, char *ceiltex)
{
	LBXGL_Brush_MakeRoomExtents(lbxgl_brushmesh_world,
		x-xs, y-ys, z-zs, x+xs, y+ys, z+zs,
		floortex, walltex, ceiltex);
}

LBXGL_API void MapEdit_SubtractBoxExtents(
	float mx, float my, float mz,
	float nx, float ny, float nz)
{
	float mins[3], maxs[3];

	mins[0]=mx; mins[1]=my; mins[2]=mz;
	maxs[0]=nx; maxs[1]=ny; maxs[2]=nz;
	LBXGL_Brush_SubtractBrushBBox(lbxgl_brushmesh_world, mins, maxs);
}

LBXGL_API void MapEdit_SubtractBox(
	float x, float y, float z,
	float xs, float ys, float zs)
{
	float mins[3], maxs[3];

	mins[0]=x-xs; mins[1]=y-ys; mins[2]=z-zs;
	maxs[0]=x+xs; maxs[1]=y+ys; maxs[2]=z+zs;
	LBXGL_Brush_SubtractBrushBBox(lbxgl_brushmesh_world, mins, maxs);
}
