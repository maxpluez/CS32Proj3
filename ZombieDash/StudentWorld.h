#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
using namespace std;

class Actor;
class Exit;
class Penelope;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool canMoveTo(double dest_x, double dest_y, Actor* self);
    void addVomit(double vomitX, double vomitY, int ddir);
    double distance(double x1, double x2, double y1, double y2);
    
    //list<Actor*>::iterator overlapCitizen(Actor* e);
    int exitDo(Exit* e);
    void burn(Actor* a);
    void poison(Actor* a);
    bool penelopeStepOn(Actor* a);
    bool personInFront(double vomitX, double vomitY);
    bool smartScan(Actor* self, double& targetX, double& targetY);
    
    void pGetVaccine();
    void pGetFlame();
    void pGetLandmine();
private:
    list<Actor*> l;
    Penelope* penelope;
    int numOfCitizens;
    list<Actor*>::iterator remove(list<Actor*>::iterator pp);
    bool overlap(Actor* a1, Actor* a2);
};

#endif // STUDENTWORLD_H_
