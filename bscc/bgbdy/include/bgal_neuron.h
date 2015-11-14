/*

Basic Programmable Neural Nets

The neuron serves as a node, and may implement customized logic.

A neuron will typically recieve input via "dendrites" via the pulse method.
The pulse method will be responsible for maintaining the internal state of the
neuron and deciding when to fire.

The neuron will then fire via the "axon", which is another list of links to
other neurons (the axon of any given neuron is the dendrite of another).

Links will always be a dendride on one side, and an axon on the other.

Other neurons may be purely input or output, in which case they may lack
dendrites or axons (these ones will be presumably hooked up to program logic).

*/

typedef struct bgalNeuronType_s bgalNeuronType;
typedef struct bgalNeuronLink_s bgalNeuronLink;
typedef struct bgalNeuron_s bgalNeuron;

struct bgalNeuronType_s {
bgalNeuronType *next;
char *name;

void (*spawn)(bgalNeuron *self);
void (*notify_dlist)(bgalNeuron *self, bgalNeuronLink *link);
void (*notify_alist)(bgalNeuron *self, bgalNeuronLink *link);

void (*pulse)(bgalNeuron *self, bgalNeuron *other,
	bgalNeuronLink *link, float value);
void (*fire)(bgalNeuron *self);
};

struct bgalNeuronLink_s {
bgalNeuronLink *dnext;	//dendrite next
bgalNeuronLink *anext;	//axon next
bgalNeuron *dnode;	//dendrite node
bgalNeuron *anode;	//axon node node
float weight;		//link weight
float energy;		//current link energy
float bpval;		//back energy
};

struct bgalNeuron_s {
bgalNeuronLink *dlist;	//dendrite list (input)
bgalNeuronLink *alist;	//axon list (output)
bgalNeuronType *type;
void *data;
float energy;		//node energy
float limit;
};

