/*
by kenta shimazaki
*/

# include "headers.h"

void drawBackground(int type){
    if(type==0){
        struct color col = {50,50,200,255};
        struct Vec2 pos;
        struct Vec2 size;
        int s = 90;
        int z = world.time % s;
        for(int x =0;x*s < world.frameSize.x+z; x++){
            for(int y = 0;y*s < world.frameSize.y+z; y++){
                if((x+y)%2)continue;
                Vec2Set(&pos,x*s-z,y*s-z);
                Vec2Set(&size,s,s);
                drawRectFrame(&pos,&size,1,&col);
            }
        }
    }else if(type==1){
        struct color col = {50,200,100,255};
        struct Vec2 pos;
        int s = 100;
        int z = world.time % s;
        for(int x =-1;x*s < world.frameSize.x-z; x++){
            for(int y = -1;y*s < world.frameSize.y+z; y++){
                if((x+y)%2)continue;
                Vec2Set(&pos,x*s+z,y*s-z);
                drawRectFrameRot(&pos,s,&col,1,90);
            }
        }
    }else if(type==2){
        struct color col = {200,200,40,255};
        struct Vec2 pos;
        int ds = 100;
        int z = (world.time) % ds;
        Vec2Set(&pos,world.frameSize.x/2,world.frameSize.y/2);
        for(int s=z;s<1000;s+=ds){
            drawRectFrameRot(&pos,s,&col,1,45);
        }
    }else if(type==3){
        struct color col = {50,150,200,255};
        struct Vec2 pos;
        struct Vec2 size;
        int s = 400;
        int z = (world.time/2) % s;
        for(int x =-1;x*s < world.frameSize.x+z; x++){
            for(int y = -2;y*s < world.frameSize.y+z; y++){
                if((x+y)%2)continue;
                Vec2Set(&pos,x*s+z,y*s+z*2);
                Vec2Set(&size,s,s);
                drawRectFrame(&pos,&size,5,&col);
            }
        }
        s = 200;
        z = (world.time/2) % s;
        for(int x =-1;x*s < world.frameSize.x+z; x++){
            for(int y = -2;y*s < world.frameSize.y+z; y++){
                if((x+y)%2)continue;
                Vec2Set(&pos,x*s+z,y*s+z*2);
                Vec2Set(&size,s,s);
                drawRectFrame(&pos,&size,1,&col);
            }
        }
    }else if(type==4){
        struct color col = {0,100,255,255};
        if(world.time%180<=60){
            setRGB(&col,0,255,100,255);
        }else if(world.time%180<=120){
            setRGB(&col,255,0,100,255);
        }
        struct Vec2 pos;
        int ds = 100;
        int z = (world.time*3) % ds;
        Vec2Set(&pos,world.frameSize.x/2,world.frameSize.y/2);
        for(int s=z;s<1000;s+=ds){
            drawRectFrameRot(&pos,s,&col,1,45);
        }
    }else if(type==5){
        struct color col = {255,100,100,255};
        struct Vec2 pos;
        struct Vec2 size;
        int s = 80;
        int z = world.time % s;
        for(int x =-1;x*s < world.frameSize.x; x++){
            for(int y = -1;y*s < world.frameSize.y; y++){
                if((x+y)%2)continue;
                Vec2Set(&pos,x*s,y*s+z);
                Vec2Set(&size,s,s);
                drawRectFrame(&pos,&size,1,&col);
            }
        }
    }
}

void drawEffect(){
    for(int i=0;i<EFFECT_MAX;i++){
        Vec2AddOwn(&world.effect[i].vel,&world.effect[i].acc);
        Vec2AddOwn(&world.effect[i].pos,&world.effect[i].vel);
        world.effect[i].life --;
        if(world.effect[i].life<=0){
            world.effect[i].type = EF_NONE;
        }
        struct Vec2 drawPos;
        Vec2Sub(&world.effect[i].pos,&world.cameraSmooth,&drawPos);
        switch (world.effect[i].type){
            case EF_SQUARE:
                world.effect[i].col.a = world.effect[i].life*7;
                drawRectFrameRot(&drawPos,world.effect[i].size.x,&world.effect[i].col,2,world.effect[i].rot);
                break;
            case EF_SQUARE_SPREAD:
                world.effect[i].col.a = world.effect[i].life*7;
                drawRectFrameRot(&drawPos,world.effect[i].size.x,&world.effect[i].col,2,world.effect[i].rot);
                break;
            case EF_PLAYER_DEAD:
                world.effect[i].col.a = world.effect[i].life*7;
                drawRectFrameRot(&drawPos,world.effect[i].size.x,&world.effect[i].col,2,world.effect[i].rot);
                break;
            default:
                break;
        }
    }
}

void addEffect(struct Vec2 *p,struct color *col,int type,int size){
    for(int i=0;i<EFFECT_MAX-1;i++){
        world.effectId ++ ;
        world.effectId %= EFFECT_MAX;
        if(world.effect[world.effectId].type == EF_NONE)break;
    }
    world.effect[world.effectId].type = type;
    colorCpy(col,&world.effect[world.effectId].col);
    Vec2Cpy(p,&world.effect[world.effectId].pos);
    Vec2Set(&world.effect[world.effectId].size,size,size);
    world.effect[world.effectId].rot = 0;
    struct Vec2 dd;
    struct Vec2 I = {0,0};
    switch (type){
        case EF_SQUARE:
            Vec2Set(&world.effect[world.effectId].acc,0,0.2f);
            Vec2Set(&world.effect[world.effectId].vel,0,-5);
            world.effect[world.effectId].life = 30;
            world.effect[world.effectId].rot = rand()%360;
            break;
        case EF_SQUARE_SPREAD:
            getCircleCircumPos(&I,radians(rand()%360),5,&dd);
            Vec2Cpy(&dd,&world.effect[world.effectId].vel);
            Vec2Cpy(&world.effect[world.effectId].vel,&world.effect[world.effectId].acc);
            Vec2MulOwn(&world.effect[world.effectId].acc,-1.0/18);
            world.effect[world.effectId].life = 30;
            world.effect[world.effectId].rot = rand()%360;
            break;
        case EF_PLAYER_DEAD:
            getCircleCircumPos(&I,radians(size),5,&dd);
            Vec2Cpy(&dd,&world.effect[world.effectId].vel);
            Vec2MulOwn(&world.effect[world.effectId].vel,5);
            Vec2Cpy(&world.effect[world.effectId].vel,&world.effect[world.effectId].acc);
            Vec2MulOwn(&world.effect[world.effectId].acc,-1.0/25);
            Vec2Cpy(&world.player.size,&world.effect[world.effectId].size);
            world.effect[world.effectId].life = 30;
            world.effect[world.effectId].rot = size;
            break;
    }
}

void addEffectRandom(struct Vec2 *p,struct color *col,int type,int size,int qu){
    if(type == EF_PLAYER_DEAD){
        for(int i=0;i<qu;i++){
            addEffect(p,col,type,360.0/qu*i);
        }
    }else{
        for(int i=0;i<qu;i++){
            struct Vec2 pos;
            Vec2Cpy(p,&pos);
            Vec2AddSchOwn(&pos,rand()%world.chipSize,rand()%world.chipSize);
            addEffect(&pos,col,type,size);
        }
    }
}
