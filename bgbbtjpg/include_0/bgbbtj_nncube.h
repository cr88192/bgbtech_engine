typedef struct BGBBTJ_NNC_NeuronState3D_s BGBBTJ_NNC_NeuronState3D;
typedef struct BGBBTJ_NNC_NeuronCube3D_s BGBBTJ_NNC_NeuronCube3D;

struct BGBBTJ_NNC_NeuronState3D_s {
u16 out[6], eout[6];
u16 out2[6], eout2[6];
byte weight[6][6];
};

struct BGBBTJ_NNC_NeuronCube3D_s {
int xo, yo, zo;
int xs, ys, zs;
BGBBTJ_NNC_NeuronState3D *net;
};
