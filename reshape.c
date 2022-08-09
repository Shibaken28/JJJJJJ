# include "headers.h"

void Reshape(int w,int h){
    glViewport(0,0,w,h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0,w,0,h);
    glScaled(1,-1,1);
    glTranslated(0,-h,0);
    if(w!=world.frameSize.x)w=world.frameSize.x;
    if(h!=world.frameSize.y)h=world.frameSize.y;
    glutReshapeWindow(w,h);
}

