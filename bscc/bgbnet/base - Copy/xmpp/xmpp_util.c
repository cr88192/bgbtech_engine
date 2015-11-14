/*--
Cat pdnet;Protocols;XMPP/Jabber;Util
Text
	Utility functions for the XMPP (aka: Jabber) protocol.
--*/

#include <bgbnet.h>

char *xmpp_cl_user;
char *xmpp_cl_host;
char *xmpp_cl_passwd;
char *xmpp_cl_rsrc;

dyxNode *xmpp_results=NULL;

/*--
Cat pdnet;Protocols;XMPP/Jabber;Util
Form
	dyxNode *XMPP_CheckResults(char *tag);
Description
	Poll if a given tag is in the results pool.
	Returns NULL if the node could not be found.

	The results pool is used for handling results or errors for iq \
	requests (saving a little bit of coder effort).
--*/
dyxNode *XMPP_CheckResults(char *tag)
{
	dyxNode *cur, *lst;

	lst=NULL;
	cur=xmpp_results;
	while(cur)
	{
		if(dyxGetNodeAttrIsP(cur, "id", tag))
		{
			if(lst)lst->next=cur->next;
				else xmpp_results=cur->next;
			return(cur);
		}
		lst=cur;
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat pdnet;Protocols;XMPP/Jabber;Util
Form
	int XMPP_AddResults(dyxNode *node);
Description
	Tries to add node to the results pool.
	Does nothing if the node does not contain a "type='result'" \
	or "type='error'" attribute.

	The results pool is used for handling results or errors for iq \
	requests (saving a little bit of coder effort).
--*/
int XMPP_AddResults(dyxNode *node)
{
	if(dyxGetNodeAttrIsP(node, "type", "result") ||
		dyxGetNodeAttrIsP(node, "type", "error"))
	{
		node->next=xmpp_results;
		xmpp_results=node;
		return(0);
	}
	return(-1);
}


char *XMPP_GenLocal()
{
	char *buf;

	if(!xmpp_cl_user || !xmpp_cl_host || !xmpp_cl_rsrc)
	{
		buf=kralloc(64);
		kprints(buf, "null@null/null");
		return(buf);
	}

	buf=kralloc(64);
	kprints(buf, "%s@%s/%s", xmpp_cl_user, xmpp_cl_host, xmpp_cl_rsrc);
	return(buf);
}
