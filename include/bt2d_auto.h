//AHSRC:base/bt2d_base.c
void Tile2D_Init(void);
double bt2dTime(void);
void Tile2D_Reload(void);
void Tile2D_DrawEntity_Sprite(Tile2D_Entity *self);
bool Tile2D_CheckBoxBox(vec2 amins, vec2 amaxs, vec2 bmins, vec2 bmaxs);
void Tile2D_Update_EntityMob(Tile2D_Entity *self, float dt);
void Tile2D_Update_EntityBullet(Tile2D_Entity *self, float dt);
void Tile2D_AnimTick_Bear(Tile2D_Entity *self);
void Tile2D_AnimTick_GiantBear(Tile2D_Entity *self);
void Tile2D_AnimTick_Gib(Tile2D_Entity *self);
void Tile2D_Entity_DieGeneric(Tile2D_Entity *self,Tile2D_Entity *other, float dmg, int mod);
Tile2D_Entity *Tile2D_SpawnEntity_Bear(vec2 org);
Tile2D_Entity *Tile2D_SpawnEntity_GiantBear(vec2 org);
Tile2D_Entity *Tile2D_SpawnEntity_Bullet(vec2 org, vec2 vel, int style);
void Tile2D_DrawEntities();
BT2D_API void Tile2D_DrawWorld();
BT2D_API void Tile2D_UpdateWorld(float dta);
//AHSRC:base/bt2d_bsglue.c
BT2D_API void BT2D_ScriptInit();
int Tile2D_TstFunc();
BT2D_API s32 PdwCToPMxmreJdpEoc(s32 a0);
//AHSRC:base/bt2d_entity.c
bool Tile2D_Entity_CheckBox(Tile2D_Entity *self, vec2 mins, vec2 maxs);
Tile2D_Entity *Tile2D_Entity_QueryBox(vec2 mins, vec2 maxs, Tile2D_Entity *skip, int flags);
Tile2D_Entity *Tile2D_Entity_QueryCollide(Tile2D_Entity *self, int flags);
Tile2D_Entity *Tile2D_Entity_QueryMoveCollide(Tile2D_Entity *self, vec2 org, int flags);
void Tile2D_Entity_Unlink(Tile2D_Entity *self);
void Tile2D_Entity_Delete(Tile2D_Entity *self);
void Tile2D_Entity_Damage(Tile2D_Entity *self,Tile2D_Entity *other, float dmg, int mod);
Tile2D_Entity *Tile2D_AllocEntity(void);
void *Tile2D_SpawnFuncForName(char *name);
void Tile2D_SpawnEntities();
//AHSRC:base/bt2d_move.c
vec2 Tile2D_Update_CheckMove(Tile2D_Entity *self, vec2 dir, float dt);
vec2 Tile2D_Update_CheckMoveFly(Tile2D_Entity *self, float dt);
//AHSRC:base/bt2d_player.c
void Tile2D_AnimTick_Player(Tile2D_Entity *player);
void Tile2D_Update_Player(Tile2D_Entity *player, float dt);
BT2D_API int Tile2D_HandleInput();
void Tile2D_SpawnPlayer(vec2 org);
//AHSRC:base/bt2d_tilemap.c
char *Tile2D_TexNameForColor(byte *clr);
char *Tile2D_TexNameForPoint(float x, float y);
char *Tile2D_TexNameForBox(float x, float y, float xs, float ys);
char *Tile2D_EntityNameForColor(byte *clr);
