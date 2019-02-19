#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>

class StudentWorld;

//-----------------------------------------------------------------------------------------

//Actor class

class Actor : public GraphObject{
public:
    Actor(int imageID, double xc, double yc, int ddir, int ddepth, StudentWorld* w);
    virtual int doSomething() = 0;
    virtual bool canBeDamaged();
    virtual bool canBeInfected();
    virtual bool blockMove();
    virtual bool blockFlame();
    bool isAlive();
    void setDead();
    virtual void damage(){}
    virtual void infect(){}
    StudentWorld* hell();
private:
    bool alive;
    StudentWorld* m_hell;
};

//-----------------------------------------------------------------------------------------

//Two Special Objects Directly Derived From Actor

class Wall : public Actor{
public:
    Wall(int xc, int yc, StudentWorld* w);
    int doSomething();
    bool blockMove();
    bool blockFlame();
};

class Exit : public Actor{
public:
    Exit(double xc, double yc, StudentWorld* w);
    bool blockFlame();
    int doSomething();
};

//-----------------------------------------------------------------------------------------

//First Level Virtual Classes Derived From Actor

class CanBeDamaged : public Actor{
public:
    CanBeDamaged(int imageID, double xc, double yc, int ddir, int ddepth, StudentWorld* w);
    bool canBeDamaged();
};

class Prop : public Actor{
public:
    Prop(int imageID, double xc, double yc, int ddir, StudentWorld* w);
};

//-----------------------------------------------------------------------------------------

//Second Level Virtual Classes

class People : public CanBeDamaged{
public:
    People(int imageID, double xc, double yc, StudentWorld* w);
    void newCoord(double& x, double& y, int direction, double amount);
    bool blockMove(){return true;}
    int approach(double targetX, double targetY, int& another);
};

class GoodPeople : public People{
public:
    GoodPeople(int imageID, double xc, double yc, StudentWorld* w);
    bool canBeInfected();
    void infect();
    bool infectToDeath();
private:
    bool infected;
    int infectionCount;
    bool isInfected();
    bool getWorse();
    int getInfectionCount();
};

class Goodie : public CanBeDamaged{
public:
    Goodie(int imageID, double xc, double yc, StudentWorld* w);
};

class BadPeople : public People{
public:
    BadPeople(int imageID, double xc, double yc, StudentWorld* w);
    bool zombieMove();
    bool planMovement();
    void setRandomDirection();
    
    bool vomit();
    
    bool isParalyzed(){return paralyzed;}
    void changeParalyzeStatus(){paralyzed = !paralyzed;}
    
    int getMovementPlan(){return movementPlan;}
    //bool decMovementPlan();
    //void setPlanToZero(){movementPlan=0;}
private:
    bool paralyzed;
    int movementPlan;
};

//-----------------------------------------------------------------------------------------

//Prop Classes

class Pit : public Prop{
public:
    Pit(double xc, double yc, StudentWorld* w);
    int doSomething();
};

class Flame : public Prop{
public:
    Flame(double xc, double yc, int ddir, StudentWorld* w);
    int doSomething();
private:
    int ttl;
};

class Vomit : public Prop{
public:
    Vomit(double xc, double yc, int ddir, StudentWorld* w);
    int doSomething();
private:
    int ttl;
};

//-----------------------------------------------------------------------------------------

//Goodie Classes

class VaccineGoodie : public Goodie{
public:
    VaccineGoodie(double xc, double yc, StudentWorld* w);
    int doSomething();
};

class GasCanGoodie : public Goodie{
public:
    GasCanGoodie(double xc, double yc, StudentWorld* w);
    int doSomething();
};

class LandmineGoodie : public Goodie{
public:
    LandmineGoodie(double xc, double yc, StudentWorld* w);
    int doSomething();
};

//-----------------------------------------------------------------------------------------

//Good Peoples

class Penelope : public GoodPeople{
public:
    Penelope(double xc, double yc, StudentWorld* w);
    void incLandmines();
    void incFlames();
    void incVaccines();
    bool useLandmine();
    bool useFlame();
    bool useVaccine();
    int getNumLandmines();
    int getNumFlames();
    int getNumVaccines();
    int doSomething();
    void damage();
private:
    int numLandmines;
    int numFlames;
    int numVaccines;
};

class Citizen : public GoodPeople{
public:
    Citizen(double xc, double yc, StudentWorld* w);
    int doSomething();
    
    bool isParalyzed(){return paralyzed;}
    void changeParalyzeStatus(){paralyzed = !paralyzed;}
private:
    bool paralyzed;
};

//-----------------------------------------------------------------------------------------

//Bad Peoples

class DumbZombie : public BadPeople{
public:
    DumbZombie(double xc, double yc, StudentWorld* w);
    int doSomething();
};

class SmartZombie : public BadPeople{
public:
    SmartZombie(double xc, double yc, StudentWorld* w);
    int doSomething();
};

#endif // ACTOR_H_
