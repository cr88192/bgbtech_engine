#include <bgbfrir.h>
#include <ctype.h>

extern int frir_ops_nops;
extern char *frir_ops_strs[];
extern char *frir_ops_args[];
extern char *frir_ops_sfis[];

FRIR_OpHandlerFcn FRIR_FetchOpcodeHandlerI(int opnum)
{
	char tb[256];
	FRIR_OpHandlerFcn tmp;
	char *s, *t;
	
	if(opnum<0)
		return(NULL);
	if(opnum>=frir_ops_nops)
		return(NULL);

	s=frir_ops_strs[opnum];
	if(!(*s))return(NULL);
	
	strcpy(tb, "FRIR_ThOp_");
	t=tb+strlen(tb);
	while(*s)
	{
		*t++=toupper(*s++);
	}
	*t++=0;
	
	tmp=dyllGetAddr(tb);
	return(tmp);
}

BGBFRIR_API FRIR_OpHandlerFcn FRIR_FetchOpcodeHandler(int opnum)
{
	FRIR_OpHandlerFcn tmp;

	switch(opnum)
	{
	case FRIR_SOP_NOP:		tmp=FRIR_ThOp_NOP; break;
	case FRIR_SOP_ADD_I:	tmp=FRIR_ThOp_ADD_I; break;
	case FRIR_SOP_ADD_L:	tmp=FRIR_ThOp_ADD_L; break;
	case FRIR_SOP_ADD_F:	tmp=FRIR_ThOp_ADD_F; break;
	case FRIR_SOP_ADD_D:	tmp=FRIR_ThOp_ADD_D; break;
	case FRIR_SOP_ADD_A:	tmp=FRIR_ThOp_ADD_A; break;
	case FRIR_SOP_SUB_I:	tmp=FRIR_ThOp_SUB_I; break;
	case FRIR_SOP_SUB_L:	tmp=FRIR_ThOp_SUB_L; break;
	case FRIR_SOP_SUB_F:	tmp=FRIR_ThOp_SUB_F; break;
	case FRIR_SOP_SUB_D:	tmp=FRIR_ThOp_SUB_D; break;
	case FRIR_SOP_SUB_A:	tmp=FRIR_ThOp_SUB_A; break;
	case FRIR_SOP_MUL_I:	tmp=FRIR_ThOp_MUL_I; break;
	case FRIR_SOP_MUL_L:	tmp=FRIR_ThOp_MUL_L; break;
	case FRIR_SOP_MUL_F:	tmp=FRIR_ThOp_MUL_F; break;
	case FRIR_SOP_MUL_D:	tmp=FRIR_ThOp_MUL_D; break;
	case FRIR_SOP_DIV_I:	tmp=FRIR_ThOp_DIV_I; break;
	case FRIR_SOP_DIV_L:	tmp=FRIR_ThOp_DIV_L; break;
	case FRIR_SOP_DIV_F:	tmp=FRIR_ThOp_DIV_F; break;
	case FRIR_SOP_DIV_D:	tmp=FRIR_ThOp_DIV_D; break;
	case FRIR_SOP_MOD_I:	tmp=FRIR_ThOp_MOD_I; break;
	case FRIR_SOP_MOD_L:	tmp=FRIR_ThOp_MOD_L; break;
	case FRIR_SOP_AND_I:	tmp=FRIR_ThOp_AND_I; break;
	case FRIR_SOP_AND_L:	tmp=FRIR_ThOp_AND_L; break;
	case FRIR_SOP_OR_I:		tmp=FRIR_ThOp_OR_I; break;
	case FRIR_SOP_OR_L:		tmp=FRIR_ThOp_OR_L; break;
	case FRIR_SOP_XOR_I:	tmp=FRIR_ThOp_XOR_I; break;
	case FRIR_SOP_XOR_L:	tmp=FRIR_ThOp_XOR_L; break;
	case FRIR_SOP_SHL_I:	tmp=FRIR_ThOp_SHL_I; break;
	case FRIR_SOP_SHL_L:	tmp=FRIR_ThOp_SHL_L; break;
	case FRIR_SOP_SHR_I:	tmp=FRIR_ThOp_SHR_I; break;
	case FRIR_SOP_SHR_L:	tmp=FRIR_ThOp_SHR_L; break;
	case FRIR_SOP_SHRR_I:	tmp=FRIR_ThOp_SHRR_I; break;
	case FRIR_SOP_SHRR_L:	tmp=FRIR_ThOp_SHRR_L; break;

	case FRIR_SOP_NEG_I:	tmp=FRIR_ThOp_NEG_I; break;
	case FRIR_SOP_NEG_L:	tmp=FRIR_ThOp_NEG_L; break;
	case FRIR_SOP_NEG_F:	tmp=FRIR_ThOp_NEG_F; break;
	case FRIR_SOP_NEG_D:	tmp=FRIR_ThOp_NEG_D; break;
	case FRIR_SOP_NOT_I:	tmp=FRIR_ThOp_NOT_I; break;
	case FRIR_SOP_NOT_L:	tmp=FRIR_ThOp_NOT_L; break;

	case FRIR_SOP_MOV_IC:	tmp=FRIR_ThOp_MOV_IC; break;
	case FRIR_SOP_MOV_LC:	tmp=FRIR_ThOp_MOV_LC; break;
	case FRIR_SOP_MOV_FC:	tmp=FRIR_ThOp_MOV_FC; break;
	case FRIR_SOP_MOV_DC:	tmp=FRIR_ThOp_MOV_DC; break;

	case FRIR_SOP_MOV_I:	tmp=FRIR_ThOp_MOV_I; break;
	case FRIR_SOP_MOV_L:	tmp=FRIR_ThOp_MOV_L; break;
	case FRIR_SOP_MOV_F:	tmp=FRIR_ThOp_MOV_F; break;
	case FRIR_SOP_MOV_D:	tmp=FRIR_ThOp_MOV_D; break;
	case FRIR_SOP_MOV_A:	tmp=FRIR_ThOp_MOV_A; break;

	case FRIR_SOP_CONV_I2L:	tmp=FRIR_ThOp_CONV_I2L; break;
	case FRIR_SOP_CONV_I2F:	tmp=FRIR_ThOp_CONV_I2F; break;
	case FRIR_SOP_CONV_I2D:	tmp=FRIR_ThOp_CONV_I2D; break;
	case FRIR_SOP_CONV_I2A:	tmp=FRIR_ThOp_CONV_I2A; break;
	case FRIR_SOP_CONV_L2I:	tmp=FRIR_ThOp_CONV_L2I; break;
	case FRIR_SOP_CONV_L2F:	tmp=FRIR_ThOp_CONV_L2F; break;
	case FRIR_SOP_CONV_L2D:	tmp=FRIR_ThOp_CONV_L2D; break;
	case FRIR_SOP_CONV_L2A:	tmp=FRIR_ThOp_CONV_L2A; break;
	case FRIR_SOP_CONV_F2I:	tmp=FRIR_ThOp_CONV_F2I; break;
	case FRIR_SOP_CONV_F2L:	tmp=FRIR_ThOp_CONV_F2L; break;
	case FRIR_SOP_CONV_F2D:	tmp=FRIR_ThOp_CONV_F2D; break;
	case FRIR_SOP_CONV_D2I:	tmp=FRIR_ThOp_CONV_D2I; break;
	case FRIR_SOP_CONV_D2L:	tmp=FRIR_ThOp_CONV_D2L; break;
	case FRIR_SOP_CONV_D2F:	tmp=FRIR_ThOp_CONV_D2F; break;
	case FRIR_SOP_CONV_A2I:	tmp=FRIR_ThOp_CONV_A2I; break;
	case FRIR_SOP_CONV_A2L:	tmp=FRIR_ThOp_CONV_A2L; break;

	case FRIR_SOP_LEA_B:	tmp=FRIR_ThOp_LEA_B; break;
	case FRIR_SOP_LEA_S:	tmp=FRIR_ThOp_LEA_S; break;
	case FRIR_SOP_LEA_I:	tmp=FRIR_ThOp_LEA_I; break;
	case FRIR_SOP_LEA_L:	tmp=FRIR_ThOp_LEA_L; break;
	case FRIR_SOP_LEA_F:	tmp=FRIR_ThOp_LEA_F; break;
	case FRIR_SOP_LEA_D:	tmp=FRIR_ThOp_LEA_D; break;
	case FRIR_SOP_LEA_A:	tmp=FRIR_ThOp_LEA_A; break;
	case FRIR_SOP_LEA2_B:	tmp=FRIR_ThOp_LEA2_B; break;
	case FRIR_SOP_LEA2_S:	tmp=FRIR_ThOp_LEA2_S; break;
	case FRIR_SOP_LEA2_I:	tmp=FRIR_ThOp_LEA2_I; break;
	case FRIR_SOP_LEA2_L:	tmp=FRIR_ThOp_LEA2_L; break;
	case FRIR_SOP_LEA2_F:	tmp=FRIR_ThOp_LEA2_F; break;
	case FRIR_SOP_LEA2_D:	tmp=FRIR_ThOp_LEA2_D; break;
	case FRIR_SOP_LEA2_A:	tmp=FRIR_ThOp_LEA2_A; break;

	case FRIR_SOP_LLLOAD_I:		tmp=FRIR_ThOp_LLLOAD_I; break;
	case FRIR_SOP_LLLOAD_L:		tmp=FRIR_ThOp_LLLOAD_L; break;
	case FRIR_SOP_LLLOAD_F:		tmp=FRIR_ThOp_LLLOAD_F; break;
	case FRIR_SOP_LLLOAD_D:		tmp=FRIR_ThOp_LLLOAD_D; break;
	case FRIR_SOP_LLLOAD_A:		tmp=FRIR_ThOp_LLLOAD_A; break;
	case FRIR_SOP_LLSTORE_I:	tmp=FRIR_ThOp_LLSTORE_I; break;
	case FRIR_SOP_LLSTORE_L:	tmp=FRIR_ThOp_LLSTORE_L; break;
	case FRIR_SOP_LLSTORE_F:	tmp=FRIR_ThOp_LLSTORE_F; break;
	case FRIR_SOP_LLSTORE_D:	tmp=FRIR_ThOp_LLSTORE_D; break;
	case FRIR_SOP_LLSTORE_A:	tmp=FRIR_ThOp_LLSTORE_A; break;
	case FRIR_SOP_LALOAD_I:		tmp=FRIR_ThOp_LALOAD_I; break;
	case FRIR_SOP_LALOAD_L:		tmp=FRIR_ThOp_LALOAD_L; break;
	case FRIR_SOP_LALOAD_F:		tmp=FRIR_ThOp_LALOAD_F; break;
	case FRIR_SOP_LALOAD_D:		tmp=FRIR_ThOp_LALOAD_D; break;
	case FRIR_SOP_LALOAD_A:		tmp=FRIR_ThOp_LALOAD_A; break;
	case FRIR_SOP_LASTORE_I:	tmp=FRIR_ThOp_LASTORE_I; break;
	case FRIR_SOP_LASTORE_L:	tmp=FRIR_ThOp_LASTORE_L; break;
	case FRIR_SOP_LASTORE_F:	tmp=FRIR_ThOp_LASTORE_F; break;
	case FRIR_SOP_LASTORE_D:	tmp=FRIR_ThOp_LASTORE_D; break;
	case FRIR_SOP_LASTORE_A:	tmp=FRIR_ThOp_LASTORE_A; break;
	case FRIR_SOP_LXLOAD_I:		tmp=FRIR_ThOp_LXLOAD_I; break;
	case FRIR_SOP_LXLOAD_L:		tmp=FRIR_ThOp_LXLOAD_L; break;
	case FRIR_SOP_LXLOAD_F:		tmp=FRIR_ThOp_LXLOAD_F; break;
	case FRIR_SOP_LXLOAD_D:		tmp=FRIR_ThOp_LXLOAD_D; break;
	case FRIR_SOP_LXLOAD_A:		tmp=FRIR_ThOp_LXLOAD_A; break;
	case FRIR_SOP_LXSTORE_I:	tmp=FRIR_ThOp_LXSTORE_I; break;
	case FRIR_SOP_LXSTORE_L:	tmp=FRIR_ThOp_LXSTORE_L; break;
	case FRIR_SOP_LXSTORE_F:	tmp=FRIR_ThOp_LXSTORE_F; break;
	case FRIR_SOP_LXSTORE_D:	tmp=FRIR_ThOp_LXSTORE_D; break;
	case FRIR_SOP_LXSTORE_A:	tmp=FRIR_ThOp_LXSTORE_A; break;

	case FRIR_SOP_CMP_LT_I:		tmp=FRIR_ThOp_CMP_LT_I; break;
	case FRIR_SOP_CMP_LT_L:		tmp=FRIR_ThOp_CMP_LT_L; break;
	case FRIR_SOP_CMP_LT_F:		tmp=FRIR_ThOp_CMP_LT_F; break;
	case FRIR_SOP_CMP_LT_D:		tmp=FRIR_ThOp_CMP_LT_D; break;
	case FRIR_SOP_CMP_LT_A:		tmp=FRIR_ThOp_CMP_LT_A; break;
	case FRIR_SOP_CMP_GT_I:		tmp=FRIR_ThOp_CMP_GT_I; break;
	case FRIR_SOP_CMP_GT_L:		tmp=FRIR_ThOp_CMP_GT_L; break;
	case FRIR_SOP_CMP_GT_F:		tmp=FRIR_ThOp_CMP_GT_F; break;
	case FRIR_SOP_CMP_GT_D:		tmp=FRIR_ThOp_CMP_GT_D; break;
	case FRIR_SOP_CMP_GT_A:		tmp=FRIR_ThOp_CMP_GT_A; break;
	case FRIR_SOP_CMP_LE_I:		tmp=FRIR_ThOp_CMP_LE_I; break;
	case FRIR_SOP_CMP_LE_L:		tmp=FRIR_ThOp_CMP_LE_L; break;
	case FRIR_SOP_CMP_LE_F:		tmp=FRIR_ThOp_CMP_LE_F; break;
	case FRIR_SOP_CMP_LE_D:		tmp=FRIR_ThOp_CMP_LE_D; break;
	case FRIR_SOP_CMP_LE_A:		tmp=FRIR_ThOp_CMP_LE_A; break;
	case FRIR_SOP_CMP_GE_I:		tmp=FRIR_ThOp_CMP_GE_I; break;
	case FRIR_SOP_CMP_GE_L:		tmp=FRIR_ThOp_CMP_GE_L; break;
	case FRIR_SOP_CMP_GE_F:		tmp=FRIR_ThOp_CMP_GE_F; break;
	case FRIR_SOP_CMP_GE_D:		tmp=FRIR_ThOp_CMP_GE_D; break;
	case FRIR_SOP_CMP_GE_A:		tmp=FRIR_ThOp_CMP_GE_A; break;
	case FRIR_SOP_CMP_EQ_I:		tmp=FRIR_ThOp_CMP_EQ_I; break;
	case FRIR_SOP_CMP_EQ_L:		tmp=FRIR_ThOp_CMP_EQ_L; break;
	case FRIR_SOP_CMP_EQ_F:		tmp=FRIR_ThOp_CMP_EQ_F; break;
	case FRIR_SOP_CMP_EQ_D:		tmp=FRIR_ThOp_CMP_EQ_D; break;
	case FRIR_SOP_CMP_EQ_A:		tmp=FRIR_ThOp_CMP_EQ_A; break;
	case FRIR_SOP_CMP_NE_I:		tmp=FRIR_ThOp_CMP_NE_I; break;
	case FRIR_SOP_CMP_NE_L:		tmp=FRIR_ThOp_CMP_NE_L; break;
	case FRIR_SOP_CMP_NE_F:		tmp=FRIR_ThOp_CMP_NE_F; break;
	case FRIR_SOP_CMP_NE_D:		tmp=FRIR_ThOp_CMP_NE_D; break;
	case FRIR_SOP_CMP_NE_A:		tmp=FRIR_ThOp_CMP_NE_A; break;

	case FRIR_SOP_CMP_LT_IC:	tmp=FRIR_ThOp_CMP_LT_IC; break;
	case FRIR_SOP_CMP_LT_LC:	tmp=FRIR_ThOp_CMP_LT_LC; break;
	case FRIR_SOP_CMP_LT_FC:	tmp=FRIR_ThOp_CMP_LT_FC; break;
	case FRIR_SOP_CMP_LT_DC:	tmp=FRIR_ThOp_CMP_LT_DC; break;
	case FRIR_SOP_CMP_GT_IC:	tmp=FRIR_ThOp_CMP_GT_IC; break;
	case FRIR_SOP_CMP_GT_LC:	tmp=FRIR_ThOp_CMP_GT_LC; break;
	case FRIR_SOP_CMP_GT_FC:	tmp=FRIR_ThOp_CMP_GT_FC; break;
	case FRIR_SOP_CMP_GT_DC:	tmp=FRIR_ThOp_CMP_GT_DC; break;
	case FRIR_SOP_CMP_LE_IC:	tmp=FRIR_ThOp_CMP_LE_IC; break;
	case FRIR_SOP_CMP_LE_LC:	tmp=FRIR_ThOp_CMP_LE_LC; break;
	case FRIR_SOP_CMP_LE_FC:	tmp=FRIR_ThOp_CMP_LE_FC; break;
	case FRIR_SOP_CMP_LE_DC:	tmp=FRIR_ThOp_CMP_LE_DC; break;
	case FRIR_SOP_CMP_GE_IC:	tmp=FRIR_ThOp_CMP_GE_IC; break;
	case FRIR_SOP_CMP_GE_LC:	tmp=FRIR_ThOp_CMP_GE_LC; break;
	case FRIR_SOP_CMP_GE_FC:	tmp=FRIR_ThOp_CMP_GE_FC; break;
	case FRIR_SOP_CMP_GE_DC:	tmp=FRIR_ThOp_CMP_GE_DC; break;
	case FRIR_SOP_CMP_EQ_IC:	tmp=FRIR_ThOp_CMP_EQ_IC; break;
	case FRIR_SOP_CMP_EQ_LC:	tmp=FRIR_ThOp_CMP_EQ_LC; break;
	case FRIR_SOP_CMP_EQ_FC:	tmp=FRIR_ThOp_CMP_EQ_FC; break;
	case FRIR_SOP_CMP_EQ_DC:	tmp=FRIR_ThOp_CMP_EQ_DC; break;
	case FRIR_SOP_CMP_NE_IC:	tmp=FRIR_ThOp_CMP_NE_IC; break;
	case FRIR_SOP_CMP_NE_LC:	tmp=FRIR_ThOp_CMP_NE_LC; break;
	case FRIR_SOP_CMP_NE_FC:	tmp=FRIR_ThOp_CMP_NE_FC; break;
	case FRIR_SOP_CMP_NE_DC:	tmp=FRIR_ThOp_CMP_NE_DC; break;

	case FRIR_SOP_ADD_IC:	tmp=FRIR_ThOp_ADD_IC; break;
	case FRIR_SOP_ADD_LC:	tmp=FRIR_ThOp_ADD_LC; break;
	case FRIR_SOP_SUB_IC:	tmp=FRIR_ThOp_SUB_IC; break;
	case FRIR_SOP_SUB_LC:	tmp=FRIR_ThOp_SUB_LC; break;
	case FRIR_SOP_MUL_IC:	tmp=FRIR_ThOp_MUL_IC; break;
	case FRIR_SOP_MUL_LC:	tmp=FRIR_ThOp_MUL_LC; break;
	case FRIR_SOP_DIV_IC:	tmp=FRIR_ThOp_DIV_IC; break;
	case FRIR_SOP_DIV_LC:	tmp=FRIR_ThOp_DIV_LC; break;
	case FRIR_SOP_MOD_IC:	tmp=FRIR_ThOp_MOD_IC; break;
	case FRIR_SOP_MOD_LC:	tmp=FRIR_ThOp_MOD_LC; break;
	case FRIR_SOP_AND_IC:	tmp=FRIR_ThOp_AND_IC; break;
	case FRIR_SOP_AND_LC:	tmp=FRIR_ThOp_AND_LC; break;
	case FRIR_SOP_OR_IC:	tmp=FRIR_ThOp_OR_IC; break;
	case FRIR_SOP_OR_LC:	tmp=FRIR_ThOp_OR_LC; break;
	case FRIR_SOP_XOR_IC:	tmp=FRIR_ThOp_XOR_IC; break;
	case FRIR_SOP_XOR_LC:	tmp=FRIR_ThOp_XOR_LC; break;
	case FRIR_SOP_SHL_IC:	tmp=FRIR_ThOp_SHL_IC; break;
	case FRIR_SOP_SHL_LC:	tmp=FRIR_ThOp_SHL_LC; break;
	case FRIR_SOP_SHR_IC:	tmp=FRIR_ThOp_SHR_IC; break;
	case FRIR_SOP_SHR_LC:	tmp=FRIR_ThOp_SHR_LC; break;
	case FRIR_SOP_SHRR_IC:	tmp=FRIR_ThOp_SHRR_IC; break;
	case FRIR_SOP_SHRR_LC:	tmp=FRIR_ThOp_SHRR_LC; break;

	default:
		tmp=NULL; break;
	}

	if(tmp)return(tmp);

	tmp=FRIR_FetchOpcodeHandlerI(opnum);
	return(tmp);
}

FRIR_TraceEndFcn FRIR_FetchOpcodeEndHandlerI(int opnum)
{
	char tb[256];
	FRIR_TraceEndFcn tmp;
	char *s, *t;
	
	if(opnum<0)
		return(NULL);
	if(opnum>=frir_ops_nops)
		return(NULL);

	s=frir_ops_strs[opnum];
	if(!(*s))return(NULL);
	
	strcpy(tb, "FRIR_ThOpE_");
	t=tb+strlen(tb);
	while(*s)
	{
		*t++=toupper(*s++);
	}
	*t++=0;
	
	tmp=dyllGetAddr(tb);
	return(tmp);
}

BGBFRIR_API FRIR_TraceEndFcn FRIR_FetchOpcodeEndHandler(int opnum)
{
	FRIR_TraceEndFcn tmp;

	switch(opnum)
	{
	case FRIR_SOP_NOP:			tmp=FRIR_ThOpE_NOP; break;

	case FRIR_SOP_JMP_LT_I:		tmp=FRIR_ThOpE_JMP_LT_I; break;
	case FRIR_SOP_JMP_LT_L:		tmp=FRIR_ThOpE_JMP_LT_L; break;
	case FRIR_SOP_JMP_LT_F:		tmp=FRIR_ThOpE_JMP_LT_F; break;
	case FRIR_SOP_JMP_LT_D:		tmp=FRIR_ThOpE_JMP_LT_D; break;
	case FRIR_SOP_JMP_LT_A:		tmp=FRIR_ThOpE_JMP_LT_A; break;
	case FRIR_SOP_JMP_GT_I:		tmp=FRIR_ThOpE_JMP_GT_I; break;
	case FRIR_SOP_JMP_GT_L:		tmp=FRIR_ThOpE_JMP_GT_L; break;
	case FRIR_SOP_JMP_GT_F:		tmp=FRIR_ThOpE_JMP_GT_F; break;
	case FRIR_SOP_JMP_GT_D:		tmp=FRIR_ThOpE_JMP_GT_D; break;
	case FRIR_SOP_JMP_GT_A:		tmp=FRIR_ThOpE_JMP_GT_A; break;
	case FRIR_SOP_JMP_LE_I:		tmp=FRIR_ThOpE_JMP_LE_I; break;
	case FRIR_SOP_JMP_LE_L:		tmp=FRIR_ThOpE_JMP_LE_L; break;
	case FRIR_SOP_JMP_LE_F:		tmp=FRIR_ThOpE_JMP_LE_F; break;
	case FRIR_SOP_JMP_LE_D:		tmp=FRIR_ThOpE_JMP_LE_D; break;
	case FRIR_SOP_JMP_LE_A:		tmp=FRIR_ThOpE_JMP_LE_A; break;
	case FRIR_SOP_JMP_GE_I:		tmp=FRIR_ThOpE_JMP_GE_I; break;
	case FRIR_SOP_JMP_GE_L:		tmp=FRIR_ThOpE_JMP_GE_L; break;
	case FRIR_SOP_JMP_GE_F:		tmp=FRIR_ThOpE_JMP_GE_F; break;
	case FRIR_SOP_JMP_GE_D:		tmp=FRIR_ThOpE_JMP_GE_D; break;
	case FRIR_SOP_JMP_GE_A:		tmp=FRIR_ThOpE_JMP_GE_A; break;
	case FRIR_SOP_JMP_EQ_I:		tmp=FRIR_ThOpE_JMP_EQ_I; break;
	case FRIR_SOP_JMP_EQ_L:		tmp=FRIR_ThOpE_JMP_EQ_L; break;
	case FRIR_SOP_JMP_EQ_F:		tmp=FRIR_ThOpE_JMP_EQ_F; break;
	case FRIR_SOP_JMP_EQ_D:		tmp=FRIR_ThOpE_JMP_EQ_D; break;
	case FRIR_SOP_JMP_EQ_A:		tmp=FRIR_ThOpE_JMP_EQ_A; break;
	case FRIR_SOP_JMP_NE_I:		tmp=FRIR_ThOpE_JMP_NE_I; break;
	case FRIR_SOP_JMP_NE_L:		tmp=FRIR_ThOpE_JMP_NE_L; break;
	case FRIR_SOP_JMP_NE_F:		tmp=FRIR_ThOpE_JMP_NE_F; break;
	case FRIR_SOP_JMP_NE_D:		tmp=FRIR_ThOpE_JMP_NE_D; break;
	case FRIR_SOP_JMP_NE_A:		tmp=FRIR_ThOpE_JMP_NE_A; break;
	case FRIR_SOP_JMP_LT_IC:	tmp=FRIR_ThOpE_JMP_LT_IC; break;
	case FRIR_SOP_JMP_LT_LC:	tmp=FRIR_ThOpE_JMP_LT_LC; break;
	case FRIR_SOP_JMP_LT_FC:	tmp=FRIR_ThOpE_JMP_LT_FC; break;
	case FRIR_SOP_JMP_LT_DC:	tmp=FRIR_ThOpE_JMP_LT_DC; break;
	case FRIR_SOP_JMP_GT_IC:	tmp=FRIR_ThOpE_JMP_GT_IC; break;
	case FRIR_SOP_JMP_GT_LC:	tmp=FRIR_ThOpE_JMP_GT_LC; break;
	case FRIR_SOP_JMP_GT_FC:	tmp=FRIR_ThOpE_JMP_GT_FC; break;
	case FRIR_SOP_JMP_GT_DC:	tmp=FRIR_ThOpE_JMP_GT_DC; break;
	case FRIR_SOP_JMP_LE_IC:	tmp=FRIR_ThOpE_JMP_LE_IC; break;
	case FRIR_SOP_JMP_LE_LC:	tmp=FRIR_ThOpE_JMP_LE_LC; break;
	case FRIR_SOP_JMP_LE_FC:	tmp=FRIR_ThOpE_JMP_LE_FC; break;
	case FRIR_SOP_JMP_LE_DC:	tmp=FRIR_ThOpE_JMP_LE_DC; break;
	case FRIR_SOP_JMP_GE_IC:	tmp=FRIR_ThOpE_JMP_GE_IC; break;
	case FRIR_SOP_JMP_GE_LC:	tmp=FRIR_ThOpE_JMP_GE_LC; break;
	case FRIR_SOP_JMP_GE_FC:	tmp=FRIR_ThOpE_JMP_GE_FC; break;
	case FRIR_SOP_JMP_GE_DC:	tmp=FRIR_ThOpE_JMP_GE_DC; break;
	case FRIR_SOP_JMP_EQ_IC:	tmp=FRIR_ThOpE_JMP_EQ_IC; break;
	case FRIR_SOP_JMP_EQ_LC:	tmp=FRIR_ThOpE_JMP_EQ_LC; break;
	case FRIR_SOP_JMP_EQ_FC:	tmp=FRIR_ThOpE_JMP_EQ_FC; break;
	case FRIR_SOP_JMP_EQ_DC:	tmp=FRIR_ThOpE_JMP_EQ_DC; break;
	case FRIR_SOP_JMP_NE_IC:	tmp=FRIR_ThOpE_JMP_NE_IC; break;
	case FRIR_SOP_JMP_NE_LC:	tmp=FRIR_ThOpE_JMP_NE_LC; break;
	case FRIR_SOP_JMP_NE_FC:	tmp=FRIR_ThOpE_JMP_NE_FC; break;
	case FRIR_SOP_JMP_NE_DC:	tmp=FRIR_ThOpE_JMP_NE_DC; break;

	case FRIR_SOP_JMP_LT_IZ:	tmp=FRIR_ThOpE_JMP_LT_IZ; break;
	case FRIR_SOP_JMP_GT_IZ:	tmp=FRIR_ThOpE_JMP_GT_IZ; break;
	case FRIR_SOP_JMP_LE_IZ:	tmp=FRIR_ThOpE_JMP_LE_IZ; break;
	case FRIR_SOP_JMP_GE_IZ:	tmp=FRIR_ThOpE_JMP_GE_IZ; break;
	case FRIR_SOP_JMP_EQ_IZ:	tmp=FRIR_ThOpE_JMP_EQ_IZ; break;
	case FRIR_SOP_JMP_NE_IZ:	tmp=FRIR_ThOpE_JMP_NE_IZ; break;

	case FRIR_SOP_RET:			tmp=FRIR_ThOpE_RET; break;
	case FRIR_SOP_RETV:			tmp=FRIR_ThOpE_RETV; break;

	default:
		tmp=NULL; break;
	}
	if(tmp)return(tmp);

	tmp=FRIR_FetchOpcodeEndHandlerI(opnum);
	return(tmp);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_FindTargetForLabel(
	FRIR_Trace *tr, int lbl)
{
	FRIR_Method *mth;
	FRIR_Trace *tr1;
	int i;
	
	mth=tr->method;
	for(i=0; i<mth->n_trace; i++)
	{
		tr1=mth->trace[i];
		if(tr1->label==lbl)
			return(tr1);
	}
	return(NULL);
}

//default end function for trace
BGBFRIR_API FRIR_Trace *FRIR_Trace_EndDefault(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
//	op->fcn(ctx, op);
	return(tr->next);
}

//default end function for trace (unconditional jump)
BGBFRIR_API FRIR_Trace *FRIR_Trace_EndJumpDefault(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_EndDefault_BadOpcode(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	ctx->ret=FRIR_RET_BADOPCODE;
	return(tr->next);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_EndJumpDefInit(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	FRIR_Trace *tr1;

//	tr1=FRIR_Trace_FindTargetForLabel(tr, op->i.i);
	tr1=FRIR_Trace_FindTargetForLabel(tr, tr->jmp_label);
	if(tr1)
	{
		tr->jmpnext=tr1;
		
		switch(tr->n_ops)
		{
		case 1: tr->run=FRIR_Trace_RunDefaultJ1; break;
		case 2: tr->run=FRIR_Trace_RunDefaultJ2; break;
		case 3: tr->run=FRIR_Trace_RunDefaultJ3; break;
		case 4: tr->run=FRIR_Trace_RunDefaultJ4; break;
		case 5: tr->run=FRIR_Trace_RunDefaultJ5; break;
		case 6: tr->run=FRIR_Trace_RunDefaultJ6; break;
		case 7: tr->run=FRIR_Trace_RunDefaultJ7; break;
		case 8: tr->run=FRIR_Trace_RunDefaultJ8; break;
		default:
			tr->run=FRIR_Trace_RunDefaultJmp;
			tr->end=FRIR_Trace_EndJumpDefault;
			break;
		}
		
		tr->end=FRIR_Trace_EndJumpDefault;
		
		return(tr1);
	}

	//error
	ctx->ret=FRIR_RET_BADJUMP;
	return(tr->next);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_EndJccDefInit(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	FRIR_Trace *tr1;

//	tr1=FRIR_Trace_FindTargetForLabel(tr, op->i.i);
	tr1=FRIR_Trace_FindTargetForLabel(tr, tr->jmp_label);
	if(!tr1)
	{
		//error
		ctx->ret=FRIR_RET_BADJUMP;
		return(tr->next);
	}
	
	
	tr->jmpnext=tr1;
		
	switch(tr->n_ops)
	{
	case 1: tr->run=FRIR_Trace_RunDefaultJcc1; break;
	case 2: tr->run=FRIR_Trace_RunDefaultJcc2; break;
	case 3: tr->run=FRIR_Trace_RunDefaultJcc3; break;
	case 4: tr->run=FRIR_Trace_RunDefaultJcc4; break;
	case 5: tr->run=FRIR_Trace_RunDefaultJcc5; break;
	case 6: tr->run=FRIR_Trace_RunDefaultJcc6; break;
	case 7: tr->run=FRIR_Trace_RunDefaultJcc7; break;
	case 8: tr->run=FRIR_Trace_RunDefaultJcc8; break;
	default:
		tr->run=FRIR_Trace_RunDefaultJcc;
//		tr->end=FRIR_Trace_EndJccDefault;
		break;
	}
			
	tr->end=FRIR_FetchOpcodeEndHandler(op->op);
	if(!tr->end)
	{
		tr->end=FRIR_Trace_EndDefault_BadOpcode;
		ctx->ret=FRIR_RET_BADOPCODE;
		return(tr->next);
	}
		
	tr1=tr->end(ctx, tr, op);
	return(tr1);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_EndRetDefInit(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	FRIR_Trace *tr1;

	switch(tr->n_ops)
	{
	case 1: tr->run=FRIR_Trace_RunDefaultJ1; break;
	case 2: tr->run=FRIR_Trace_RunDefaultJ2; break;
	case 3: tr->run=FRIR_Trace_RunDefaultJ3; break;
	case 4: tr->run=FRIR_Trace_RunDefaultJ4; break;
	case 5: tr->run=FRIR_Trace_RunDefaultJ5; break;
	case 6: tr->run=FRIR_Trace_RunDefaultJ6; break;
	case 7: tr->run=FRIR_Trace_RunDefaultJ7; break;
	case 8: tr->run=FRIR_Trace_RunDefaultJ8; break;
	default:
		tr->run=FRIR_Trace_RunDefaultJmp;
		tr->end=FRIR_Trace_EndJumpDefault;
		break;
	}
		
	tr->end=FRIR_FetchOpcodeEndHandler(op->op);
	if(!tr->end)
	{
		tr->end=FRIR_Trace_EndDefault_BadOpcode;
		ctx->ret=FRIR_RET_BADOPCODE;
		return(tr->next);
	}
		
	tr1=tr->end(ctx, tr, op);
	return(tr1);
}

//default run function for trace
BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op, *ope;

	op=tr->ops;
	ope=tr->ops+tr->n_ops;
	while(op<ope)
		{ op->fcn(ctx, op); op++; }
	return(tr->end(ctx, tr, op-1));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJmp(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op, *ope;

	op=tr->ops;
	ope=tr->ops+tr->n_ops-1;
	while(op<ope)
		{ op->fcn(ctx, op); op++; }
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op, *ope;

	op=tr->ops;
	ope=tr->ops+tr->n_ops-1;
	while(op<ope)
		{ op->fcn(ctx, op); op++; }
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault1(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault2(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault3(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault4(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault5(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault6(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault7(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefault8(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op);
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ1(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ2(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op);
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ3(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op);
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ4(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op);
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ5(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op);
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ6(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op);
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ7(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op);
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJ8(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op);
	return(tr->jmpnext);
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc1(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	return(tr->end(ctx, tr, tr->ops));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc2(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc3(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc4(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc5(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc6(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc7(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++; op->fcn(ctx, op); op++;
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API FRIR_Trace *FRIR_Trace_RunDefaultJcc8(
	FRIR_Context *ctx, FRIR_Trace *tr)
{
	FRIR_Opcode *op;	op=tr->ops;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;	op->fcn(ctx, op); op++;
	op->fcn(ctx, op); op++;
	return(tr->end(ctx, tr, op));
}

BGBFRIR_API void FRIR_Thread_RunStep(FRIR_Context *ctx)
{
	FRIR_Trace *tr;

	tr=ctx->trace;
	while(!ctx->ret)
		{ tr=tr->run(ctx, tr); }
	ctx->trace=tr;
}
