/*
by kenta shimazaki
*/

# include "headers.h"

//Vec2 Function
void Vec2Set(struct Vec2 *a,float x,float y);
void Vec2Cpy(struct Vec2 *a,struct Vec2 *b);
void Vec2AddOwn(struct Vec2 *a,struct Vec2 *b);
void Vec2AddSchOwn(struct Vec2 *a,float x,float y);
void Vec2AddSch(struct Vec2 *a,float x,float y,struct Vec2 *b);
void Vec2Add(struct Vec2 *a,struct Vec2 *b,struct Vec2 *c);
void Vec2Sub(struct Vec2 *a,struct Vec2 *b,struct Vec2 *c);
void Vec2SubOwn(struct Vec2 *a,struct Vec2 *b);
void Vec2Mul(struct Vec2 *a,float b,struct Vec2 *c);
void Vec2MulOwn(struct Vec2 *a,float b);
float clampf(float* a,float l,float r);
int isExistCommonRange(float l1,float r1,float l2,float r2);
int isRectCollision(struct Vec2 *p1,struct Vec2 *s1,struct Vec2 *p2,struct Vec2 *s2);
int isInRect(struct Vec2 *a, struct Vec2 *ul, struct Vec2 *br);
int isInRange(float x,float l,float r);
int isRectCollision(struct Vec2 *a,struct Vec2 *b,struct Vec2 *c,struct Vec2 *d);
int isCircleCollision(struct Vec2 *p1,float r1,struct Vec2 *p2,float r2);
float Noml2(struct Vec2 *p);
int isInRangeHalf(float x,float l,float r);
int isInRect2(struct Vec2 *a, struct Vec2 *ul, struct Vec2 *br);

// a <- (x,y)
void Vec2Set(struct Vec2 *a,float x,float y){
	a->x = x;
	a->y = y;
}

// b <- a
void Vec2Cpy(struct Vec2 *a,struct Vec2 *b){
	b->x = a->x;
	b->y = a->y;
}

void Vec2AddOwn(struct Vec2 *a,struct Vec2 *b){
	a->x += b->x;
	a->y += b->y;
}

void Vec2AddSch(struct Vec2 *a,float x,float y,struct Vec2 *b){
    b->x = a->x + x;
    b->y = a->y + y;
}

void Vec2AddSchOwn(struct Vec2 *a,float x,float y){
    a->x += x;
    a->y += y;
}

void Vec2SubOwn(struct Vec2 *a,struct Vec2 *b){
	a->x -= b->x;
	a->y -= b->y;
}

void Vec2Add(struct Vec2 *a,struct Vec2 *b,struct Vec2 *c){
	c->x = a->x + b->x;
	c->y = a->y + b->y;
}

void Vec2Sub(struct Vec2 *a,struct Vec2 *b,struct Vec2 *c){
	c->x = a->x - b->x;
	c->y = a->y - b->y;
}

void Vec2Mul(struct Vec2 *a,float b,struct Vec2 *c){
	c->x = a->x * b;
	c->y = a->y * b;
}

void Vec2MulOwn(struct Vec2 *a,float b){
	a->x *= b;
	a->y *= b;
}

int isInRange(float x,float l,float r){
    int ret = 0;
    if(l < x && x < r)ret = 1;
    return ret;
}
int isInRange2(float x,float l,float r){
    int ret = 0;
    if(l <= x && x <= r)ret = 1;
    return ret;
}

int isInRangeHalf(float x,float l,float r){
    int ret = 0;
    if(l < x && x < r)ret = 1;
    return ret;
}

int isInRect2(struct Vec2 *a, struct Vec2 *ul, struct Vec2 *br){
    int ret = 0;
    if(isInRange2(a->x, ul->x, br->x) && isInRange2(a->y, ul->y, br->y) )ret = 1;
    return ret;
}

int isInRect(struct Vec2 *a, struct Vec2 *ul, struct Vec2 *br){
    int ret = 0;
    if(isInRangeHalf(a->x, ul->x, br->x) && isInRangeHalf(a->y, ul->y, br->y) )ret = 1;
    return ret;
}

float clampf(float* a,float l,float r){
    int changed = 0;
    if(*a<l){
        *a=l;
        changed=-1;
    }
    if(*a>=r){
        *a=r-1;
        changed=1;
    }
    return changed;
}

int isExistCommonRange(float l1,float r1,float l2,float r2){
    if((l1<l2&&l2<r1) || (l1<r2&&r2<r1))return 1;
    if((l2<l1&&l1<r2) || (l2<r1&&r1<r2))return 1;
    return 0;
}

int isRectCollision(struct Vec2 *p1,struct Vec2 *s1,struct Vec2 *p2,struct Vec2 *s2){
    int hx = isExistCommonRange(p1->x,p1->x + s1->x,p2->x, p2->x+s2->x);
    int hy = isExistCommonRange(p1->y,p1->y + s1->y,p2->y, p2->y+s2->y);
    return hx&hy;
}

float Noml2(struct Vec2 *p){
    return p->x * p->x + p->y * p->y;
}

/*
中心p1,半径r1とp2,r2の円の当たり判定
*/
int isCircleCollision(struct Vec2 *p1,float r1,struct Vec2 *p2,float r2){
    int ret = 0;
    struct Vec2 d;
    Vec2Sub(p1,p2,&d);
    if ( Noml2(&d) < (r1+r2)*(r1+r2) ) ret = 1;
    return ret;
}



float radians(float deg){
    return M_PI*deg/180.0;
}

void getCircleCircumPos(struct Vec2 *center,float theta,int r,struct Vec2 *ret){
    ret->x = (int)(center->x + cos(theta)*r);
    ret->y = (int)(center->y + sin(theta)*r);
}
