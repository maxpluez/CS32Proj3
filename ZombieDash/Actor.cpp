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

People::People(int imageID, double xc, double yc, StudentWorld* w)
: CanBeDamaged(imageID, xc, yc, 0, 0, w){}

void People::newCoord(double& x, double& y, int direction, double amount){
    if(direction==left)
        x-=amount;
    if(direction==right)
        x+=amount;
    if(direction==up)
        y+=amount;
    if(direction==down)
        y-=amount;
}

GoodPeople::GoodPeople(int imageID, double xc, double yc, StudentWorld* w)
: People(imageID, xc, yc, w){
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

Goodie::Goodie(int imageID, double xc, double yc, StudentWorld* w)
: CanBeDamaged(imageID, xc, yc, 0, 1, w){}

BadPeople::BadPeople(int imageID, double xc, double yc, StudentWorld* w)
: People(imageID, xc, yc, w), paralyzed(false), movementPlan(0){}

bool BadPeople::planMovement(){
    if(movementPlan!=0)
        return false;
    int n = randInt(3,10);
    movementPlan = n;
    return true;
}

bool BadPeople::zombieMove(){
    int direction = getDirection();
    double dest_x = getX();
    double dest_y = getY();
    newCoord(dest_x, dest_y, direction, 1);
    if(hell()->canMoveTo(dest_x, dest_y, this)){
        moveTo(dest_x,dest_y);
        movementPlan--;
        return true;
    } else {
        movementPlan=0;
        return false;
    }
}

void BadPeople::setRandomDirection(){
    int i = randInt(1,4);
    switch(i){
        case 1:
            setDirection(up);
            break;
        case 2:
            setDirection(down);
            break;
        case 3:
            setDirection(left);
            break;
        case 4:
            setDirection(right);
            break;
    }
}

bool BadPeople::vomit(){
    double vomitX = getX();
    double vomitY = getY();
    int direction = getDirection();
    if(direction==left||direction==right)
        newCoord(vomitX, vomitY, direction, SPRITE_WIDTH);
    else
        newCoord(vomitX, vomitY, direction, SPRITE_HEIGHT);
    if(hell()->personInFront(vomitX, vomitY)){
        int n = randInt(1,3);
        if(n==1){
            hell()->addVomit(vomitX, vomitY, getDirection());
            hell()->playSound(SOUND_ZOMBIE_VOMIT);
            changeParalyzeStatus();
            return true;
        }
    }
    return false;
}

/*
bool BadPeople::decMovementPlan(){
    if(movementPlan<=0)
        return false;
    movementPlan--;
    return true;
}
 */

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
    ttl--;
    return GWSTATUS_CONTINUE_GAME;
}

//-----------------------------------------------------------------------------------------

//Goodie Classes

VaccineGoodie::VaccineGoodie(double xc, double yc, StudentWorld* w)
: Goodie(IID_VACCINE_GOODIE, xc, yc, w){}

int VaccineGoodie::doSomething(){
    if(!isAlive())
        return GWSTATUS_CONTINUE_GAME;
    if(hell()->penelopeStepOn(this)){
        hell()->increaseScore(50);
        setDead();
        hell()->playSound(SOUND_GOT_GOODIE);
        hell()->pGetVaccine();
    }
    return GWSTATUS_CONTINUE_GAME;
}

GasCanGoodie::GasCanGoodie(double xc, double yc, StudentWorld* w)
: Goodie(IID_GAS_CAN_GOODIE, xc, yc, w){}

int GasCanGoodie::doSomething(){
    if(!isAlive())
        return GWSTATUS_CONTINUE_GAME;
    if(hell()->penelopeStepOn(this)){
        hell()->increaseScore(50);
        setDead();
        hell()->playSound(SOUND_GOT_GOODIE);
        hell()->pGetFlame();
    }
    return GWSTATUS_CONTINUE_GAME;
}

LandmineGoodie::LandmineGoodie(double xc, double yc, StudentWorld* w)
: Goodie(IID_LANDMINE_GOODIE, xc, yc, w){}

int LandmineGoodie::doSomething(){
    if(!isAlive())
        return GWSTATUS_CONTINUE_GAME;
    if(hell()->penelopeStepOn(this)){
        hell()->increaseScore(50);
        setDead();
        hell()->playSound(SOUND_GOT_GOODIE);
        hell()->pGetLandmine();
    }
    return GWSTATUS_CONTINUE_GAME;
}

//-----------------------------------------------------------------------------------------

//Good Peoples

Penelope::Penelope(double xc, double yc, StudentWorld* w)
: GoodPeople(IID_PLAYER, xc, yc, w){
    numLandmines = 0;
    numFlames = 0;
    numVaccines = 0;
}

void Penelope::incLandmines(){
    numLandmines+=2;
}

void Penelope::incFlames(){
    numFlames+=5;
}

void Penelope::incVaccines(){
    numVaccines++;
}

bool Penelope::useLandmine(){
    if(numLandmines>0){
        numLandmines--;
        return true;
    }
    return false;
}

bool Penelope::useFlame(){
    if(numFlames>0){
        numFlames--;
        return true;
    }
    return false;
}

bool Penelope::useVaccine(){
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
    double dest_x = getX();
    double dest_y = getY();
    switch(key){
        case KEY_PRESS_RIGHT:
            setDirection(right);
            newCoord(dest_x, dest_y, right, 4);
            if(hell()->canMoveTo(dest_x, dest_y, this))
                moveTo(dest_x, dest_y);
            break;
        case KEY_PRESS_LEFT:
            setDirection(left);
            newCoord(dest_x, dest_y, left, 4);
            if(hell()->canMoveTo(dest_x, dest_y, this))
                moveTo(dest_x, dest_y);
            break;
        case KEY_PRESS_UP:
            setDirection(up);
            newCoord(dest_x, dest_y, up, 4);
            if(hell()->canMoveTo(dest_x, dest_y, this))
                moveTo(dest_x, dest_y);
            break;
        case KEY_PRESS_DOWN:
            setDirection(down);
            newCoord(dest_x, dest_y, down, 4);
            if(hell()->canMoveTo(dest_x, dest_y, this))
                moveTo(dest_x, dest_y);
            break;
    }
    return GWSTATUS_CONTINUE_GAME;
}

void Penelope::damage(){
    setDead();
}

//-----------------------------------------------------------------------------------------

DumbZombie::DumbZombie(double xc, double yc, StudentWorld* w)
: BadPeople(IID_ZOMBIE, xc, yc, w){}

int DumbZombie::doSomething(){
    if(!isAlive())
        return GWSTATUS_CONTINUE_GAME;
    if(isParalyzed()){
        changeParalyzeStatus();
        return GWSTATUS_CONTINUE_GAME;
    }
    
    //Determine vomit
    if(vomit()){
        changeParalyzeStatus();
        return GWSTATUS_CONTINUE_GAME;
    }
    
    //Determine move
    if(getMovementPlan()==0){
        planMovement();
        setRandomDirection();
    }
    zombieMove();
    changeParalyzeStatus();
    return GWSTATUS_CONTINUE_GAME;
}

SmartZombie::SmartZombie(double xc, double yc, StudentWorld* w)
: BadPeople(IID_ZOMBIE, xc, yc, w){}

int SmartZombie::doSomething(){
    if(!isAlive())
        return GWSTATUS_CONTINUE_GAME;
    if(isParalyzed()){
        changeParalyzeStatus();
        return GWSTATUS_CONTINUE_GAME;
    }
    
    //Determine vomit
    if(vomit()){
        changeParalyzeStatus();
        return GWSTATUS_CONTINUE_GAME;
    }
    
    //Determine move
    if(getMovementPlan()==0){
        planMovement();
    
        double targetX, targetY;
    
        if(hell()->smartScan(this, targetX, targetY)){
            if(getX()<targetX)
                setDirection(right);
            else if(getY()<targetY)
                setDirection(up);
            else if(getX()>targetX)
                setDirection(left);
            else
                setDirection(down);
        } else {
            setRandomDirection();
        }
    }
    
    zombieMove();
    changeParalyzeStatus();
    return GWSTATUS_CONTINUE_GAME;
}
