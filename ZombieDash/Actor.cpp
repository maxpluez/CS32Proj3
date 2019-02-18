#include "Actor.h"
#include "StudentWorld.h"
#include <list>

//-----------------------------------------------------------------------------------------

//Actor

Actor::Actor(int imageID, double xc, double yc, int ddir, int ddepth, StudentWorld* w)
: GraphObject(imageID, xc, yc, ddir, ddepth), alive(true), m_hell(w){}

bool Actor::canBeDamaged(){
    return false;
}

bool Actor::canBeInfected(){
    return false;
}

bool Actor::blockMove(){
    return false;
}

bool Actor::blockFlame(){
    return false;
}

bool Actor::isAlive(){
    return alive;
}

void Actor::setDead(){
    alive = false;
}

StudentWorld* Actor::hell(){
    return m_hell;
}

//-----------------------------------------------------------------------------------------

//Wall and Exit: Two Special Blocks Directly Derived From Actor

Wall::Wall(int xc, int yc, StudentWorld* w) : Actor(IID_WALL, xc, yc, 0, 0, w){}

bool Wall::blockMove(){
    return true;
}

bool Wall::blockFlame(){
    return true;
}

int Wall::doSomething(){return GWSTATUS_CONTINUE_GAME;}

Exit::Exit(double xc, double yc, StudentWorld* w):Actor(IID_EXIT, xc, yc, 0, 1, w){}

bool Exit::blockFlame(){
    return true;
}

/*
 void Exit::doSomething(){
 list<Actor*>::iterator pp = hell()->overlapCitizen(this);
 hell()->remove(pp);
 }
 */

int Exit::doSomething(){
    return hell()->exitDo(this);
}

//-----------------------------------------------------------------------------------------

//First Level Virtual Classes Derived From Actor

CanBeDamaged::CanBeDamaged(int imageID, double xc, double yc, int ddir, int ddepth, StudentWorld* w)
: Actor(imageID, xc, yc, ddir, ddepth, w){}

bool CanBeDamaged::canBeDamaged(){
    return true;
}


Prop::Prop(int imageID, double xc, double yc, int ddir, StudentWorld* w)
: Actor(imageID, xc, yc, ddir, 0, w){}

//-----------------------------------------------------------------------------------------

//Second Level Virtual Classes

GoodPeople::GoodPeople(int imageID, double xc, double yc, StudentWorld* w)
: CanBeDamaged(imageID, xc, yc, 0, 0, w){
    infected = false;
    infectionCount = 0;
}

void GoodPeople::infect(){
    infected = true;
}

bool GoodPeople::isInfected(){
    return infected;
}

bool GoodPeople::getWorse(){
    if(infected){
        infectionCount++;
        return true;
    }
    return false;
}

int GoodPeople::getInfectionCount(){
    return infectionCount;
}

bool GoodPeople::canBeInfected(){
    return true;
}

//-----------------------------------------------------------------------------------------

//Prop Classes

Pit::Pit(double xc, double yc, StudentWorld* w)
: Prop(IID_PIT, xc, yc, 0, w){}

int Pit::doSomething(){
    hell()->burn(this);
    
    return GWSTATUS_CONTINUE_GAME;
}

Flame::Flame(double xc, double yc, int ddir, StudentWorld* w)
: Prop(IID_FLAME, xc, yc, ddir, w), ttl(2){}

int Flame::doSomething(){
    if(!isAlive())
        return GWSTATUS_CONTINUE_GAME;
    if(ttl==0){
        setDead();
        return GWSTATUS_CONTINUE_GAME;
    }
    hell()->burn(this);
    ttl--;
    return GWSTATUS_CONTINUE_GAME;
}

Vomit::Vomit(double xc, double yc, int ddir, StudentWorld* w)
: Prop(IID_VOMIT, xc, yc, ddir, w), ttl(2){}

int Vomit::doSomething(){
    if(!isAlive())
        return GWSTATUS_CONTINUE_GAME;
    if(ttl==0){
        setDead();
        return GWSTATUS_CONTINUE_GAME;
    }
    hell()->poison(this);
    return GWSTATUS_CONTINUE_GAME;
}

//-----------------------------------------------------------------------------------------

//Goodie Classes

//-----------------------------------------------------------------------------------------
Penelope::Penelope(double xc, double yc, StudentWorld* w)
: GoodPeople(IID_PLAYER, xc, yc, w){
    numLandmines = 0;
    numFlames = 0;
    numVaccines = 0;
}

void Penelope::incLandmines(){
    numLandmines++;
}

void Penelope::incFlames(){
    numFlames++;
}

void Penelope::incVaccines(){
    numVaccines++;
}

bool Penelope::decLandmines(){
    if(numLandmines>0){
        numLandmines--;
        return true;
    }
    return false;
}

bool Penelope::decFlames(){
    if(numFlames>0){
        numFlames--;
        return true;
    }
    return false;
}

bool Penelope::decVaccines(){
    if(numVaccines>0){
        numVaccines--;
        return true;
    }
    return false;
}

int Penelope::getNumLandmines(){
    return numLandmines;
}

int Penelope::getNumFlames(){
    return numFlames;
}

int Penelope::getNumVaccines(){
    return numVaccines;
}

int Penelope::doSomething(){
    if(!isAlive())
        return GWSTATUS_PLAYER_DIED;
    if(isInfected()){
        getWorse();
        if(getInfectionCount()>=500){
            setDead();
            hell()->playSound(SOUND_PLAYER_DIE);
            return GWSTATUS_PLAYER_DIED;
        }
    }
    int key;
    if(!hell()->getKey(key))
        return GWSTATUS_CONTINUE_GAME;
    double dest_x, dest_y;
    switch(key){
        case KEY_PRESS_RIGHT:
            setDirection(right);
            dest_x = getX() + 4;
            dest_y = getY();
            if(dest_x>=VIEW_WIDTH)
                return GWSTATUS_CONTINUE_GAME;
            if(hell()->canMoveTo(dest_x, dest_y))
                moveTo(dest_x, dest_y);
            break;
        case KEY_PRESS_LEFT:
            setDirection(left);
            dest_x = getX() - 4;
            dest_y = getY();
            if(dest_x<=0)
                return GWSTATUS_CONTINUE_GAME;
            if(hell()->canMoveTo(dest_x, dest_y))
                moveTo(dest_x, dest_y);
            break;
        case KEY_PRESS_UP:
            setDirection(up);
            dest_x = getX();
            dest_y = getY() + 4;
            if(dest_y>=VIEW_WIDTH)
                return GWSTATUS_CONTINUE_GAME;
            if(hell()->canMoveTo(dest_x, dest_y))
                moveTo(dest_x, dest_y);
            break;
        case KEY_PRESS_DOWN:
            setDirection(down);
            dest_x = getX();
            dest_y = getY() - 4;
            if(dest_y<=0)
                return GWSTATUS_CONTINUE_GAME;
            if(hell()->canMoveTo(dest_x, dest_y))
                moveTo(dest_x, dest_y);
            break;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void Penelope::damage(){
    setDead();
}

