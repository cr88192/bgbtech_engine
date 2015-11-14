//gl_win.c
int GfxDev_Key_Event(int key, int down);
void panic(char *str);
void GfxDrv_GetWindowSize(int *xs, int *ys);
void GfxDrv_BeginDrawing();
void GfxDrv_EndDrawing(void);
int GfxDrv_VID_SetWindowedMode();
void GfxDrv_GL_Init();
void GfxDrv_VID_Shutdown();
int GfxDrv_MapKey(int key);
void GfxDrv_SetDefaults(char *label, int width, int height);
void	GfxDrv_Start();
int GfxDrv_Init();
//input.c
int PDGL_AddKeyHandler(int (*fcn)(int key, int down));
int PDGL_KeyDown(int key);
int GfxDev_Key_Event(int key, int down);
int PDGL_TimeMS();
int PDGL_DeltaMS(int *l);
//mouse_win.c
int Mouse_GetRelPos(int *dx, int *dy, int *mb);
int Mouse_GetPos(int *mx, int *my, int *mb);
int Mouse_SetPos(int mx, int my);
//stub_w32.c
int pdgl_main(int argc, char **argv);
int pdgl_main_startup(int argc, char *argv[]);
int pdgl_main_body();
int pdgl_main_shutdown();
void GfxDrv_GetWindowSize(int *xs, int *ys);
void GfxDrv_BeginDrawing();
void GfxDrv_EndDrawing();
int GfxDrv_Init();
void GfxDrv_Start();
void GfxDrv_VID_Shutdown();
int PDGL_DeltaMS(int *l);
int Mouse_GetPos(int *mx, int *my, int *mb);
void panic(char *str);
//prim3d2.c
int PD3D_DrawFlatCube(float minx, float miny, float minz, float maxx, float maxy, float maxz, float r, float g, float b, float a);
int PD3D_DrawFlatLineCube(float minx, float miny, float minz, float maxx, float maxy, float maxz, float r, float g, float b, float a);
int PD3D_DrawCube(float minx, float miny, float minz, float maxx, float maxy, float maxz);
int PD3D_DrawGammaCube(float minx, float miny, float minz, float maxx, float maxy, float maxz, float y);
int PD3D_DrawSphere(float x, float y, float z, float ra);
int PD3D_DrawFlatSphere(float x, float y, float z, float ra,float r, float g, float b, float a);
int PD3D_DrawGammaSphere(float ra, float gg);
float PD3D_DrawGrid(float x, float y, float w, int ax);
float PD3D_DrawXYGrid(float x, float y, float w);
float PD3D_DrawXZGrid(float x, float z, float w);
int PD3D_DrawCylinder(float x, float y, float z, float h, float rt, float rb);
int PD3D_DrawDisk(float z, float h, float rad);
int PD3D_DrawPlane(float x, float y, float z, float d, float w);
//gl_draw.c
int Draw_SetSolid_2D(float aspect, int xo, int yo, int xs, int ys);
int Draw_SetSolid2_2D(float aspect, float size,float *org, float *angles, int xo, int yo, int xs, int ys);
int Draw_SetOrtho_3D(float aspect, float size,float *org, float *angles, int x, int y, int w, int h);
int Draw_SetSolid3_2D(float lxs, float lys,float *org, int xo, int yo, int xs, int ys);
int Draw_SetPerspective_3D(float aspect, float fov, float d,float *org, float *angles, int xo, int yo, int xs, int ys);
int Draw_Square(int x, int y, int w, int h);
int Draw_FillSquare(int x, int y, int w, int h, float c[4]);
int Draw_FillSquareRGBA(int x, int y, int w, int h,int r, int g, int b, int a);
int Draw_Circle(int ox, int oy, int r);
int Draw_FillCircle(int ox, int oy, int ra, int r, int g, int b, int a);
int Draw_PictureCircle(int ox, int oy, int r, int texnum);
int Draw_Bind(int num);
int Draw_DisableTextures();
int Draw_EnableTextures();
int Draw_ColorRGBA(int r, int g, int b, int a);
int Draw_Picture(int x, int y, int w, int h, int num);
