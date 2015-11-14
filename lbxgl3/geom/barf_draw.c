#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

void LBXGL_BARF_DrawArraySimpleFlat(LBXGL_BARF_RenderArray *arr)
{
	if(!arr->n_tris)
		return;

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysSimpleFlat(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysSimpleFlat(PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz);
	}
}

void LBXGL_BARF_DrawArrayFlat(LBXGL_BARF_RenderArray *arr)
{
	if(!arr->n_tris)
		return;

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalFlat(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
//			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm);
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlat(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
//			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm);
			3, GL_INT_2_10_10_10_REV, 0, arr->norm);
	}
}

void LBXGL_BARF_DrawArrayFlatRGB(LBXGL_BARF_RenderArray *arr)
{
	if(!arr->n_tris)
		return;

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
//			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
//			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, arr->rgba);
	}
}

void LBXGL_BARF_DrawArrayFlatVLRGB(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn, fl;

	if(!arr->n_tris)
		return;

	if(arr->texflags&LBXGL_TXFL_ALPHA)
	{
		if(!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
			return;
		if(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET))
				return;
	}

	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_vrgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, arr->vrgba);
	}
}

void LBXGL_BARF_DrawArrayLight(
	LBXGL_BARF_RenderArray *arr, LBXGL_Light *light)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!arr->n_tris)
		return;

//	if((arr->texflags&LBXGL_TXFL_ALPHA) &&
//		!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
//			return;

	if(arr->texflags&LBXGL_TXFL_ALPHA)
	{
		if(!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
			return;
		if(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET))
				return;
	}

	LBXGL_Shader_BindTexture(arr->tex);

	if(light->flags&LBXGL_LFL_SUN)
	{
		if(arr->vbo>0)
		{
			pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, 0, arr->n_tris*3,
				3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
				2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//				3, GL_BYTE, 3, (byte *)arr->vbo_norm,
				3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_srgba);
			pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, 0, arr->n_tris*3,
				3, GL_FLOAT, 0, arr->xyz,
				2, GL_FLOAT, 0, arr->st,
//				3, GL_BYTE, 3, arr->norm,
				3, GL_INT_2_10_10_10_REV, 0, arr->norm,
				4, GL_UNSIGNED_BYTE, 0, arr->srgba);
		}
		return;
	}

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, arr->rgba);
	}
}

void LBXGL_BARF_DrawArrayForLight(
	LBXGL_BARF_RenderArray *arr, LBXGL_Light *light)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!arr->n_tris)
		return;

//	if((arr->texflags&LBXGL_TXFL_ALPHA) &&
//		!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
	if(arr->texflags&LBXGL_TXFL_ALPHA)
		return;

	LBXGL_Shader_BindTexture(arr->tex);

	if(light->flags&LBXGL_LFL_SUN)
	{
		if(arr->vbo>0)
		{
			pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, 0, arr->n_tris*3,
				3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
				2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//				3, GL_BYTE, 3, (byte *)arr->vbo_norm,
				3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_srgba);
			pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, 0, arr->n_tris*3,
				3, GL_FLOAT, 0, (byte *)arr->xyz,
				2, GL_FLOAT, 0, (byte *)arr->st,
//				3, GL_BYTE, 3, (byte *)arr->norm,
				3, GL_INT_2_10_10_10_REV, 0, arr->norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)arr->srgba);
		}
		return;
	}

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->xyz,
			2, GL_FLOAT, 0, (byte *)arr->st,
//			3, GL_BYTE, 3, (byte *)arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->rgba);
	}
}

void LBXGL_BARF_DrawArrayStaticLight(
	LBXGL_BARF_RenderArray *arr)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!arr->n_tris)
		return;

#if 0
	if(1)
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		rgba=arr->tlrgba;
		rgba2=(byte *)arr->vbo_tlrgba;

		if(arr->vbo>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo); }

		for(i=0; i<arr->n_tris; i++)
		{
			for(j=i; j<arr->n_tris; j++)
				if(arr->tex[j]!=arr->tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, arr->tex[i]);
			LBXGL_Shader_BindTexture(tn);

			if(arr->vbo>0)
			{
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
					2, GL_FLOAT, 0, (byte *)arr->vbo_st,
					3, GL_BYTE, 3, (byte *)arr->vbo_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba2);
			}else
			{
				LBXGL_Shader_DrawArraysNormalTexRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, (byte *)arr->xyz,
					2, GL_FLOAT, 0, (byte *)arr->st,
//					3, GL_BYTE, 3, (byte *)arr->norm,
					3, GL_INT_2_10_10_10_REV, 0, arr->norm,
					4, GL_UNSIGNED_BYTE, 0, rgba);
			}

			i+=k-1;
		}

		if(arr->vbo>0)
			{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
		return;
	}
#endif
}

void LBXGL_BARF_DrawArrayFinal(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!arr->n_tris)
		return;

//	if((arr->texflags&LBXGL_TXFL_ALPHA) &&
//		!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
//			return;

	if(arr->texflags&LBXGL_TXFL_ALPHA)
	{
		if(!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
			return;
		if(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET))
				return;
	}

	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm);
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm);
			3, GL_INT_2_10_10_10_REV, 0, arr->norm);
	}
}

void LBXGL_BARF_DrawArrayFinalRGB(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!arr->n_tris)
		return;

//	if((arr->texflags&LBXGL_TXFL_ALPHA) &&
//		!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
//			return;

	if(arr->texflags&LBXGL_TXFL_ALPHA)
	{
		if(!(arr->texflags&LBXGL_TXFL_LIGHTMAT))
			return;
		if(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET))
				return;
	}

	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, arr->rgba);
	}
}

void LBXGL_BARF_DrawArrayBasic(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!arr->n_tris)
		return;

	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm);
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm);
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->norm);
	}
}

void LBXGL_BARF_DrawArrayAlpha(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!arr->n_tris)
		return;

//	if(!(arr->texflags&LBXGL_TXFL_ALPHA) ||
//		(arr->texflags&LBXGL_TXFL_LIGHTMAT) ||
//		(arr->texflags&LBXGL_TXFL_FLUID))
//			return;

	if(!(arr->texflags&LBXGL_TXFL_ALPHA) ||
		(arr->texflags&LBXGL_TXFL_FLUID))
			return;
	if(arr->texflags&LBXGL_TXFL_LIGHTMAT)
	{
		if(!(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET)))
				return;
	}

	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm);
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm);
			3, GL_INT_2_10_10_10_REV, 0, arr->norm);
	}
}

void LBXGL_BARF_DrawArrayAlphaRGB(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!arr->n_tris)
		return;

//	if(!(arr->texflags&LBXGL_TXFL_ALPHA) ||
//		(arr->texflags&LBXGL_TXFL_LIGHTMAT) ||
//		(arr->texflags&LBXGL_TXFL_FLUID))
//			return;

	if(!(arr->texflags&LBXGL_TXFL_ALPHA) ||
		(arr->texflags&LBXGL_TXFL_FLUID))
			return;
	if(arr->texflags&LBXGL_TXFL_LIGHTMAT)
	{
		if(!(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET)))
				return;
	}

	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, arr->rgba);
	}
}

void LBXGL_BARF_DrawArrayAlphaFluidFlat(LBXGL_BARF_RenderArray *arr)
{
	if(!arr->n_tris)
		return;

	if(	!(arr->texflags&LBXGL_TXFL_ALPHA) ||
		!(arr->texflags&LBXGL_TXFL_FLUID))
			return;
	if(arr->texflags&LBXGL_TXFL_LIGHTMAT)
	{
		if(!(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET)))
				return;
	}

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalFlat(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalFlat(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm);
	}
}

void LBXGL_BARF_DrawArrayAlphaFluid(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!arr->n_tris)
		return;

	if(	!(arr->texflags&LBXGL_TXFL_ALPHA) ||
		!(arr->texflags&LBXGL_TXFL_FLUID))
			return;
	if(arr->texflags&LBXGL_TXFL_LIGHTMAT)
	{
		if(!(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET)))
				return;
	}

	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, arr->rgba);
	}
}

void LBXGL_BARF_DrawArrayAlphaFluid2(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!arr->n_tris)
		return;

	if(	!(arr->texflags&LBXGL_TXFL_ALPHA) ||
		!(arr->texflags&LBXGL_TXFL_FLUID))
			return;
	if(arr->texflags&LBXGL_TXFL_LIGHTMAT)
	{
		if(!(lbxgl_world->flags&
			(BTGE_WFL_LIGHTBUDGET|BTGE_WFL_ALPHABUDGET)))
				return;
	}
	if(arr->texflags2&BTGE_TXFL2_NOWATEROVERLAY)
	{
		return;
	}

	tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
	LBXGL_Shader_BindTexture(tn);

//	LBXGL_Shader_BindTexture(arr->tex);

	if(arr->vbo>0)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, arr->vbo);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, (byte *)arr->vbo_xyz,
			2, GL_FLOAT, 0, (byte *)arr->vbo_st,
//			3, GL_BYTE, 3, (byte *)arr->vbo_norm,
			3, GL_INT_2_10_10_10_REV, 0, (byte *)arr->vbo_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)arr->vbo_rgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
	}else
	{
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, 0, arr->n_tris*3,
			3, GL_FLOAT, 0, arr->xyz,
			2, GL_FLOAT, 0, arr->st,
//			3, GL_BYTE, 3, arr->norm,
			3, GL_INT_2_10_10_10_REV, 0, arr->norm,
			4, GL_UNSIGNED_BYTE, 0, arr->rgba);
	}
}

#if 0
// void LBXGL_BARF_DrawArrayAlphaFluid2(LBXGL_BARF_RenderArray *arr)
{
	int i, j, k, tn;

	if(!chk->mesh->n_aface)
		return;

#if 1
	tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
	LBXGL_Shader_BindTexture(tn);

	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface); }
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(!BTGE_Voxel_VoxelFluidOverlayP(chk->mesh->aface_vox[i]))
		if(!BTGE_Voxel_VoxelFluidOverlayP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
//		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
//		LBXGL_Shader_BindTexture(tn);

#if 1
		if(chk->mesh->vbo_aface>0)
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_rgba);
		}else
		{
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->aface_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->aface_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->aface_rgba);
		}
#endif

		i+=k-1;
	}
	if(chk->mesh->vbo_aface>0)
		{ pdglBindBuffer(GL_ARRAY_BUFFER, 0); }
#endif
}
#endif




void LBXGL_BARF_DrawContextDark(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayFlatVLRGB(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListFlatVLRGB(world->vox_region);
}

void LBXGL_BARF_DrawContextLight(LBXGL_BARF_RenderContext *ctx,
	LBXGL_Light *light)
{
	int i, j, k;

	if(!ctx)
		return;

	LBXGL_Shader_BindTexture(0);
	pdglColor4f(1, 1, 1, 1);

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayLight(ctx->array[i], light);
	}

//	LBXGL_Voxel_DrawRegionListLight(world->vox_region, light);
}

void LBXGL_BARF_DrawContextShadows(LBXGL_BARF_RenderContext *ctx,
	LBXGL_Light *light)
{
}

void LBXGL_BARF_DrawContextForLight(LBXGL_BARF_RenderContext *ctx,
	LBXGL_Light *light)
{
	int i, j, k;

	if(!ctx)
		return;

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayForLight(ctx->array[i], light);
	}

//	LBXGL_Voxel_DrawRegionListForLight(world->vox_region, light);
}

void LBXGL_BARF_DrawContextPointLight(LBXGL_BARF_RenderContext *ctx)
{
}

void LBXGL_BARF_DrawContextFinal(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	pdglColor4f(1, 1, 1, 1);
	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayFinal(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListFinal(world->vox_region);
}

void LBXGL_BARF_DrawContextStaticLight(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	pdglColor4f(1, 1, 1, 1);
	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayStaticLight(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListStaticLight(world->vox_region);
}

void LBXGL_BARF_DrawContextFlat(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayFlat(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
}

void LBXGL_BARF_DrawContext(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayBasic(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListBasic(world->vox_region);
}

void LBXGL_BARF_DrawContextAlpha(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayAlphaRGB(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListAlphaRGB(world->vox_region);
}

void LBXGL_BARF_DrawContextFluidFlat(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayAlphaFluidFlat(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListAlphaFluidFlat(world->vox_region);
}

void LBXGL_BARF_DrawContextFluid(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayAlphaFluid(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListAlphaFluid(world->vox_region);
}

void LBXGL_BARF_DrawContextFluid2(LBXGL_BARF_RenderContext *ctx)
{
	int i, j, k;

	if(!ctx)
		return;

	for(i=0; i<ctx->n_array; i++)
	{
		LBXGL_BARF_DrawArrayAlphaFluid2(ctx->array[i]);
	}

//	LBXGL_Voxel_DrawRegionListAlphaFluid2(world->vox_region);
}
