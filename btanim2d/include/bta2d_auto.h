//AHSRC:base/bta2d_api.c
BTA2D_API void BTA2D_Init();
BTA2D_API int BTA2D_DefineTool(int id, char *img);
BTA2D_API void BTA2D_BeginDrawing();
BTA2D_API void BTA2D_EndDrawing();
BTA2D_API void BTA2D_EmitTrianglefv(float *xy0, float *xy1, float *xy2, float *st0, float *st1, float *st2, float *rgba0, float *rgba1, float *rgba2, int tex);
BTA2D_API void BTA2D_EmitQuadfv(float *xy0, float *xy1, float *xy2, float *xy3, float *st0, float *st1, float *st2, float *st3, float *rgba0, float *rgba1, float *rgba2, float *rgba3, int tex);
BTA2D_API void BTA2D_EmitBasicTexQuadf(float mx, float my, float nx, float ny, int tex);
BTA2D_API void BTA2D_EmitBasic2TexQuadf(float ox, float oy, float sx, float sy, int tex);
BTA2D_API void BTA2D_EmitBasicTexClrQuadf(float mx, float my, float nx, float ny, int tex, float cr, float cg, float cb, float ca);
BTA2D_API void BTA2D_EmitBasic2TexClrQuadf(float ox, float oy, float sx, float sy, int tex, float cr, float cg, float cb, float ca);
BTA2D_API BTA2D_EditLayer *BTA2D_AllocEditLayer(int xs, int ys);
BTA2D_API void BTA2D_RedrawCanvas();
BTA2D_API void BTA2D_DrawEmitToolbar();
BTA2D_API void BTA2D_Draw2D();
BTA2D_API int BTA2D_HandleTool();
BTA2D_API int BTA2D_HandleInput();
