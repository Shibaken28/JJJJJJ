# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <time.h>
# include <math.h>
# include <GL/glut.h>
# include <GL/glpng.h>
# include <getopt.h>

# define DRAWMS 18 //何ミリ秒置きにdisplay関数を呼び出すか

# define UC_KEYS 10

# define MAP_WIDTH_MAX 512
# define MAP_HEIGHT_MAX 512

# define WARP_ZONE_MAX 32
# define FILE_NAME_MAX 16

# define ENEMY_MAX 128

# define EFFECT_MAX 512

# define ITEM_MAX 64

# define SAVEDATA_FILENAME "saveData.txt"

struct SaveData{
	char respawnName[FILE_NAME_MAX];
	int rx,ry;
	char itemPlace[ITEM_MAX][FILE_NAME_MAX];
	int ix[ITEM_MAX],iy[ITEM_MAX];
	int itemCount;
	int deathCount;
};


struct color{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};


enum direction{
	D_UP,D_RIGHT,D_DOWN,D_LEFT
};

enum enemis{
	E_EMPTY,E_BLOCK,E_NEEDLE,E_BLOCKPHA,E_BLOCKGF,E_NEXT,E_WARP,E_START,E_GRVLINE,E_TEXT,E_SAVE
	,E_NEEDLEGF,E_TITLE,E_ITEM,E_DEATHCOUNTER
};

enum status{
	S_TYPE,S_DIR,S_MOD,S_NOW,S_WARPID,S_TOUCHED,S_HEIGHT,S_WIDTH,S_CHAR
};

struct enemyType{
	int status[10];
};

enum keyStatus{
	K_NEUTRAL,K_PRESSED,K_DOWN,K_UP
	//何も押されていない状態,押された瞬間,押されている状態(PRESSEDではない),離された瞬間
};

struct keyType{
	int isSpecial;
	int code;
};

enum controlKey{
	UC_LEFT,UC_RIGHT,UC_JUMP,UC_RETRY,UC_CHANGE
};


struct UserControl{
	struct keyType keys[UC_KEYS];
	enum keyStatus status[UC_KEYS];
};

struct Vec2{
	float x,y;
};

enum EnemyType{
	ET_EMPTY,ET_NML
};

struct Enemy{
	int type;
	struct Vec2 pos;
	struct Vec2 vel;
	struct Vec2 size;
};

struct Player{
	struct Vec2 pos;
	struct Vec2 vel;
	struct Vec2 size;
	enum direction dir;
	int jumpMax ;//最大ジャンプ回数
	int jump ;//現在のジャンプ回数
	struct Vec2 respawnPos;
	char respawnName[FILE_NAME_MAX];
	int img;
};

enum EffectType{
	EF_NONE,EF_SQUARE,EF_SQUARE_SPREAD,EF_PLAYER_DEAD
};

struct Effect{
	int type;
	struct Vec2 pos;
	struct Vec2 vel;
	struct Vec2 acc;
	struct Vec2 size;
	struct color col;
	int life;
	float rot;
};



struct WarpZone{
	char name[FILE_NAME_MAX];
	int x;
	int y;
};

struct Level{
	struct Enemy enemy[ENEMY_MAX];
	struct enemyType map[MAP_HEIGHT_MAX][MAP_WIDTH_MAX];
	int type;
	struct WarpZone warp[WARP_ZONE_MAX];
};

struct World{
	struct Player player;
	struct Vec2 grav;
	struct Level level;
	struct Vec2 camera;//左上の座標をここにする
	struct Vec2 cameraSmooth;//左上の座標をここにする
	struct Vec2 frameSize;
	int chipSize;
	int mapHeight;
	int mapWidth;
	struct Vec2 chipSizeVec2;
	int time;
	int debug;
	int shaking;
	char mapName[FILE_NAME_MAX];
	struct Effect effect[EFFECT_MAX];
	int effectId;
	int deadDelay;
	int bgType;
	struct SaveData saveData;
};


enum imgEnum{
	IE_PLAYER1,IE_PLAYER2,IE_FLOOR1,IE_FLOOR2,IE_NEEDLE,
	IE_BLOCKPHA,IE_BLOCKGF,IE_WARP1,IE_WARP2,
	IE_GRVLINE1,IE_GRVLINE2,IE_SAVE1,IE_SAVE2,
	IE_NEEDLEGF,IE_NEEDLEPHA,IE_TITLE,
	IE_ENEMY1,IE_ENEMY2,IE_ITEM
};

struct Resource{
	GLuint imgMain[64];
	pngInfo infoMain[64];
	GLuint imgNum[10];
	pngInfo infoNum[10];
	GLuint imgChr[256];
	pngInfo infoChr[256];
};

extern struct UserControl userControl;
extern struct World world;
extern struct Resource resource;

//Vec2 Function
void Vec2Set(struct Vec2 *a,float x,float y);
void Vec2Cpy(struct Vec2 *a,struct Vec2 *b);
void Vec2AddOwn(struct Vec2 *a,struct Vec2 *b);
void Vec2SubOwn(struct Vec2 *a,struct Vec2 *b);
void Vec2Add(struct Vec2 *a,struct Vec2 *b,struct Vec2 *c);
void Vec2Sub(struct Vec2 *a,struct Vec2 *b,struct Vec2 *c);
void Vec2Mul(struct Vec2 *a,float b,struct Vec2 *c);
void Vec2MulOwn(struct Vec2 *a,float b);
void Vec2AddSchOwn(struct Vec2 *a,float x,float y);
void Vec2AddSch(struct Vec2 *a,float x,float y,struct Vec2 *b);
float clampf(float* a,float l,float r);
int isExistCommonRange(float l1,float r1,float l2,float r2);
int isInRect(struct Vec2 *a, struct Vec2 *ul, struct Vec2 *br);
int isInRange(float x,float l,float r);
int isInRange2(float x,float l,float r);
int isRectCollision(struct Vec2 *p1,struct Vec2 *s1,struct Vec2 *p2,struct Vec2 *s2);
int isCircleCollision(struct Vec2 *p1,float r1,struct Vec2 *p2,float r2);
float Noml2(struct Vec2 *p);
int isInRangeHalf(float x,float l,float r);

void Display(void);
void Reshape(int,int);
void Timer(int );
void Keyboard(unsigned char,int,int);
void KeyboardUp(unsigned char ,int,int);
void SpecialKey(int,int,int);
void SpecialKeyUp(int,int,int);
void setKeyConfig();
void gameReset();
void levelReset(struct Level *level);
void readImg(enum imgEnum i,const char *name);
void levelLoad(const char *name);

int setRGB(struct color*,unsigned char,unsigned char,unsigned char,unsigned );
void setGlColor(struct color*);
int colorCmp(struct color *a,struct color *b);
void drawRect(struct Vec2 *p,struct Vec2 *s,struct color *c);
void drawRectFrame(struct Vec2 *p,struct Vec2 *s,int size,struct color *c);

float radians(float deg);
void getCircleCircumPos(struct Vec2 *center,float theta,int r,struct Vec2 *ret);
void glVertexVec2(struct Vec2 *p);
void drawRectRot(struct Vec2 *p,float r,struct color *c,float Rot);
void drawRectFrameRot(struct Vec2 *p,float r,struct color *c,int size,float Rot);
void colorCpy(struct color *from,struct color *to);
int isInRect2(struct Vec2 *a, struct Vec2 *ul, struct Vec2 *br);

void drawEffect();
void addEffect(struct Vec2 *p,struct color *col,int type,int size);
void addEffectRandom(struct Vec2 *p,struct color *col,int type,int size,int qu);

void drawBackground(int type);

