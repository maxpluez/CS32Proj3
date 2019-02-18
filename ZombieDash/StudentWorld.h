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
    bool canMoveTo(double dest_x, double dest_y);
    //list<Actor*>::iterator remove(list<Actor*>::iterator pp);
    //list<Actor*>::iterator overlapCitizen(Actor* e);
    int exitDo(Exit* e);
    bool overlap(Actor* a1, Actor* a2);
private:
    list<Actor*> l;
    Penelope* p;
    int numOfCitizens;
};

#endif // STUDENTWORLD_H_
