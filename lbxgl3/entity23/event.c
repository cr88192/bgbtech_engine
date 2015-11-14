/*--
Cat lbxgl;Event
Text
	2004-12-13:
	General "Events" level stuff.
	An event is not necissarily a message, and may or may not be directed at an \
	entity (possibilities include using events to allow communication, eg, with \
	the physics system, anim sequencer, sound code, ...).
	Events will be more of a broadcast system, and multiple subsystems may \
	handle an event.
	
	<target> <event> <args*>
	
	target is optionally the name of a particular target (eg: subsystem) \
	intended to handle this message. messages will be sent to all targets with \
	a matching name.
	
	event is a general event name, which will be sent to all handlers for a \
	particular event.
	
	for target and event handlers, a null name will mean "recieve all".
	for events, a null target or event name will send it only to those with \
	null names.
	
	it is reasonable for unknown messages or those intended for a different \
	subsystem to show up in some cases.
--*/

#include <lbxgl2.h>

LBXGL_EventCallback *lbxgl_event_handlers=NULL;

/*--
Cat lbxgl;Event
Form
	int LBXGL_Event_RegisterHandler(char *target, char *event, \
	int (*func)(void *data, char *target, char *event, void **msgdata), \
	void *data);
Description
	Registers a handler for the event system.
	target and event specify the events this handler is to recieve, or NULL \
	as an indicator that any are acceptable.
	func is the callback function, and data is a pointer to be passed back to \
	the callback.
	Normally the callback should return 0, but may return -1 to indicate that \
	it should be removed.
	
	On calls msgdata is a NULL terminated array of message specific data items \
	(pdlib type system).
--*/
int LBXGL_Event_RegisterHandler(char *target, char *event,
	int (*func)(void *data, char *target, char *event, void **msgdata),
	void *data)
{
	LBXGL_EventCallback *tmp;

	tmp=kalloc(sizeof(LBXGL_EventCallback));
	if(target)tmp->target=kstrdup(target);
	if(event)tmp->event=kstrdup(event);

	tmp->func=func;
	tmp->data=data;

	tmp->next=lbxgl_event_handlers;
	lbxgl_event_handlers=tmp;
	return(0);
}

/*--
Cat lbxgl;Event
Form
	int LBXGL_Event_SendEvent(char *target, char *event, void **args);
Description
	Sends an event message.
	
	args is a NULL terminated array of message specific data items \
	(pdlib type system).
--*/
int LBXGL_Event_SendEvent(char *target, char *event, void **args)
{
	LBXGL_EventCallback *cur, *lst;
	int i;

	lst=NULL;
	cur=lbxgl_event_handlers;
	while(cur)
	{
		if(	(!target && cur->target) || (!event && cur->event) ||
			(target && cur->target && strcmp(cur->target, target)) ||
			(event && cur->event && strcmp(cur->event, event)))
		{
			cur=cur->next;
			continue;
		}

		i=cur->func(cur->data, target, event, args);
		if(i==-1)
		{
			if(lst)
			{
				lst->next=cur->next;
				cur=cur->next;
				continue;
			}else
			{
				lbxgl_event_handlers=cur->next;
				cur=cur->next;
				continue;
			}
		}

		lst=cur;
		cur=cur->next;
	}
	return(0);
}
