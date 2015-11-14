#include <btgecm.h>

BTGE_API BTGECM_VoxNeuralNet3D *BTGECM_VoxNN_AllocNet(int xs, int ys, int zs)
{
	BTGECM_VoxNeuralNet3D *tmp;
	
	tmp=gctalloc("btgecm_voxneuralnet3d_t", sizeof(BTGECM_VoxNeuralNet3D));
	tmp->xs=xs;
	tmp->ys=ys;
	tmp->zs=zs;
	tmp->nnsz=xs*ys*zs;
	
	tmp->neuron=gcalloc(tmp->nnsz*sizeof(BTGECM_VoxNeuron3D));
	tmp->nstate=gcalloc(tmp->nnsz*sizeof(BTGECM_VoxNeuronState3D));
	tmp->lstate=gcalloc(tmp->nnsz*sizeof(BTGECM_VoxNeuronState3D));

	return(tmp);
}

BTGE_API float BTGECM_VoxNN_GetNeuronLastOutput(
	BTGECM_VoxNeuralNet3D *net, int x, int y, int z, int fn)
{
	float v;
	int i, j, k;
	
	if((x<0) || (y<0) || (z<0))
		return(0);
	if((x>=net->xs) || (y>=net->ys) || (z>=net->zs))
		return(0);
	
	i=((z*net->ys)+y)*net->xs)+x;
	j=net->lstate[i].ov[fn];
	v=BGBBTJ_DecodeFloat16(j);
	return(v);
}

BTGE_API BTGECM_VoxNeuron3D BTGECM_VoxNN_GetNeuron(
	BTGECM_VoxNeuralNet3D *net, int x, int y, int z)
{
	BTGECM_VoxNeuron3D vn;
	int i, j, k;
	
	if((x<0) || (y<0) || (z<0))
	{
		memset(&vn, 0, sizeof(BTGECM_VoxNeuron3D));
		return(vn);
	}
	if((x>=net->xs) || (y>=net->ys) || (z>=net->zs))
	{
		memset(&vn, 0, sizeof(BTGECM_VoxNeuron3D));
		return(vn);
	}
	
	i=((z*net->ys)+y)*net->xs)+x;
	vn=net->node[i];
	return(vn);
}

BTGE_API BTGECM_VoxNeuronState3D BTGECM_VoxNN_GetNeuronLastState(
	BTGECM_VoxNeuralNet3D *net, int x, int y, int z)
{
	BTGECM_VoxNeuronState3D vn;
	int i, j, k;
	
	if((x<0) || (y<0) || (z<0))
	{
		memset(&vn, 0, sizeof(BTGECM_VoxNeuronState3D));
		return(vn);
	}
	if((x>=net->xs) || (y>=net->ys) || (z>=net->zs))
	{
		memset(&vn, 0, sizeof(BTGECM_VoxNeuronState3D));
		return(vn);
	}
	
	i=((z*net->ys)+y)*net->xs)+x;
	vn=net->lstate[i];
	return(vn);
}

BTGE_API void BTGECM_VoxNN_SetNeuronNextState(
	BTGECM_VoxNeuralNet3D *net, int x, int y, int z,
	BTGECM_VoxNeuronState3D vns)
{
	BTGECM_VoxNeuronState3D vn;
	int i, j, k;
	
	if((x<0) || (y<0) || (z<0))
		{ return; }
	if((x>=net->xs) || (y>=net->ys) || (z>=net->zs))
		{ return; }
	i=((z*net->ys)+y)*net->xs)+x;
	net->nstate[i]=vns;
}

BTGE_API void BTGECM_VoxNN_StepNeuron(
	BTGECM_VoxNeuralNet3D *net, int x, int y, int z)
{
	float vi[6];
	float vo[6];
	float vpi[6];
//	float vpo[6];
	float vpi_ofw[6];
	float vpi_ocw[6];
	float cw[6];
	float ofw[6];
	float ocw[6];
	BTGECM_VoxNeuron3D vn;
	BTGECM_VoxNeuronState3D vst[6];
	BTGECM_VoxNeuronState3D st;
	float cv, cpi;

	vst[0]=BTGECM_VoxNN_GetNeuronLastState(net, x-1, y, z);
	vst[1]=BTGECM_VoxNN_GetNeuronLastState(net, x+1, y, z);
	vst[2]=BTGECM_VoxNN_GetNeuronLastState(net, x, y-1, z);
	vst[3]=BTGECM_VoxNN_GetNeuronLastState(net, x, y+1, z);
	vst[4]=BTGECM_VoxNN_GetNeuronLastState(net, x, y, z-1);
	vst[5]=BTGECM_VoxNN_GetNeuronLastState(net, x, y, z+1);
	vi[0]=BGBBTJ_DecodeFloat16(vst[0].ov[1]);
	vi[1]=BGBBTJ_DecodeFloat16(vst[1].ov[0]);
	vi[2]=BGBBTJ_DecodeFloat16(vst[2].ov[3]);
	vi[3]=BGBBTJ_DecodeFloat16(vst[3].ov[2]);
	vi[4]=BGBBTJ_DecodeFloat16(vst[4].ov[5]);
	vi[5]=BGBBTJ_DecodeFloat16(vst[5].ov[4]);
	vpi[0]=BGBBTJ_DecodeFloat16(vst[0].pi[1]);
	vpi[1]=BGBBTJ_DecodeFloat16(vst[1].pi[0]);
	vpi[2]=BGBBTJ_DecodeFloat16(vst[2].pi[3]);
	vpi[3]=BGBBTJ_DecodeFloat16(vst[3].pi[2]);
	vpi[4]=BGBBTJ_DecodeFloat16(vst[4].pi[5]);
	vpi[5]=BGBBTJ_DecodeFloat16(vst[5].pi[4]);

//	vi[VOXNN_FACE_NX]=BTGECM_VoxNN_GetNeuronLastOutput(
//		net, x-1, y, z, VOXNN_FACE_PX);
//	vi[VOXNN_FACE_PX]=BTGECM_VoxNN_GetNeuronLastOutput(
//		net, x+1, y, z, VOXNN_FACE_NX);
//	vi[VOXNN_FACE_NY]=BTGECM_VoxNN_GetNeuronLastOutput(
//		net, x, y-1, z, VOXNN_FACE_PY);
//	vi[VOXNN_FACE_PY]=BTGECM_VoxNN_GetNeuronLastOutput(
//		net, x, y+1, z, VOXNN_FACE_NY);
//	vi[VOXNN_FACE_NZ]=BTGECM_VoxNN_GetNeuronLastOutput(
//		net, x, y, z-1, VOXNN_FACE_PZ);
//	vi[VOXNN_FACE_PZ]=BTGECM_VoxNN_GetNeuronLastOutput(
//		net, x, y, z+1, VOXNN_FACE_NZ);

	cw[0]=BGBBTJ_DecodeFloat16(vn.cw[0]);
	cw[1]=BGBBTJ_DecodeFloat16(vn.cw[1]);
	cw[2]=BGBBTJ_DecodeFloat16(vn.cw[2]);
	cw[3]=BGBBTJ_DecodeFloat16(vn.cw[3]);
	cw[4]=BGBBTJ_DecodeFloat16(vn.cw[4]);
	cw[5]=BGBBTJ_DecodeFloat16(vn.cw[5]);

	ofw[0]=BGBBTJ_DecodeFloat16(vn.ofw[0]);
	ofw[1]=BGBBTJ_DecodeFloat16(vn.ofw[1]);
	ofw[2]=BGBBTJ_DecodeFloat16(vn.ofw[2]);
	ofw[3]=BGBBTJ_DecodeFloat16(vn.ofw[3]);
	ofw[4]=BGBBTJ_DecodeFloat16(vn.ofw[4]);
	ofw[5]=BGBBTJ_DecodeFloat16(vn.ofw[5]);
	ocw[0]=BGBBTJ_DecodeFloat16(vn.ocw[0]);
	ocw[1]=BGBBTJ_DecodeFloat16(vn.ocw[1]);
	ocw[2]=BGBBTJ_DecodeFloat16(vn.ocw[2]);
	ocw[3]=BGBBTJ_DecodeFloat16(vn.ocw[3]);
	ocw[4]=BGBBTJ_DecodeFloat16(vn.ocw[4]);
	ocw[5]=BGBBTJ_DecodeFloat16(vn.ocw[5]);

	vn=BTGECM_VoxNN_GetNeuron(net, x, y, z);
	cv=	vi[0]*cw[0] +	vi[1]*cw[1] +
		vi[2]*cw[2] +	vi[3]*cw[3] +
		vi[4]*cw[4] +	vi[5]*cw[5];
	
	vo[0]=	vi[1]*ofw[0]+	cv*ocw[0];
	vo[1]=	vi[0]*ofw[1]+	cv*ocw[1];
	vo[2]=	vi[3]*ofw[2]+	cv*ocw[2];
	vo[3]=	vi[2]*ofw[3]+	cv*ocw[3];
	vo[4]=	vi[5]*ofw[4]+	cv*ocw[4];
	vo[5]=	vi[4]*ofw[5]+	cv*ocw[5];

	vpi_ofw[0]=vpi[0]*ofw[0];
	vpi_ofw[1]=vpi[1]*ofw[1];
	vpi_ofw[2]=vpi[2]*ofw[2];
	vpi_ofw[3]=vpi[3]*ofw[3];
	vpi_ofw[4]=vpi[4]*ofw[4];
	vpi_ofw[5]=vpi[5]*ofw[5];

	vpi_cfw[0]=vpi[0]*ocw[0];
	vpi_cfw[1]=vpi[1]*ocw[1];
	vpi_cfw[2]=vpi[2]*ocw[2];
	vpi_cfw[3]=vpi[3]*ocw[3];
	vpi_cfw[4]=vpi[4]*ocw[4];
	vpi_cfw[5]=vpi[5]*ocw[5];

	cpi=vpi_cfw[0]+vpi_cfw[1]+vpi_cfw[2]+
		vpi_cfw[3]+vpi_cfw[4]+vpi_cfw[5];

	st.cv=BGBBTJ_EncodeFloat16(cv);
	st.ov[0]=BGBBTJ_EncodeFloat16(vo[0]);
	st.ov[1]=BGBBTJ_EncodeFloat16(vo[1]);
	st.ov[2]=BGBBTJ_EncodeFloat16(vo[2]);
	st.ov[3]=BGBBTJ_EncodeFloat16(vo[3]);
	st.ov[4]=BGBBTJ_EncodeFloat16(vo[4]);
	st.ov[5]=BGBBTJ_EncodeFloat16(vo[5]);

	BTGECM_VoxNN_SetNeuronNextState(net, x, y, z, st);
}

BTGE_API void BTGECM_VoxNN_StepNet(
	BTGECM_VoxNeuralNet3D *net)
{
	int i, j, k;
	
	for(i=0; i<net->zs; i++)
		for(j=0; j<net->ys; j++)
			for(k=0; k<net->xs; k++)
	{
		BTGECM_VoxNN_StepNeuron(net, i, j, k);
	}
}
