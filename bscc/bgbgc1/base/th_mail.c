#include <bgbgc.h>

BGBGC_MailMsg *bgbgc_msg_free=NULL;
void *bgbgc_msg_mutex=NULL;
int bgbgc_msg_tls;

int BGBGC_MailInit()
{
	static int init=0;

	if(init)return(0);
	init=1;

	bgbgc_msg_mutex=thMutex();
	bgbgc_msg_tls=thAllocTls();

	return(1);
}

BGBGC_MailBox *BGBGC_MailAllocBox()
{
	BGBGC_MailBox *tmp;

	BGBGC_MailInit();

	tmp=gcalloc("_mailbox_t", sizeof(BGBGC_MailBox));
	tmp->mtx=thFastMutex();
	return(tmp);
}

void BGBGC_MailFreeBox(BGBGC_MailBox *ctx)
{
	thFreeFastMutex(ctx->mtx);
	gcfree(ctx);
}

void BGBGC_MailLockBox(BGBGC_MailBox *ctx)
	{ thLockFastMutex(ctx->mtx); }
void BGBGC_MailUnlockBox(BGBGC_MailBox *ctx)
	{ thUnlockFastMutex(ctx->mtx); }

BGBGC_MailMsg *BGBGC_MailAllocMsg()
{
	BGBGC_MailMsg *tmp;

	tmp=bgbgc_msg_free;
	if(tmp)
	{
		bgbgc_msg_free=tmp->next;
		tmp->next=NULL;
		return(tmp);
	}

	tmp=gcalloc("_mailmsg_t", sizeof(BGBGC_MailMsg));
	return(tmp);
}

void BGBGC_MailFreeMsg(BGBGC_MailMsg *ctx)
{
	msg->to=NULL;
	msg->frm=NULL;
	msg->key=NULL;
	msg->val=NULL;

	ctx->next=bgbgc_msg_free;
	bgbgc_msg_free=ctx;
}

void BGBGC_MailClearMsg(BGBGC_MailMsg *msg)
{
	msg->to=NULL;
	msg->frm=NULL;
	msg->key=NULL;
	msg->val=NULL;
}


void BGBGC_MailAddInbox(BGBGC_MailBox *ctx, BGBGC_MailMsg *msg)
{
	msg->next=ctx->inbox;
	ctx->inbox=msg;
}

void BGBGC_MailAddOutbox(BGBGC_MailBox *ctx, BGBGC_MailMsg *msg)
{
	msg->next=ctx->outbox;
	ctx->outbox=msg;
}

BGBGC_MailMsg *BGBGC_MailGetInbox(BGBGC_MailBox *ctx)
{
	BGBGC_MailMsg *tmp;

	tmp=ctx->inbox;
	if(tmp)ctx->inbox=tmp->next;
	return(tmp);
}


void *thMailAlloc()
	{ return(BGBGC_MailAllocBox()); }
void thMailFree(void *box)
	{ BGBGC_MailFreeBox(box); }

void thMailBind(void *box)
	{ thSetTlsValue(bgbgc_msg_tls, box); }
void *thMailGetBox()
	{ return(thGetTlsValue(bgbgc_msg_tls); }
void thMailUnbind()
	{ thSetTlsValue(bgbgc_msg_tls, NULL); }

void thMailReadNext()
{
	BGBGC_MailBox *ctx;
	BGBGC_MailMsg *msg;

	ctx=thGetTlsValue(bgbgc_msg_tls);
	thLockFastMutex(ctx->mtx);

	if(ctx->msg)
		BGBGC_MailFreeMsg(ctx->msg);

	msg=ctx->inbox;
	if(msg)
	{
		ctx->inbox=msg->next;
		msg->next=NULL;
	}
	ctx->msg=msg;
	thUnlockFastMutex(ctx->mtx);
}

void thMailCreate()
{
	BGBGC_MailBox *ctx;
	BGBGC_MailMsg *msg;

	ctx=thGetTlsValue(bgbgc_msg_tls);
	thLockFastMutex(ctx->mtx);

	if(!ctx->msg)ctx->msg=BGBGC_MailAllocMsg();
		else BGBGC_MailClearMsg(ctx->msg);

	thUnlockFastMutex(ctx->mtx);
}

void thMailReply()
{
	BGBGC_MailBox *ctx;
	BGBGC_MailMsg *msg;

	ctx=thGetTlsValue(bgbgc_msg_tls);
	thLockFastMutex(ctx->mtx);
	if(!ctx->msg)ctx->msg=BGBGC_MailAllocMsg();
	ctx->msg->to=ctx->msg->from;
	ctx->msg->from=ctx;
	thUnlockFastMutex(ctx->mtx);
}

void thMailSend()
{
	BGBGC_MailBox *ctx;
	BGBGC_MailMsg *msg;

	ctx=thGetTlsValue(bgbgc_msg_tls);
	thLockFastMutex(ctx->mtx);
	msg=ctx->msg; ctx->msg=NULL;
	thUnlockFastMutex(ctx->mtx);

	if(!msg)return;
	if(!msg->to)return;

	msg->from=ctx;
	ctx=msg->to;

	thLockFastMutex(ctx->mtx);
	msg->next=ctx->inbox;
	ctx->inbox=msg;
	thUnlockFastMutex(ctx->mtx);
}

void *thMailGetTo()
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)return(ctx->msg->to);
	return(NULL);
}

void *thMailGetFrom()
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)return(ctx->msg->from);
	return(NULL);
}

void *thMailGetKey()
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)return(ctx->msg->key);
	return(NULL);
}

void *thMailGetValue()
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)return(ctx->msg->val);
	return(NULL);
}

void thMailSetTo(void *p)
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)
		ctx->msg->to=p;
}

void thMailSetFrom(void *p)
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)
		ctx->msg->from=p;
}

void thMailSetKey(void *p)
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)
		ctx->msg->key=p;
}

void thMailSetValue(void *p)
{
	ctx=thGetTlsValue(bgbgc_msg_tls);
	if(ctx && ctx->msg)
		ctx->msg->val=p;
}
