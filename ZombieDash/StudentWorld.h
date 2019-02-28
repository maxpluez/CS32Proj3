#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
using namespace std;

class Actor;
class Exit;
class Penelope;
class VaccineGoodie;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    
    //Three Must Implement Functions
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    //Commonly Used utility Functions
    bool canMoveTo(double dest_x, double dest_y, Actor* self);
    bool canFireTo(double dest_x, double dest_y);
    void addActor(Actor* a);
    double distance(double x1, double x2, double y1, double y2);
    bool penelopeStepOn(Actor* a);
    bool decreaseCitizen();
    
    //Some actions related to doSomething() that can only be done here
    
    //Exit
    int exitDo(Exit* e);
    
    //Flame
    void burn(Actor* a);
    
    //Vomit
    void poison(Actor* a);
    
    //Zombie
    bool personInFront(double vomitX, double vomitY);
    bool smartScan(Actor* self, double& targetX, double& targetY);
    bool canCreateVaccine(VaccineGoodie* v);
    
    //Citizen
    double distp(Actor* self);
    double distz(double x, double y);
    void penelopeCoord(double& targetX, double& targetY);
    
    //Landmine
    bool triggerLandmine(Actor* a);
    
    //Penelope
    void pGetVaccine();
    void pGetFlame();
    void pGetLandmine();
    
private:
    list<Actor*> l;
    Penelope* penelope;
    int numOfCitizens;
    
    //private member functions
    list<Actor*>::iterator remove(list<Actor*>::iterator pp); //delete memory + remove from list
    bool overlap(Actor* a1, Actor* a2);
};

#endif // STUDENTWORLD_H_
