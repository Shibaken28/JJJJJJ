
# include "headers.h"

/*RGB値を代入する．0以上255以下の範囲外の数は受け付けない*/
int setRGB(struct color *a,unsigned char r,unsigned char g,unsigned char b,unsigned ca){
    if(r<0||g<0||b<0||ca<0)return 1;
    if(r>255||g>255||b>255||ca>255)return 1;
    a->r=r;
    a->g=g;
    a->b=b;
    a->a=ca;
    return 0;
}

void setGlColor(struct color *c){
    glColor4ub(c->r,c->g,c->b,c->a);
}

int colorCmp(struct color *a,struct color *b){
    int r=0;
    if(a->r==b->r&&a->g==b->g&&a->b==b->b)r=1;
    return r;
}

void colorCpy(struct color *from,struct color *to){
    to -> r = from -> r;
    to -> g = from -> g;
    to -> b = from -> b;
    to -> a = from -> a;
}
