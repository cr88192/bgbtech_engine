#include <bs2asm.h>

void BS2ASM_EntParse_DeclValue(BS2ASM_Context *ctx, char **str,
	BS2ASM_TagEntity *ent, char *key)
{
	char tb0[256], tb1[256];
	BS2ASM_TagEntity *tmp;
	char *s, *s0, *s1;
	int ty0, ty1;
	
	s=*str;
	
	s0=BS2ASM_Parse_Token(s, tb0, &ty0);
	s1=BS2ASM_Parse_Token(s0, tb1, &ty1);
	
	BS2ASM_SetEntityValue(ent, key, tb0);

//	printf("%s=%s ", key, tb0);

	s=s0;
	*str=s;
}

void BS2ASM_EntParse_DeclBlock(BS2ASM_Context *ctx, char **str,
	BS2ASM_TagEntity *ent)
{
	char tb0[256], tb1[256];
	BS2ASM_TagEntity *tmp;
	char *s, *s0, *s1;
	int ty0, ty1;
	
	s=*str;
	
	while(1)
	{
		s=BS2ASM_Parse_EatWhite(s);
		if(!*s)break;
	
		s0=BS2ASM_Parse_Token(s, tb0, &ty0);
		s1=BS2ASM_Parse_Token(s0, tb1, &ty1);

//		printf("<%s %s> ", tb0, tb1);

		if((ty0==BTK_BRACE) && !strcmp(tb0, "}"))
			{ s=s0; break; }

		if((ty0==BTK_NAME) && (ty1==BTK_BRACE) && !strcmp(tb1, "{"))
		{
//			printf("%s { ", tb0);
		
			tmp=BS2ASM_NewTagEntity(ctx, tb0);
			s=s1;
			BS2ASM_EntParse_DeclBlock(ctx, &s, tmp);
			BS2ASM_AddEntityChild(ent, tmp);
			
//			printf(" } ");
			continue;
		}

		if((ty0==BTK_NAME) && (ty1==BTK_OPERATOR) && !strcmp(tb1, "="))
		{
			s=s1;
			BS2ASM_EntParse_DeclValue(ctx, &s, ent, tb0);
			continue;
		}

//		printf(" opcode<%s %s> ", tb0, tb1);
		
		//opcode
		s=BS2ASM_ParseCommand(ctx, ent, s);
	}
	
	*str=s;
}

BS2ASM_TagEntity *BS2ASM_EntParse_Buffer(BS2ASM_Context *ctx, char *str)
{
	BS2ASM_TagEntity *tmp, *tmp1;
	char *s;
	
	BS2ASM_LitIndexUTF8(ctx, "UTF8");

	s=str;
	tmp=BS2ASM_NewTagEntity(ctx, "top");
	BS2ASM_EntParse_DeclBlock(ctx, &s, tmp);
//	tmp->first=tmp1;
	ctx->top=tmp;
	return(tmp);
}
