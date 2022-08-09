/*
by kenta shimazaki
*/

# include "headers.h"

/*
by kenta shimazaki
*/
struct UserControl userControl;
struct World world;

struct Resource resource;

int main(int argc, char** argv) {
	world.debug = 0;
	int r;
	struct option longopt[] = {
		{"debug", 0, NULL, 'd'},
		{NULL    , 0, NULL, 0}
	};
	opterr=0;  //  getopt()による"unknown option"メッセージを抑制する
	while(1){
		r=getopt_long(argc, argv, "d:", longopt, NULL);
		if (r==-1) break;
		switch(r){
			case 'd':
				printf("[+] option debug\n");
				world.debug = 1;
				break;
			default:
				break;
		}
	}
		  
	glutInit(&argc, argv);  
	glutInitWindowSize(1000,700);//デフォルトサイズ設定
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA);
    glClearColor(0.0, 0.0, 1.0, 1.0);
	setKeyConfig();
	glutCreateWindow("JJJJJJ");//
	glutDisplayFunc(Display);//ウィンドウの状態が変更時に呼び出される
    glutReshapeFunc(Reshape);
	glutTimerFunc(DRAWMS,Timer,0);//

	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKey);
	glutSpecialUpFunc(SpecialKeyUp);
	glutKeyboardUpFunc(KeyboardUp);

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     //  PNG画像の読み込み
	readImg(IE_PLAYER1,"img/ore.png");
	readImg(IE_PLAYER2,"img/ore2.png");
	readImg(IE_FLOOR1,"img/block1.png");
	readImg(IE_NEEDLE,"img/needle1.png");
	readImg(IE_BLOCKPHA,"img/blockpha.png");
	readImg(IE_BLOCKGF,"img/yukaRed.png");
	readImg(IE_NEEDLEGF,"img/needle2.png");
	readImg(IE_NEEDLEPHA,"img/needlepha.png");
	readImg(IE_WARP1,"img/door1.png");
	readImg(IE_WARP2,"img/door2.png");
	readImg(IE_GRVLINE1,"img/grvline1.png");
	readImg(IE_GRVLINE2,"img/grvline2.png");
	readImg(IE_SAVE1,"img/save1.png");
	readImg(IE_SAVE2,"img/save2.png");
	readImg(IE_TITLE,"img/logo.png");
	readImg(IE_ENEMY1,"img/enemy1.png");
	readImg(IE_ENEMY2,"img/enemy2.png");
	readImg(IE_ITEM,"img/item.png");
	for(int i=0;i<10;i++){
		char filename[32];
		sprintf(filename,"img/img_%d.png",i);
    	resource.imgNum[i] = pngBind(filename, PNG_NOMIPMAP, PNG_ALPHA, 
                   		&(resource.infoNum[i]), GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}
	for(char i='!';i<='~';i++){
		char filename[32];
		if('A'<=i&&i<='Z'){
			sprintf(filename,"img/img_whitec_%c.png",i);
		}else{
			sprintf(filename,"img/img_white_%c.png",i);
		}
		//printf("[+] resource read %s\n",filename);
    	resource.imgChr[(int)i] = pngBind(filename, PNG_NOMIPMAP, PNG_ALPHA, 
                   		&(resource.infoChr[(int)i]), GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	gameReset();

	glutMainLoop();

    return 0;
}


void readImg(enum imgEnum i,const char *name){
    resource.imgMain[i] = pngBind(name, PNG_NOMIPMAP, PNG_ALPHA, 
                   &resource.infoMain[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
}

void levelReset(struct Level *level){
	for(int x=0;x<MAP_WIDTH_MAX;x++){
		for(int y=0;y<MAP_HEIGHT_MAX;y++){
			level -> map[y][x].status[S_TYPE] = E_EMPTY;
		}
	}
	for(int i=0;i<ENEMY_MAX;i++){
		level -> enemy[i].type = ET_EMPTY;
	}
}


void levelLoad(const char *name){
	Vec2Set(&(world.grav),0,1.4);
	Vec2Set(&(world.player.vel),0,0);
	world.effectId = 0;
	levelReset(&world.level);
	strcpy(world.mapName,name);
	FILE *fp;
	char fullname[32];
	sprintf(fullname,"map/%s.txt",name);
	printf("[+] load %s\n",fullname);
	fp = fopen(fullname,"r");
	if(fp == NULL){
		printf("[+] map file (%s) not find\n",fullname);
		exit(1);
	}
	fscanf(fp,"%d%d",&world.level.type,&world.bgType);
	fscanf(fp,"%d",&world.chipSize);
	Vec2Set(&(world.chipSizeVec2),world.chipSize,world.chipSize);
	fscanf(fp,"%f%f",&world.player.size.x,&world.player.size.y);
	fscanf(fp,"%f",&world.grav.y);
	world.mapHeight = 0;
	Vec2MulOwn(&(world.player.pos),world.chipSize);
	int enemyId=0;
	while(1){
		char str[512];
		fscanf(fp,"%s",str);
		if(strcmp(str,"END")==0)break;
		world.mapHeight ++ ;
		world.mapWidth = strlen(str);
		int y = world.mapHeight - 1;
		for(int x=0;x<world.mapWidth;x++){
			switch(str[x]){
				case '0':
					world.level.map[y][x].status[S_TYPE] = E_BLOCKGF;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 0;
					break;
				case '1':
					world.level.map[y][x].status[S_TYPE] = E_BLOCKGF;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 1;
					break;
				case '@':
					world.level.map[y][x].status[S_TYPE] = E_BLOCK;
					break;
				case '-':
					world.level.map[y][x].status[S_TYPE] = E_GRVLINE;
					world.level.map[y][x].status[S_TOUCHED] = 0;
					world.level.map[y][x].status[S_WIDTH] = 1;
					world.level.map[y][x].status[S_DIR] = D_UP;
					break;
				case '|':
					world.level.map[y][x].status[S_TYPE] = E_GRVLINE;
					world.level.map[y][x].status[S_TOUCHED] = 0;
					world.level.map[y][x].status[S_WIDTH] = 1;
					world.level.map[y][x].status[S_DIR] = D_RIGHT;
					break;
				case '*':
					world.level.map[y][x].status[S_TYPE] = E_BLOCKPHA;
					break;
				case 'l':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLE;
					world.level.map[y][x].status[S_DIR] = D_LEFT;
					break;
				case 'r':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLE;
					world.level.map[y][x].status[S_DIR] = D_RIGHT;
					break;
				case '2':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_UP;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 0;
					break;
				case '3':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_RIGHT;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 0;
					break;
				case '4':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_DOWN;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 0;
					break;
				case '5':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_LEFT;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 0;
					break;
				case '"':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_UP;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 1;
					break;
				case '#':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_RIGHT;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 1;
					break;
				case '$':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_DOWN;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 1;
					break;
				case '%':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLEGF;
					world.level.map[y][x].status[S_DIR] = D_LEFT;
					world.level.map[y][x].status[S_MOD] = 2;
					world.level.map[y][x].status[S_NOW] = 1;
					break;
				case '^':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,0,-8);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case '~':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,0,8);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case '>':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,8,0);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case '<':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,-8,0);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case 'U':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,0,-16);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case 'D':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,0,16);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case 'R':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,16,0);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case 'L':
					world.level.enemy[enemyId].type = ET_NML;
				 	Vec2Set(&world.level.enemy[enemyId].pos,x*world.chipSize,y*world.chipSize);
				 	Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
				 	Vec2Set(&world.level.enemy[enemyId].vel,-16,0);
				 	Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
					enemyId ++;
					break;
				case 'u':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLE;
					world.level.map[y][x].status[S_DIR] = D_UP;
					break;
				case 'd':
					world.level.map[y][x].status[S_TYPE] = E_NEEDLE;
					world.level.map[y][x].status[S_DIR] = D_DOWN;
					break;
				case 's':
					world.level.map[y][x].status[S_TYPE] = E_SAVE;
					world.level.map[y][x].status[S_TOUCHED] = 0;
					break;
				case 'T':
					world.level.map[y][x].status[S_TYPE] = E_TITLE;
					break;
				case 'Z':
					world.level.map[y][x].status[S_TYPE] = E_DEATHCOUNTER;
					break;
				case 'I':
					world.level.map[y][x].status[S_TYPE] = E_ITEM;
					for(int cnt=0;cnt<world.saveData.itemCount;cnt++){
						if(strcmp(world.saveData.itemPlace[cnt],name)==0&&world.saveData.ix[cnt]==x&&world.saveData.iy[cnt]==y){
							world.level.map[y][x].status[S_TYPE] = E_EMPTY;
						}
					}
					break;
				case 'S':
					world.level.map[y][x].status[S_TYPE] = E_START;
					strcpy(world.player.respawnName,name);
					Vec2Set(&(world.player.respawnPos),x*world.chipSize,y*world.chipSize);
					Vec2Cpy(&(world.player.respawnPos),&(world.player.pos));
					break;
				default:
					world.level.map[y][x].status[S_TYPE] = E_EMPTY;
			}
		}
	}

	printf("[+] load map w:%d h:%d\n",world.mapWidth,world.mapHeight);
	int id=0;
	
	while(1){
		int fromX,fromY,toX,toY;
		char toName[FILE_NAME_MAX];
		fscanf(fp,"%d %d %s %d %d",&fromX,&fromY,toName,&toX,&toY);
		if(fromX==-1)break;
		fromX--;fromY--;toX--;toY--;
		world.level.map[fromY][fromX].status[S_TYPE] = E_WARP;
		world.level.map[fromY][fromX].status[S_WARPID] = id;
		strcpy(world.level.warp[id].name,toName);
		world.level.warp[id].x = toX;
		world.level.warp[id].y = toY;
		printf("[+] id:%2d,(%3d,%3d) to %s (%3d,%3d)\n",id,fromX,fromY,toName,toX,toY);
		id ++;
	}

	while(1){
		int x,y;
		char str[256];
		fscanf(fp,"%d %d %s",&x,&y,str);
		if(x==-1)break;
		x--;y--;
		int len = strlen(str);
		for(int i=0;i<len;i+=2){
			if(x+i/2>=world.mapWidth)break;
			world.level.map[y][x+i/2].status[S_TYPE] = E_TEXT;
			if(i+1==len)str[i+1] = '_';
			world.level.map[y][x+i/2].status[S_CHAR] = (str[i]<<8) + str[i+1];
		}
	}

	/*
	重力線をつなげる
	*/
	//横方向
	for(int y=0;y<world.mapHeight;y++){
		for(int x=world.mapWidth-1;x>=1;x--){
			if ( world.level.map[y][x].status[S_TYPE] == E_GRVLINE && world.level.map[y][x-1].status[S_TYPE] == E_GRVLINE
			 && world.level.map[y][x].status[S_DIR] == D_UP && world.level.map[y][x-1].status[S_DIR] == D_UP){
			 	world.level.map[y][x].status[S_TYPE] = E_EMPTY;
				world.level.map[y][x-1].status[S_WIDTH] += world.level.map[y][x].status[S_WIDTH];
			}
		}
	}
	//縦方向
	for(int x=0;x<world.mapWidth;x++){
		for(int y=world.mapHeight-1;y>=1;y--){
			if ( world.level.map[y][x].status[S_TYPE] == E_GRVLINE && world.level.map[y-1][x].status[S_TYPE] == E_GRVLINE
			 && world.level.map[y][x].status[S_DIR] == D_RIGHT && world.level.map[y-1][x].status[S_DIR] == D_RIGHT){
			 	world.level.map[y][x].status[S_TYPE] = E_EMPTY;
				world.level.map[y-1][x].status[S_WIDTH] += world.level.map[y][x].status[S_WIDTH];
			}
		}
	}
	
	
	if(world.level.type != -1)world.time = 0;
	fclose(fp);
	printf("[+] load warp zones\n");
	
}


void gameReset(){
	setKeyConfig();
	world.player.img = 0;
	world.shaking = 0;
	levelReset(&world.level);


	Vec2Set(&(world.camera),0,0);
	Vec2Set(&(world.cameraSmooth),0,0);
	world.frameSize.x = 64*15;
	world.frameSize.y = 64*10;
	world.time=0;

	world.player.dir = D_RIGHT;
	world.player.jump = 0;
	world.player.jumpMax = 2;
	
	for(int i=0;i<EFFECT_MAX;i++){
		world.effect[i].type = EF_NONE;
	}

	FILE *fp;
	fp = fopen(SAVEDATA_FILENAME,"r");
	if(fp==NULL){
		printf("[+] %s is not found.",SAVEDATA_FILENAME);
		exit(1);
	}
    fscanf(fp,"%d",&world.saveData.deathCount);
	fscanf(fp,"%s",world.saveData.respawnName);
	fscanf(fp,"%d%d",&world.saveData.rx,&world.saveData.ry);
	world.saveData.itemCount = 0;
	for(int i=0;i<ITEM_MAX;i++){
		fscanf(fp,"%s",world.saveData.itemPlace[i]);
		fscanf(fp,"%d%d",&world.saveData.ix[i],&world.saveData.iy[i]);
		if(world.saveData.ix[i] == -1) break;
		world.saveData.itemCount ++;
	}	

	fclose(fp);

	levelLoad(world.saveData.respawnName);
	Vec2Set(&world.player.pos,world.saveData.rx,world.saveData.ry);
	
}

void setKeyConfig(){
	userControl.keys[UC_LEFT].isSpecial = 1;
	userControl.keys[UC_LEFT].code = GLUT_KEY_LEFT;
	userControl.status[UC_LEFT] = K_NEUTRAL;
	userControl.keys[UC_RIGHT].isSpecial = 1;
	userControl.keys[UC_RIGHT].code = GLUT_KEY_RIGHT;
	userControl.status[UC_RIGHT] = K_NEUTRAL;
	userControl.keys[UC_JUMP].isSpecial = 0;
	userControl.keys[UC_JUMP].code = 'z';
	userControl.status[UC_JUMP] = K_NEUTRAL;
	userControl.keys[UC_RETRY].isSpecial = 0;
	userControl.keys[UC_RETRY].code = 'r';
	userControl.status[UC_RETRY] = K_NEUTRAL;
	userControl.keys[UC_CHANGE].isSpecial = 0;
	userControl.keys[UC_CHANGE].code = 'o';
	userControl.status[UC_CHANGE] = K_NEUTRAL;
}

void Timer(int value){
	glutPostRedisplay();
	glutTimerFunc(DRAWMS,Timer,0);
}

void Keyboard(unsigned char key,int x,int y){
	for(int i=0;i<UC_KEYS;i++){
		if(userControl.keys[i].isSpecial==0&&userControl.keys[i].code==key){
			if(userControl.status[i] == K_NEUTRAL){
				userControl.status[i] = K_PRESSED;
			}
		}
	}
}

void KeyboardUp(unsigned char key,int x,int y){
	for(int i=0;i<UC_KEYS;i++){
		if(userControl.keys[i].isSpecial==0&&userControl.keys[i].code==key){
			userControl.status[i] = K_UP;
		}
	}
}


void SpecialKey(int key,int x,int y){
	for(int i=0;i<UC_KEYS;i++){
		if(userControl.keys[i].isSpecial&&userControl.keys[i].code==key){
			if(userControl.status[i] == K_NEUTRAL){
				userControl.status[i] = K_PRESSED;
			}
		}
	}
}

void SpecialKeyUp(int key,int x,int y){
	for(int i=0;i<UC_KEYS;i++){
		if(userControl.keys[i].isSpecial&&userControl.keys[i].code==key){
			userControl.status[i] = K_UP;
		}
	}
}
