#define VOXNN_FACE_NX	0
#define VOXNN_FACE_PX	1
#define VOXNN_FACE_NY	2
#define VOXNN_FACE_PY	3
#define VOXNN_FACE_NZ	4
#define VOXNN_FACE_PZ	5

typedef struct BTGECM_VoxNeuron3D_s BTGECM_VoxNeuron3D;
typedef struct BTGECM_VoxNeuronState3D_s BTGECM_VoxNeuronState3D;
typedef struct BTGECM_VoxNeuralNet3D_s BTGECM_VoxNeuralNet3D;

struct BTGECM_VoxNeuron3D_s {
u16 cw[6];
u16 ofw[6];
u16 ocw[6];
};

struct BTGECM_VoxNeuronState3D_s {
u16 cv;		//center value
u16 ov[6];	//output value
u16 pi[6];	//promote/inhibit
};

struct BTGECM_VoxNeuralNet3D_s {
BTGECM_VoxNeuron3D *node;
BTGECM_VoxNeuronState3D *nstate;	//next state
BTGECM_VoxNeuronState3D *lstate;	//last state
int xs, ys, zs, nnsz;
};
