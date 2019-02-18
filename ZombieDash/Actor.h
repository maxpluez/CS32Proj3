#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

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

class Wall : public Actor{
public:
    Wall(int xc, int yc, StudentWorld* w);
    int doSomething();
    bool blockMove();
    bool blockFlame();
};

class CanBeDamaged : public Actor{
public:
    CanBeDamaged(int imageID, double xc, double yc, int ddir, int ddepth, StudentWorld* w);
    bool canBeDamaged();
};

class CanBeInfected : public CanBeDamaged{
public:
    CanBeInfected(int imageID, double xc, double yc, int ddir, int ddepth, StudentWorld* w);
    bool canBeInfected();
    void infect();
    bool isInfected();
    bool getWorse();
    int getInfectionCount();
    bool blockMove(){return true;}
private:
    bool infected;
    int infectionCount;
};

class Penelope : public CanBeInfected{
public:
    Penelope(double xc, double yc, StudentWorld* w);
    void incLandmines();
    void incFlames();
    void incVaccines();
    bool decLandmines();
    bool decFlames();
    bool decVaccines();
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

class Exit : public Actor{
public:
    Exit(double xc, double yc, StudentWorld* w);
    bool blockFlame();
    int doSomething();
};

//-------------------------------------------------------------------------------------------

class Prop : public Actor{
public:
    Prop(int imageID, double xc, double yc, int ddir, StudentWorld* w);
};

class Pit : public Prop{
public:
    Pit(double xc, double yc, StudentWorld* w);
    int doSomething();
};

#endif // ACTOR_H_
