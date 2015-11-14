/*
BSDE Internal Message Passing

the basic idea is that this will be code related to passing messages between
the physics library and the client(s).

A sane limit will be set on the length of each message queue.
If a queue is too far full, messages will be dropped.

 */

#include "bsde_internal.h"

static int *bsde_msg_int[64];
static bsde_real *bsde_msg_vector[64];
static bsde_real *bsde_msg_scalar[64];

static int bsde_msg_n_int[64];
static int bsde_msg_n_vector[64];
static int bsde_msg_n_scalar[64];

static int bsde_msg_m_int[64];
static int bsde_msg_m_vector[64];
static int bsde_msg_m_scalar[64];

static int bsde_msg_i_int[64];
static int bsde_msg_i_vector[64];
static int bsde_msg_i_scalar[64];

static int bsde_msg_curid;

void BSDE_MsgEmitInt(int val)
{
	BSDE_ThreadState *ctx;
	int i, id;

	ctx=BSDE_GetThreadState();
	id=ctx->msg_id;

	i=bsde_msg_n_int[id]++;
	if(i>=bsde_msg_m_int[id])
	{
		bsde_msg_m_int[id]+=bsde_msg_m_int[id]>>1;
		bsde_msg_int[id]=(int *)realloc(bsde_msg_int[id],
			bsde_msg_m_int[id]*sizeof(int));
	}

	bsde_msg_int[id][i]=val;
}

void BSDE_MsgEmitScalar(bsde_real val)
{
	BSDE_ThreadState *ctx;
	int i, id;

	ctx=BSDE_GetThreadState();
	id=ctx->msg_id;

	i=bsde_msg_n_scalar[id]++;
	if(i>=bsde_msg_m_scalar[id])
	{
		bsde_msg_m_scalar[id]+=bsde_msg_m_scalar[id]>>1;
		bsde_msg_scalar[id]=(bsde_real *)realloc(bsde_msg_scalar[id],
			bsde_msg_m_scalar[id]*sizeof(bsde_real));
	}

	bsde_msg_scalar[id][i]=val;
}

void BSDE_MsgEmitVector(bsde_real x, bsde_real y, bsde_real z, bsde_real w)
{
	BSDE_ThreadState *ctx;
	int i, id;

	ctx=BSDE_GetThreadState();
	id=ctx->msg_id;

	i=bsde_msg_n_vector[id]++;
	if(i>=bsde_msg_m_vector[id])
	{
		bsde_msg_m_vector[id]+=bsde_msg_m_vector[id]>>1;
		bsde_msg_vector[id]=(bsde_real *)realloc(bsde_msg_vector[id],
			bsde_msg_m_vector[id]*4*sizeof(bsde_real));
	}

	bsde_msg_vector[id][i*4+0]=x;
	bsde_msg_vector[id][i*4+1]=y;
	bsde_msg_vector[id][i*4+2]=z;
	bsde_msg_vector[id][i*4+3]=w;
}


void BSDE_MsgBegin(int to, int msg)
{
	BSDE_ThreadState *ctx;

	ctx=BSDE_GetThreadState();
	ctx->msg_id=to;

	BSDE_MsgEmitInt(BSDE_TAG_MSG);
	BSDE_MsgEmitInt(msg);
}

void BSDE_MsgEnd()
{
	BSDE_MsgEmitInt(BSDE_TAG_ENDMSG);
}

void BSDE_MsgAttrI(int var, int val)
{
	BSDE_MsgEmitInt(BSDE_TAG_ATTR_I);
	BSDE_MsgEmitInt(var); BSDE_MsgEmitInt(val);
}

void BSDE_MsgAttrF(int var, bsde_real val)
{
	BSDE_MsgEmitInt(BSDE_TAG_ATTR_F);
	BSDE_MsgEmitInt(var); BSDE_MsgEmitScalar(val);
}

void BSDE_MsgAttrIV(int var, int *val, int num)
{
	int i;
	BSDE_MsgEmitInt(BSDE_TAG_ATTR_IV);
	BSDE_MsgEmitInt(var); BSDE_MsgEmitInt(num);
	for(i=0; i<num; i++)BSDE_MsgEmitInt(val[i]);
}

void BSDE_MsgAttrFV(int var, float *val, int num)
{
	int i;
	BSDE_MsgEmitInt(BSDE_TAG_ATTR_FV);
	BSDE_MsgEmitInt(var); BSDE_MsgEmitInt(num);
	for(i=0; i<num; i++)BSDE_MsgEmitScalar(val[i]);
}

void BSDE_MsgAttrDV(int var, double *val, int num)
{
	int i;
	BSDE_MsgEmitInt(BSDE_TAG_ATTR_FV);
	BSDE_MsgEmitInt(var); BSDE_MsgEmitInt(num);
	for(i=0; i<num; i++)BSDE_MsgEmitScalar(val[i]);
}


int BSDE_MsgFlushQueue(int id)
{
	bsde_msg_i_int[id]=0;
	bsde_msg_n_int[id]=0;
	bsde_msg_i_vector[id]=0;
	bsde_msg_n_vector[id]=0;
	bsde_msg_i_scalar[id]=0;
	bsde_msg_n_scalar[id]=0;

	return(0);
}

int BSDE_MsgPeek()
{
	BSDE_ThreadState *ctx;
	int i, id;

	ctx=BSDE_GetThreadState();

	id=ctx->id;
	if(bsde_msg_i_int[id]>=bsde_msg_n_int[id])
		return(BSDE_NULL);

	i=bsde_msg_i_int[id];
	if(bsde_msg_int[id][i]!=BSDE_TAG_MSG)
	{
		//message queue blown
		BSDE_MsgFlushQueue(id);
		return(BSDE_NULL);
	}

	return(bsde_msg_int[id][i+1]);
}

int BSDE_MsgNext()
{
	BSDE_ThreadState *ctx;
	int i, j, k, n, id, iv, is;

	ctx=BSDE_GetThreadState();

	id=ctx->id;
	if(bsde_msg_i_int[id]>=bsde_msg_n_int[id])
		return(BSDE_NULL);

	i=bsde_msg_i_int[id];
	n=bsde_msg_n_int[id];

	if(bsde_msg_int[id][i]!=BSDE_TAG_MSG)
	{
		//message queue blown
		BSDE_MsgFlushQueue(id);
		return(BSDE_NULL);
	}

	iv=bsde_msg_i_vector[id];
	is=bsde_msg_i_scalar[id];

	i+=2;
	while(i<n)
	{
		j=bsde_msg_int[id][i];
		if(j==BSDE_TAG_ENDMSG) { i++; break; }

		i++;
		switch(j)
		{
		case BSDE_TAG_NORMAL: iv++; break;
		case BSDE_TAG_VERTEX: iv++; break;
		case BSDE_TAG_ATTR_I: i++; i++; break;
		case BSDE_TAG_ATTR_F: i++; is++; break;

		case BSDE_TAG_ATTR_IV:
			i++; k=bsde_msg_int[id][i++]; i+=k; break;
		case BSDE_TAG_ATTR_FV:
			i++; k=bsde_msg_int[id][i++]; is+=k; break;

		case BSDE_TAG_ATTR_2I: i++; i+=2; break;
		case BSDE_TAG_ATTR_3I: i++; i+=3; break;
		case BSDE_TAG_ATTR_4I: i++; i+=4; break;

		case BSDE_TAG_ATTR_2F: i++; iv++; break;
		case BSDE_TAG_ATTR_3F: i++; iv++; break;
		case BSDE_TAG_ATTR_4F: i++; iv++; break;

		default: n=-1; break;
		}
	}

	if(i>=n)
	{
		//message queue blown
		BSDE_MsgFlushQueue(id);
		return(BSDE_NULL);
	}

	bsde_msg_i_int[id]=i;
	bsde_msg_i_vector[id]=iv;
	bsde_msg_i_scalar[id]=is;

	return(BSDE_MsgPeek());
}

int BSDE_MsgGetAttrTag(int id, int *ii, int *iv, int *is)
{
	int i, j, k;

	i=*ii; j=-1;
	switch(bsde_msg_int[id][i])
	{
	case BSDE_TAG_ATTR_I: j=bsde_msg_int[id][i+1]; break;
	case BSDE_TAG_ATTR_F: j=bsde_msg_int[id][i+1]; break;
	case BSDE_TAG_ATTR_IV: j=bsde_msg_int[id][i+1]; break;
	case BSDE_TAG_ATTR_FV: j=bsde_msg_int[id][i+1]; break;

	case BSDE_TAG_ATTR_2I: j=bsde_msg_int[id][i+1]; break;
	case BSDE_TAG_ATTR_3I: j=bsde_msg_int[id][i+1]; break;
	case BSDE_TAG_ATTR_4I: j=bsde_msg_int[id][i+1]; break;

	case BSDE_TAG_ATTR_2F: j=bsde_msg_int[id][i+1]; break;
	case BSDE_TAG_ATTR_3F: j=bsde_msg_int[id][i+1]; break;
	case BSDE_TAG_ATTR_4F: j=bsde_msg_int[id][i+1]; break;
	default: break;
	}

	return(j);
}

int BSDE_MsgGetNextAttr(int id, int *ii, int *iv, int *is)
{
	int i, j, k, l;

	i=*ii; j=*iv; k=*is;
	switch(bsde_msg_int[id][i++])
	{
	case BSDE_TAG_NORMAL: j++; break;
	case BSDE_TAG_VERTEX: j++; break;
	case BSDE_TAG_ATTR_I: i++; i++; break;
	case BSDE_TAG_ATTR_F: i++; k++; break;

	case BSDE_TAG_ATTR_IV:
		i++; l=bsde_msg_int[id][i++]; i+=l; break;
	case BSDE_TAG_ATTR_FV:
		i++; l=bsde_msg_int[id][i++]; k+=l; break;

	case BSDE_TAG_ATTR_2I: i++; i+=2; break;
	case BSDE_TAG_ATTR_3I: i++; i+=3; break;
	case BSDE_TAG_ATTR_4I: i++; i+=4; break;

	case BSDE_TAG_ATTR_2F: i++; j++; break;
	case BSDE_TAG_ATTR_3F: i++; j++; break;
	case BSDE_TAG_ATTR_4F: i++; j++; break;

	default: return(-1); break;
	}

	*ii=i; *iv=j; *is=k;
	return(0);
}

int BSDE_MsgLookup(int var, int *rid, int *rii, int *riv, int *ris)
{
	BSDE_ThreadState *ctx;
	int i, j, k, ni, id, ii, iv, is;

	ctx=BSDE_GetThreadState();

	id=ctx->id; *rid=id;
	ii=bsde_msg_i_int[id];
	ni=bsde_msg_n_int[id];
	iv=bsde_msg_i_vector[id];
	is=bsde_msg_i_scalar[id];

	ii+=2;
	while(ii<ni)
	{
		i=bsde_msg_int[id][ii];
		if(i==BSDE_TAG_ENDMSG)break;

		j=BSDE_MsgGetAttrTag(id, &ii, &iv, &is);
		if(j==var)
		{
			*rii=ii; *riv=iv; *ris=is;
			return(0);
		}

		BSDE_MsgGetNextAttr(id, &ii, &iv, &is);
	}

	return(-1);
}

int BSDE_MsgGetAttrI(int var)
{
	int i, id, ii, iv, is;

	i=BSDE_MsgLookup(var, &id, &ii, &iv, &is);
	if(i<0)return(0);

	i=0;
	switch(bsde_msg_int[id][ii])
	{
		case BSDE_TAG_ATTR_I: i=bsde_msg_int[id][ii+1]; break;
		case BSDE_TAG_ATTR_F: i=bsde_msg_int[id][is]; break;
		default: break;
	}
	return(i);
}

bsde_real BSDE_MsgGetAttrF(int var)
{
	int i, id, ii, iv, is;
	bsde_real f;

	i=BSDE_MsgLookup(var, &id, &ii, &iv, &is);
	if(i<0)return(0);

	f=0;
	switch(bsde_msg_int[id][ii])
	{
		case BSDE_TAG_ATTR_I: f=bsde_msg_int[id][ii+1]; break;
		case BSDE_TAG_ATTR_F: f=bsde_msg_int[id][is]; break;
		default: break;
	}
	return(f);
}

int BSDE_MsgGetAttrIV(int var, int *val, int n)
{
	int i, j, k, id, ii, iv, is;

	if(n<=0)return(-1);

	i=BSDE_MsgLookup(var, &id, &ii, &iv, &is);
	if(i<0)return(-1);

	for(i=0; i<n; i++)val[i]=0;

	i=0;
	switch(bsde_msg_int[id][ii])
	{
		case BSDE_TAG_ATTR_I: val[0]=bsde_msg_int[id][ii+1]; break;
		case BSDE_TAG_ATTR_F: val[0]=bsde_msg_int[id][is]; break;
		case BSDE_TAG_ATTR_IV: j=bsde_msg_int[id][ii+1]; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+2];
			break;
		case BSDE_TAG_ATTR_FV: j=bsde_msg_int[id][ii+1]; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][is+i];
			break;

		case BSDE_TAG_ATTR_2I: j=2; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;
		case BSDE_TAG_ATTR_3I: j=3; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;
		case BSDE_TAG_ATTR_4I: j=4; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;

		case BSDE_TAG_ATTR_2F: j=2; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		case BSDE_TAG_ATTR_3F: j=3; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		case BSDE_TAG_ATTR_4F: j=4; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		default: i=-1; break;
	}
	return(i);
}

int BSDE_MsgGetAttrFV(int var, float *val, int n)
{
	int i, j, k, id, ii, iv, is;

	if(n<=0)return(-1);

	i=BSDE_MsgLookup(var, &id, &ii, &iv, &is);
	if(i<0)return(-1);

	for(i=0; i<n; i++)val[i]=0;

	i=0;
	switch(bsde_msg_int[id][ii])
	{
		case BSDE_TAG_ATTR_I: val[0]=bsde_msg_int[id][ii+1]; break;
		case BSDE_TAG_ATTR_F: val[0]=bsde_msg_int[id][is]; break;
		case BSDE_TAG_ATTR_IV: j=bsde_msg_int[id][ii+1]; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+2];
			break;
		case BSDE_TAG_ATTR_FV: j=bsde_msg_int[id][ii+1]; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][is+i];
			break;

		case BSDE_TAG_ATTR_2I: j=2; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;
		case BSDE_TAG_ATTR_3I: j=3; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;
		case BSDE_TAG_ATTR_4I: j=4; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;

		case BSDE_TAG_ATTR_2F: j=2; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		case BSDE_TAG_ATTR_3F: j=3; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		case BSDE_TAG_ATTR_4F: j=4; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		default: i=-1; break;
	}
	return(i);
}

int BSDE_MsgGetAttrDV(int var, double *val, int n)
{
	int i, j, k, id, ii, iv, is;

	if(n<=0)return(-1);

	i=BSDE_MsgLookup(var, &id, &ii, &iv, &is);
	if(i<0)return(-1);

	for(i=0; i<n; i++)val[i]=0;

	i=0;
	switch(bsde_msg_int[id][ii])
	{
		case BSDE_TAG_ATTR_I: val[0]=bsde_msg_int[id][ii+1]; break;
		case BSDE_TAG_ATTR_F: val[0]=bsde_msg_int[id][is]; break;
		case BSDE_TAG_ATTR_IV: j=bsde_msg_int[id][ii+1]; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+2];
			break;
		case BSDE_TAG_ATTR_FV: j=bsde_msg_int[id][ii+1]; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][is+i];
			break;

		case BSDE_TAG_ATTR_2I: j=2; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;
		case BSDE_TAG_ATTR_3I: j=3; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;
		case BSDE_TAG_ATTR_4I: j=4; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][ii+i+1];
			break;

		case BSDE_TAG_ATTR_2F: j=2; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		case BSDE_TAG_ATTR_3F: j=3; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		case BSDE_TAG_ATTR_4F: j=4; if(n<j)j=n;
			for(i=0; i<j; i++)val[i]=bsde_msg_int[id][iv*4+i];
			break;
		default: i=-1; break;
	}
	return(i);
}

