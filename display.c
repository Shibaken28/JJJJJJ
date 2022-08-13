
# include "headers.h"

void PutSpriteSize(int num, int x, int y, pngInfo *info,int w,int h,int rot);
void PutSprite(int num, int x, int y, pngInfo *info);
void PutSpriteSizeCol(int num, int x, int y, pngInfo *info,int w,int h,int rot,struct color *col);
void keyStatusUpdate();
void processGame();
void drawGame();
void PutSpriteNumber(int num,int x,int y,int size);
void smoothCamera(float level);
void increment();
void playerDead(int force);
void debugPrint();
void drawEffect();
void debugPrintChar(char *s,int x,int y);
int isWall(struct enemyType *enemy);

void debugPrint(){
    char s[256];
    sprintf(s,"player x:%5d y:%5d vx:%5d vy:%5d\ngrv x:%2.2f y:%2.2f\ncamera x:%5d y:%5d sx:%5d sy:%5d\nchipSize:%3d map h:%4d w:%4d\nt:%7d\nrespawn file:%s x:%5d y:%5d\nmapName: %s mapType: %d\nshaking:%3d\n",
        (int)world.player.pos.x,(int)world.player.pos.y,
        (int)world.player.vel.x,(int)world.player.vel.y,
        world.grav.x,world.grav.y,
        (int)world.camera.x,(int)world.camera.y,
        (int)world.cameraSmooth.x,(int)world.cameraSmooth.y,
        world.chipSize,world.mapHeight,world.mapWidth,world.time,
        world.player.respawnName,
        (int)world.player.respawnPos.x,(int)world.player.respawnPos.y,
        world.mapName,world.level.type,
        world.shaking
    );
    //printf("%s\n",s);
    debugPrintChar(s,10,16);
}

void debugPrintChar(char *s,int x,int y){
    glColor3ub(255,255,255);
    glRasterPos2i(x,y);
    for(int i=0;i<strlen(s);i++){
        if(s[i]=='\n'){
            y += 20;
            glRasterPos2i(x,y);
            continue;
        }
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,s[i]);
    }
}


void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    processGame();
    drawBackground(world.bgType);
    drawEffect();
    drawGame();
    keyStatusUpdate();
    if(world.debug)debugPrint();
    glFlush();
}

/*
force: デバッグモードで無敵でもミス判定を出す
*/
void playerDead(int force){
    if(world.debug && world.player.img &&force == 0)return;
    world.saveData.deathCount ++;
    struct color col = {255,255,255,0};
    world.deadDelay = 15;
    printf("[+] dead %f,%f\n",world.player.pos.x,world.player.pos.y);
    struct Vec2 p;
    Vec2AddSch(&world.player.pos,world.player.size.x/2,world.player.size.y/2,&p);
    //円状にエフェクト
    addEffectRandom(&p,&col,EF_PLAYER_DEAD,world.player.size.x,8);
}

void warpZone(int warpId){
    Vec2Set(&world.player.pos,world.level.warp[warpId].x+0.5f,world.level.warp[warpId].y+0.5f);
	world.shaking = 5;
    printf("[+] warp to %s\n",world.level.warp[warpId].name);
    levelLoad(world.level.warp[warpId].name);
}

int isWall(struct enemyType *enemy){
    int ret = 0;
    if(enemy->status[S_TYPE]==E_BLOCK)ret = 1;
    if(enemy->status[S_TYPE]==E_BLOCKGF && enemy->status[S_NOW]==0 )ret = 1;
    return ret;
}


void movePlayer(){
    if(userControl.status[UC_RETRY]==K_PRESSED){
        playerDead(1);
    }
    if(userControl.status[UC_CHANGE]==K_PRESSED && world.debug ){
        world.player.img ^= 1;
    }
    if(userControl.status[UC_JUMP]==K_PRESSED){
        if(world.player.jump < world.player.jumpMax){
            world.player.jump ++;
            if(world.grav.y>0){
                world.player.vel.y =- 20;
            }else{
                world.player.vel.y =+ 20;
            }
            increment();
        }
    }else if(userControl.status[UC_JUMP]==K_UP){
        if(world.grav.y>0&&world.player.vel.y<0)world.player.vel.y /=2;
        if(world.grav.y<0&&world.player.vel.y>0)world.player.vel.y /=2;
    }

    for(int i=0;i<ENEMY_MAX;i++){
        if(world.level.enemy[i].type==ET_EMPTY)continue;
        switch(world.level.enemy[i].type){
            case ET_EMPTY:
                break;
            case ET_NML:
                Vec2AddOwn(&world.level.enemy[i].pos,&world.level.enemy[i].vel);
            default:
                break;
        }
        int cx = world.level.enemy[i].pos.x/world.chipSize;
        int cy = world.level.enemy[i].pos.y/world.chipSize;
        //周囲のマスと衝突しているかを確認
        for(int dx = -1; dx<=2; dx++){
            for(int dy = -1; dy<=2; dy++){
                int x = cx+dx;
                int y = cy+dy;
                if(isInRange2(x,0,world.mapWidth)&&isInRange2(y,0,world.mapHeight)){
                    if(isWall(&world.level.map[y][x])==1){
                        struct Vec2 ul,br;
                        Vec2Set(&ul,x*world.chipSize,y*world.chipSize);
                        Vec2Add(&ul,&world.chipSizeVec2,&br);
                        if(isInRect(&world.level.enemy[i].pos,&ul,&br)){
                            //printf("12%d\n",rand());
                            Vec2MulOwn(&world.level.enemy[i].vel,-1);
                        }
                    }
                }
            }
        }
        //自機との当たり判定(円同士)
        struct Vec2 colp;
        Vec2AddSch(&world.player.pos,world.player.size.x/2,world.player.size.y/2,&colp);
        if(isCircleCollision(&world.level.enemy[i].pos,world.level.enemy[i].size.x/3,&colp,world.player.size.x/2)){
            playerDead(0);
        }
    }

    if(userControl.status[UC_LEFT]!=K_NEUTRAL || userControl.status[UC_RIGHT]!=K_NEUTRAL){
        if(userControl.status[UC_LEFT]!=K_NEUTRAL){
            world.player.pos.x -= 8;
	        world.player.dir = D_LEFT;
        }
        if(userControl.status[UC_RIGHT]!=K_NEUTRAL){
            world.player.pos.x += 8;
	        world.player.dir = D_RIGHT;
        }
        //壁判定
        int width = world.frameSize.x/world.chipSize+1;
        int height = world.frameSize.y/world.chipSize+1;
        int ulX = world.camera.x/world.chipSize;
        int ulY = world.camera.y/world.chipSize;
        for(int x=ulX;x<ulX+width;x++){
            for(int y=ulY;y<ulY+height;y++){
                if(isWall(&world.level.map[y][x])){
                    struct Vec2 pos;
                    pos.x = world.chipSize * x;
                    pos.y = world.chipSize * y;
                    int d;
                    int dv=1;
                    if(userControl.status[UC_LEFT]!=K_NEUTRAL)dv=1;
                    if(userControl.status[UC_RIGHT]!=K_NEUTRAL)dv=-1;
                    while(1){
                        d = isRectCollision(&world.player.pos,&world.player.size,&pos,&world.chipSizeVec2);
                        if(d==0)break;
                        world.player.pos.x += dv;
                        world.player.vel.x = 0;
                    }
                }
            }
        }
    }
    //重力の影響
    Vec2AddOwn(&(world.player.vel),&(world.grav));
    Vec2AddOwn(&(world.player.pos),&(world.player.vel));

    //床,天井判
    {
        int width = world.frameSize.x/world.chipSize+1;
        int height = world.frameSize.y/world.chipSize+1;
        int ulX = world.camera.x/world.chipSize;
        int ulY = world.camera.y/world.chipSize;
        for(int x=ulX;x<ulX+width;x++){
            for(int y=ulY;y<ulY+height;y++){
                if(x<0||y<0||x>=world.mapWidth||y>=world.mapHeight)continue;
                if(isWall(&world.level.map[y][x])){
                    struct Vec2 pos;
                    pos.x = world.chipSize * x;
                    pos.y = world.chipSize * y;
                    int d;
                    int dv=1;
                    if(world.player.vel.y>0)dv=-1;
                    while(1){
                        d = isRectCollision(&world.player.pos,&world.player.size,&pos,&world.chipSizeVec2);
                        if(d==0)break;
                        if(dv==-1&&world.grav.y>0){//床判定
                            //ジャンプ回数復活
                            world.player.jump = 0;
                        }else if(dv==1&&world.grav.y<0){//床判定
                            //ジャンプ回数復活
                            world.player.jump = 0;
                        }
                        world.player.pos.y += dv;
                        world.player.vel.y = 0;
                    }
                }
            }
        }
    }   
}

void savePoint(){
    strcpy(world.saveData.respawnName,world.mapName);
    world.saveData.rx = (int)world.player.pos.x;
    world.saveData.ry = (int)world.player.pos.y;
    FILE *fp;
    fp = fopen(SAVEDATA_FILENAME,"w");
    fprintf(fp,"%d\n",world.saveData.deathCount);
    fprintf(fp,"%s %d %d\n",world.saveData.respawnName,world.saveData.rx,world.saveData.ry);
    for(int i=0;i<world.saveData.itemCount;i++){
        fprintf(fp,"%s %d %d\n",world.saveData.itemPlace[i],world.saveData.ix[i],world.saveData.iy[i]);
    }
    fprintf(fp,"NULL -1 -1\n");
    fclose(fp);
}

void hitFunc(){
    //その他当たり判定など
    {
	    int grvFlipped = 0;
        for(int x=0;x<world.mapWidth;x++){
            for(int y=0;y<world.mapHeight;y++){
                if(world.level.map[y][x].status[S_TYPE] == E_ITEM){
                    //セーブポイントに触れたか
                    struct Vec2 ul;
                    Vec2Set(&ul,x,y);
                    Vec2MulOwn(&ul,world.chipSize);
                    struct color col = {255,200,100,0};
                    if(world.time%2==0)addEffectRandom(&ul,&col,EF_SQUARE,8,1);                    
                    if(isRectCollision(&world.player.pos,&world.player.size,&ul,&world.chipSizeVec2)){
                        world.level.map[y][x].status[S_TYPE] = E_EMPTY;
                        strcpy(world.saveData.itemPlace[world.saveData.itemCount],world.mapName);
                        world.saveData.ix[world.saveData.itemCount] = x;
                        world.saveData.iy[world.saveData.itemCount] = y;
                        world.saveData.itemCount ++;
                        addEffectRandom(&ul,&col,EF_PLAYER_DEAD,8,100);                    
                    }
                }else if(world.level.map[y][x].status[S_TYPE] == E_SAVE){
                    //セーブポイントに触れたか
                    struct Vec2 ul;
                    Vec2Set(&ul,x,y);
                    Vec2MulOwn(&ul,world.chipSize);
                    struct color col = {200,200,200,0};
                    if(world.time%3==0)addEffectRandom(&ul,&col,EF_SQUARE,12,1);                    
                    if(isRectCollision(&world.player.pos,&world.player.size,&ul,&world.chipSizeVec2)){
                        if(world.level.map[y][x].status[S_TOUCHED] == 0){
                            strcpy(world.player.respawnName,world.mapName);
                            Vec2Cpy(&world.player.pos,&world.player.respawnPos);
                            savePoint();
                            addEffectRandom(&ul,&col,EF_SQUARE_SPREAD,20,10);                    
                        }
                        world.level.map[y][x].status[S_TOUCHED] = 1;//一度触れたら離れるまで判定を無効にする
                    }else{
                        world.level.map[y][x].status[S_TOUCHED] = 0;
                        //ブロック出現エフェクト
                    }
                }else if(world.level.map[y][x].status[S_TYPE] == E_GRVLINE){
                    //重力線に触れたか
                    struct Vec2 ul,size;
                    struct Vec2 wrpPos;
                    wrpPos.x = x;
                    wrpPos.y = y;
                    Vec2MulOwn(&wrpPos,world.chipSize);
                    if(world.level.map[y][x].status[S_DIR] == D_UP){
                        Vec2AddSch(&wrpPos,0,world.chipSize/2-1,&ul);
                        Vec2Set(&size,world.chipSize*world.level.map[y][x].status[S_WIDTH],2);
                    }else{
                        Vec2AddSch(&wrpPos,world.chipSize/2-1,0,&ul);
                        Vec2Set(&size,2,world.chipSize*world.level.map[y][x].status[S_WIDTH]);
                    }
                    if(isRectCollision(&world.player.pos,&world.player.size,&ul,&size)){
                        if(world.level.map[y][x].status[S_TOUCHED] == 0 && grvFlipped == 0){
	                        grvFlipped = 1;
                            world.grav.y = -world.grav.y; //重力反転
                            world.player.vel.y = 0;
                        }
                        world.level.map[y][x].status[S_TOUCHED] = 1; //一度触れたら離れるまで判定を無効にする
                    }else{
                        world.level.map[y][x].status[S_TOUCHED] = 0;
                    }
                }else if(world.level.map[y][x].status[S_TYPE] == E_WARP){
                    //ワープゾーンに触れたか
                    struct Vec2 ul,br;
                    struct Vec2 wrpPos;
                    wrpPos.x = x;
                    wrpPos.y = y;
                    Vec2MulOwn(&wrpPos,world.chipSize);
                    Vec2Sub(&wrpPos,&world.player.size,&ul);
                    Vec2Add(&wrpPos,&world.chipSizeVec2,&br);
                    if(isInRect(&world.player.pos,&ul,&br)){
                        warpZone(world.level.map[y][x].status[S_WARPID]);
                    }
                }else if(world.level.map[y][x].status[S_TYPE] == E_NEEDLE || world.level.map[y][x].status[S_TYPE] == E_NEEDLEGF){
                    //針の判定
                    if(world.level.map[y][x].status[S_TYPE] == E_NEEDLEGF && world.level.map[y][x].status[S_NOW] != 0)continue;
                    struct Vec2 ndlPos;
                    ndlPos.x = x;
                    ndlPos.y = y;
                    Vec2MulOwn(&ndlPos,world.chipSize);
                    struct Vec2 ul1,ul2,br1,br2;
                    if(world.level.map[y][x].status[S_DIR] == D_UP){
                        Vec2AddSch(&ndlPos,-world.player.size.x,world.chipSize-world.player.size.y, &ul1);
                        Vec2AddSch(&ul1,world.chipSize+world.player.size.x,world.player.size.y,&br1);
                        Vec2AddSch(&ndlPos,world.chipSize/2-world.player.size.x,-world.player.size.y, &ul2);
                        Vec2AddSch(&ul2,world.player.size.x,world.player.size.y,&br2);
                        //斜めの部分の判定
                        struct Vec2 ul3,ul4,br3,br4;
                        Vec2AddSch(&ndlPos,-world.player.size.x,-world.player.size.y, &ul3);
                        Vec2AddSch(&ul3,world.chipSize/2,world.chipSize,&br3);
                        if(isInRect2(&world.player.pos,&ul3,&br3)&&world.player.pos.y>-2*(world.player.pos.x-ul3.x-world.chipSize/2)+ul3.y){
                            playerDead(0);
                        }
                        Vec2AddSch(&ndlPos,world.chipSize/2,-world.player.size.y, &ul4);
                        Vec2AddSch(&ul4,world.chipSize/2,world.chipSize,&br4);
                        if(isInRect2(&world.player.pos,&ul4,&br4)&&world.player.pos.y>2*(world.player.pos.x-ul4.x)+ul4.y){
                            playerDead(0);
                        }
                    }else if(world.level.map[y][x].status[S_DIR] == D_DOWN){
                        Vec2AddSch(&ndlPos,-world.player.size.x,-world.player.size.y, &ul1);
                        Vec2AddSch(&ul1,world.chipSize+world.player.size.x,world.player.size.y,&br1);
                        Vec2AddSch(&ndlPos,world.chipSize/2-world.player.size.x,0, &ul2);
                        Vec2AddSch(&ul2,world.player.size.x,world.chipSize,&br2);
                        //斜めの部分の判定
                        struct Vec2 ul3,ul4,br3,br4;
                        Vec2AddSch(&ndlPos,world.chipSize/2,0, &ul3);
                        Vec2AddSch(&ul3,world.chipSize/2,world.chipSize,&br3);
                        if(isInRect2(&world.player.pos,&ul3,&br3)&&world.player.pos.y<-2*(world.player.pos.x-ul3.x-world.chipSize/2)+ul3.y){
                            playerDead(0);
                        }
                        Vec2AddSch(&ndlPos,-world.player.size.x,0, &ul4);
                        Vec2AddSch(&ul4,world.chipSize/2,world.chipSize,&br4);
                        if(isInRect2(&world.player.pos,&ul4,&br4)&&world.player.pos.y<2*(world.player.pos.x-ul4.x)+ul4.y){
                            playerDead(0);
                        }
                    }else if(world.level.map[y][x].status[S_DIR] == D_RIGHT){
                        Vec2AddSch(&ndlPos,-world.player.size.x,-world.player.size.y, &ul1);
                        Vec2AddSch(&ul1,world.player.size.x,world.player.size.y+world.chipSize,&br1);
                        Vec2AddSch(&ndlPos,0,0, &ul2);
                        Vec2AddSch(&ul2,world.chipSize,world.player.size.y,&br2);
                        //斜めの部分の判定
                        struct Vec2 ul3,ul4,br3,br4;
                        Vec2AddSch(&ndlPos,0,-world.player.size.y, &ul3);
                        Vec2AddSch(&ul3,world.chipSize,world.chipSize/2,&br3);
                        if(isInRect2(&world.player.pos,&ul3,&br3)&&world.player.pos.y>0.5*(world.player.pos.x-ul3.x)+ul3.y){
                            playerDead(0);
                        }
                        Vec2AddSch(&ndlPos,0,world.chipSize/2, &ul4);
                        Vec2AddSch(&ul4,world.chipSize,world.chipSize/2,&br4);
                        if(isInRect2(&world.player.pos,&ul4,&br4)&&world.player.pos.y<-0.5*(world.player.pos.x-ul4.x)+ul4.y+world.chipSize/2){
                            playerDead(0);
                        }
                    }else if(world.level.map[y][x].status[S_DIR] == D_LEFT){
                        Vec2AddSch(&ndlPos,world.chipSize-world.player.size.x,-world.player.size.y, &ul2);
                        Vec2AddSch(&ul2,world.player.size.x,world.player.size.y+world.chipSize,&br2);
                        Vec2AddSch(&ndlPos,-world.player.size.x, world.chipSize/2-world.player.size.y, &ul1);
                        Vec2AddSch(&ul1,world.chipSize,world.player.size.y,&br1);
                        //斜めの部分の判定
                        struct Vec2 ul3,ul4,br3,br4;
                        Vec2AddSch(&ndlPos,-world.player.size.x,world.chipSize/2, &ul3);
                        Vec2AddSch(&ul3,world.chipSize,world.chipSize/2,&br3);
                        if(isInRect2(&world.player.pos,&ul3,&br3)&&world.player.pos.y<0.5*(world.player.pos.x-ul3.x)+ul3.y){
                            playerDead(0);
                        }
                        Vec2AddSch(&ndlPos,-world.player.size.x,-world.player.size.y, &ul4);
                        Vec2AddSch(&ul4,world.chipSize,world.chipSize/2,&br4);
                        if(isInRect2(&world.player.pos,&ul4,&br4)&&world.player.pos.y>-0.5*(world.player.pos.x-ul4.x-world.chipSize)+ul4.y){
                            playerDead(0);
                        }
                    }
                    //長方形の判定
                    if(isInRect(&world.player.pos,&ul1,&br1)||isInRect(&world.player.pos,&ul2,&br2)){
                        playerDead(0);
                    }
                }
            }
        }
    }

}


void cameraUpdate(){
    //int mapW = world.mapWidth*world.chipSize;
    int mapH = world.mapHeight*world.chipSize;

    //これがないと床の上でガタガタする
    world.player.pos.x = (int) world.player.pos.x;
    world.player.pos.y = (int) world.player.pos.y;

    //左右の見えない壁
    clampf(&(world.player.pos.x),0,world.mapWidth*world.chipSize-world.player.size.x);
    
    //速度制限
    clampf(&(world.player.vel.x),-world.player.size.x,world.player.size.x);
    clampf(&(world.player.vel.y),-world.player.size.y,world.player.size.y);
 
    //カメラの設定
    if(world.deadDelay<=0){
        smoothCamera(0.2f);
    }else{
        return;
    }

    world.camera.y = world.player.pos.y - world.frameSize.y/2;
    
    if(world.level.type == 1){
        world.camera.y = mapH-world.frameSize.y-world.time*2;
        smoothCamera(1.0f);
        if(world.camera.y + world.frameSize.y <= world.player.pos.y+world.player.size.y){
            playerDead (0);
        }
    }else if(world.level.type == 2){
        world.camera.y = mapH-world.frameSize.y-world.time*3;
        smoothCamera(1.0f);
        if(world.camera.y + world.frameSize.y <= world.player.pos.y+world.player.size.y){
            playerDead (0);
        }
    }else if(world.level.type == 3){
        world.camera.y = mapH-world.frameSize.y-world.time*4;
        smoothCamera(1.0f);
        if(world.camera.y + world.frameSize.y <= world.player.pos.y+world.player.size.y){
            playerDead (0);
        }
    }
    
    //カメラの位置
    world.camera.x = world.player.pos.x - world.frameSize.x/2;
    
    //スクロールストップ
    clampf(&(world.camera.x),0,world.chipSize*world.mapWidth-world.frameSize.x);
    clampf(&(world.camera.y),0,world.chipSize*world.mapHeight-world.frameSize.y);
    
    int cameraAdJustX=0;
    int cameraAdJustY=0;

    if(world.shaking>0){
        int lvl = 20;
        cameraAdJustX=lvl-rand()%(2*lvl);
        cameraAdJustY=lvl-rand()%(2*lvl);
        world.shaking --;
    }
    
    world.cameraSmooth.x += cameraAdJustX;
    world.cameraSmooth.y += cameraAdJustY;
}

void exProcess(){
    //level3-4で登場する耐久レベルの処理
    if(world.level.type == 10){
        static int enemyId = 0;
        if(world.time ==1 ){
            enemyId = 0;
        }
        if(world.time>=30*60){
            world.level.map[4][1].status[S_TYPE] = E_WARP;
            world.level.map[4][1].status[S_WARPID] = 0;
            //カウント終了GG
            world.level.map[0][7].status[S_CHAR] = (((int)'G')<<8) + 'G';
        }else{
            //カウントダウンの表示
            int t = 60-(world.time)/30;
            world.level.map[0][7].status[S_TYPE] = E_TEXT;
            world.level.map[0][7].status[S_CHAR] = (('0' + t/10)<<8) + ('0'+t%10);
            t = world.time ;
            //残り時間によって的に出現間隔，方向が変わる
            if(t<30*10){
                if(t%60==0){
                    world.level.enemy[enemyId].type = ET_NML;
                    Vec2Set(&world.level.enemy[enemyId].pos,-64,world.chipSize*2+rand()%(world.chipSize*(world.mapHeight-5)));
                    Vec2Set(&world.level.enemy[enemyId].vel,8,0);
                    Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
                    Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
                    enemyId ++;
                }
            }else if(t<30*20){
                if(t%60==0){
                    world.level.enemy[enemyId].type = ET_NML;
                    Vec2Set(&world.level.enemy[enemyId].pos,world.chipSize*world.mapWidth+64,world.chipSize*2+rand()%(world.chipSize*(world.mapHeight-5)));
                    Vec2Set(&world.level.enemy[enemyId].vel,-8,0);
                    Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
                    Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
                    enemyId ++;
                }
            }else if(t<30*30){
                if(t%40==0){
                    world.level.enemy[enemyId].type = ET_NML;
                    Vec2Set(&world.level.enemy[enemyId].pos,-64,world.chipSize*2+rand()%(world.chipSize*(world.mapHeight-5)));
                    Vec2Set(&world.level.enemy[enemyId].vel,8,0);
                    Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
                    Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
                    enemyId ++;
                }
            }else if(t<30*40){
                if(t%40==0){
                    world.level.enemy[enemyId].type = ET_NML;
                    Vec2Set(&world.level.enemy[enemyId].pos,world.chipSize*world.mapWidth+64,world.chipSize*2+rand()%(world.chipSize*(world.mapHeight-5)));
                    Vec2Set(&world.level.enemy[enemyId].vel,-8,0);
                    Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
                    Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
                    enemyId ++;
                }
            }else{
                if(t%(t<30*50?60:40)==0){
                    world.level.enemy[enemyId].type = ET_NML;
                    Vec2Set(&world.level.enemy[enemyId].pos,-64,world.chipSize*2+rand()%(world.chipSize*(world.mapHeight-5)));
                    Vec2Set(&world.level.enemy[enemyId].vel,8,0);
                    Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
                    Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
                    enemyId ++;
                    world.level.enemy[enemyId].type = ET_NML;
                    Vec2Set(&world.level.enemy[enemyId].pos,world.chipSize*world.mapWidth+64,world.chipSize*2+rand()%(world.chipSize*(world.mapHeight-5)));
                    Vec2Set(&world.level.enemy[enemyId].vel,-8,0);
                    Vec2AddSchOwn(&world.level.enemy[enemyId].pos,world.chipSize/2,world.chipSize/2);
                    Vec2Set(&world.level.enemy[enemyId].size,world.chipSize,world.chipSize);
                    enemyId ++;
                }
            }
        }
    }
}

void processGame(){
	world.time++;
    world.deadDelay --;

    if(world.deadDelay==0){
        levelLoad(world.player.respawnName);
        world.time = 0;
        Vec2Set(&world.player.vel,0,0);
        Vec2Cpy(&world.player.respawnPos,&world.player.pos);
    }
    if(world.deadDelay>=0){
        return;
    }
    
    exProcess();
    movePlayer();
    hitFunc();
    
    if(isInRangeHalf(world.player.pos.y,0,world.mapHeight * world.chipSize) == 0){
        playerDead(0);
    }

    cameraUpdate();
}

void drawGame(){
    /*
        描画処理
    */
    for(int x=0;x<world.mapWidth;x++){
        for(int y=0;y<world.mapHeight;y++){
            struct color col = {255,255,255,0};
            int put = -1; //描画する画像番号
            int rot = 0; //回転
            //描画する大きさ
            int w = world.chipSize;
            int h = world.chipSize;
            //描画する座標
            int dx = x*world.chipSize-world.cameraSmooth.x;
            int dy = y*world.chipSize-world.cameraSmooth.y;
            if(world.level.map[y][x].status[S_TYPE]==E_BLOCK){
                put = IE_FLOOR1;
            }else if(world.level.map[y][x].status[S_TYPE]==E_ITEM){
                put = IE_ITEM;
            }else if(world.level.map[y][x].status[S_TYPE]==E_TITLE){
                PutSpriteSize(resource.imgMain[IE_TITLE], dx,dy, &resource.infoMain[IE_TITLE], w*9, h*4,0);
            }else if(world.level.map[y][x].status[S_TYPE]==E_DEATHCOUNTER){
                PutSpriteNumber(world.saveData.deathCount,dx,dy,128);
            }else if(world.level.map[y][x].status[S_TYPE]==E_BLOCKGF){
                put = IE_BLOCKGF;
                if(world.level.map[y][x].status[S_NOW]!=0)put = IE_BLOCKPHA;
            }else if(world.level.map[y][x].status[S_TYPE]==E_NEEDLE){
                put = IE_NEEDLE;
                rot = world.level.map[y][x].status[S_DIR];
            }else if(world.level.map[y][x].status[S_TYPE]==E_NEEDLEGF){
                put = IE_NEEDLEGF;
                rot = world.level.map[y][x].status[S_DIR];
                if(world.level.map[y][x].status[S_NOW]!=0)put = IE_NEEDLEPHA;
            }else if(world.level.map[y][x].status[S_TYPE]==E_BLOCKPHA){
                put = IE_BLOCKPHA;
            }else if(world.level.map[y][x].status[S_TYPE]==E_WARP){
                put = IE_WARP1;
                if(world.time%20>=10)put = IE_WARP2; //アニメーション
            }else if(world.level.map[y][x].status[S_TYPE]==E_TEXT){ //文字描画
                int c1 = world.level.map[y][x].status[S_CHAR] >> 8;
                int c2 = world.level.map[y][x].status[S_CHAR] % (1<<8);
                PutSpriteSize(resource.imgChr[c1], dx,dy, &resource.infoChr[c1], w/2, h,0);
                PutSpriteSize(resource.imgChr[c2], dx+w/2,dy , &resource.infoChr[c2], w/2, h,0);
            }else if(world.level.map[y][x].status[S_TYPE]==E_GRVLINE){ 
                put = IE_GRVLINE1;
                rot = world.level.map[y][x].status[S_DIR];
                //重力線が横長or縦長の場合
                if(world.level.map[y][x].status[S_DIR]==D_UP)w *= world.level.map[y][x].status[S_WIDTH];
                else h *= world.level.map[y][x].status[S_WIDTH];
                if(world.level.map[y][x].status[S_TOUCHED]==1)put = IE_GRVLINE2; //プレイヤーが触れていたら
            }else if(world.level.map[y][x].status[S_TYPE]==E_SAVE){
                put = IE_SAVE1;
                if(world.level.map[y][x].status[S_TOUCHED]==1)put = IE_SAVE2; //プレイヤーが触れていたら
            }
            if(put!=-1){
                if(world.level.type == 20)setRGB(&col,0,0,0,200);
                PutSpriteSizeCol(resource.imgMain[put],dx , dy , &resource.infoMain[put], w, h,rot,&col);
            }
        }
    }

    for(int i=0;i<ENEMY_MAX;i++){
        int put = -1;
        switch(world.level.enemy[i].type){
            case ET_EMPTY:
                break;
            case ET_NML:
                put = IE_ENEMY1;
                if(world.time%60<30)put = IE_ENEMY2;
            default:
                break;
        }
        if(put != -1){
            int w = world.level.enemy[i].size.x;
            int h = world.level.enemy[i].size.y;
            PutSpriteSize(resource.imgMain[put], world.level.enemy[i].pos.x-world.cameraSmooth.x-w/2, world.level.enemy[i].pos.y-world.cameraSmooth.y-h/2 , &resource.infoMain[put], w,h, 0);
        }
    }


    if(world.deadDelay<=0){
        int rot = 0;
        if(world.grav.y < 0)rot = 2;
        if(world.player.img == 1)PutSpriteSize(resource.imgMain[IE_PLAYER2], world.player.pos.x-world.cameraSmooth.x, world.player.pos.y-world.cameraSmooth.y , &resource.infoMain[IE_PLAYER2],world.player.size.x,world.player.size.y,rot);
        else PutSpriteSize(resource.imgMain[IE_PLAYER1], world.player.pos.x-world.cameraSmooth.x, world.player.pos.y-world.cameraSmooth.y , &resource.infoMain[IE_PLAYER1],world.player.size.x,world.player.size.y,rot);
    }
    PutSpriteSize(resource.imgMain[IE_ITEM], 8, 8 , &resource.infoMain[IE_ITEM],32,32,0);
    PutSpriteNumber(world.saveData.itemCount,48,8,32);
}




void smoothCamera(float level){
    struct Vec2 sub;
    Vec2Sub(&world.camera,&world.cameraSmooth,&sub);
    Vec2MulOwn(&sub,level);
    Vec2AddOwn(&world.cameraSmooth,&sub);
}

void increment(){
    for(int x=0;x<world.mapWidth;x++){
        for(int y=0;y<world.mapHeight;y++){
            if(world.level.map[y][x].status[S_NOW]<0)continue; 
            if(world.level.map[y][x].status[S_TYPE]==E_BLOCKGF || world.level.map[y][x].status[S_TYPE]==E_NEEDLEGF){
                world.level.map[y][x].status[S_NOW] ++;
                world.level.map[y][x].status[S_NOW] %= world.level.map[y][x].status[S_MOD];
                struct Vec2 ul;
                Vec2Set(&ul,x,y);
                Vec2MulOwn(&ul,world.chipSize);
                struct color col = {255,100,100,0};
                //ブロック出現エフェクト
                if(world.level.map[y][x].status[S_NOW]==0)addEffectRandom(&ul,&col,EF_SQUARE_SPREAD,16,5);                    
            }
        }
    }
}

void keyStatusUpdate(){
    for(int i=0;i<UC_KEYS;i++){
		if(userControl.status[i] == K_PRESSED){
			userControl.status[i] = K_DOWN;
        }
        if(userControl.status[i] == K_UP){
			userControl.status[i] = K_NEUTRAL;
		}
	}
}

void PutSpriteNumber(int num,int x,int y,int size){
    int t=num;
    int d = 1;//桁数
    while(t/=10)d++;
    if(d==1){
        PutSpriteSize(resource.imgChr['0'+num],x+size/4,y,&resource.infoChr['0'+num],size/2,size,0);
    }else{
        int t=d;
        for(;d>0;d--){
            PutSpriteSize(resource.imgChr['0'+num%10],x+4+(size-4)/t*(d-1),y,&resource.infoChr['0'+num%10],(size-8)/t,size,0);
            num/=10;
        }
    }
}

//
//  num番のPNG画像を座標(x,y)に表示する
//
void PutSpriteSize(int num, int x, int y, pngInfo *info,int w,int h,int rot){
    struct color col = {255,255,255,0};
    PutSpriteSizeCol(num, x, y, info,w ,h, rot,&col);
}
void PutSpriteSizeCol(int num, int x, int y, pngInfo *info,int w,int h,int rot,struct color *col){
    setGlColor(col);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, num);
    glColor4ub(255, 255, 255, 255);

    glBegin(GL_QUADS);  //  幅w, 高さhの四角形

    int tx[4] = {0,0,1,1};
    int ty[4] = {0,1,1,0};

    int vx[4] = {x,x,x+w,x+w};
    int vy[4] = {y,y+h,y+h,y};

    for(int i=0;i<4;i++){
        glTexCoord2i(tx[i], ty[i]);
        int j = (4+i-rot)%4;
        glVertex2i(vx[j], vy[j]);
    }

    glEnd();
    
	glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void PutSprite(int num, int x, int y, pngInfo *info){
    int w, h;  //  テクスチャの幅と高さ
    w = info->Width;   //  テクスチャの幅と高さを取得する
    h = info->Height;
	PutSpriteSize(num,x,y,info,w,h,0);
}
