#define KEY_BYTES 16

typedef unsigned char byte;

byte** GenerateSBox();
byte** GetInvSBox(byte**);
byte* GetKey();
void SubWord(byte**, byte[]);
byte** ExpandKey(byte**, byte*);
void AddKey(byte[4][4], byte*);