/*
by Kenta Shimazaki
*/
# include "headers.h"


void drawRectFrameRot(struct Vec2 *p,float r,struct color *c,int size,float Rot){
    glLineWidth(size);
    setGlColor(c);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<4;i++){
        float rad = radians(90*i+Rot);
        struct Vec2 p2;
        getCircleCircumPos(p,rad,r,&p2);
        glVertexVec2(&p2);
    }
    glEnd();
}

void drawRectRot(struct Vec2 *p,float r,struct color *c,float Rot){
    setGlColor(c);
    glBegin(GL_QUADS);
    for(int i=0;i<4;i++){
        float rad = radians(90*i+Rot);
        struct Vec2 p2;
        getCircleCircumPos(p,rad,r,&p2);
        glVertexVec2(&p2);
    }
    glEnd();
}

void drawRectFrame(struct Vec2 *p,struct Vec2 *s,int size,struct color *c){
    glLineWidth(size);
    setGlColor(c);
    glBegin(GL_LINE_LOOP);
    glVertex2i(p->x,p->y);
    glVertex2i(p->x+s->x,p->y);
    glVertex2i(p->x+s->x,p->y+s->y);
    glVertex2i(p->x,p->y+s->y);
    glEnd();
}


void drawRect(struct Vec2 *p,struct Vec2 *s,struct color *c){
    setGlColor(c);
    glBegin(GL_QUADS);
    glVertex2i(p->x,p->y);
    glVertex2i(p->x+s->x,p->y);
    glVertex2i(p->x+s->x,p->y+s->y);
    glVertex2i(p->x,p->y+s->y);
    glEnd();
}

void glVertexVec2(struct Vec2 *p){
    glVertex2i(p->x,p->y);
}

