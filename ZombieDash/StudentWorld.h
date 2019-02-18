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
    //list<Actor*>::iterator overlapCitizen(Actor* e);
    int exitDo(Exit* e);
    void burn(Actor* a);
private:
    list<Actor*> l;
    Penelope* penelope;
    int numOfCitizens;
    list<Actor*>::iterator remove(list<Actor*>::iterator pp);
    bool overlap(Actor* a1, Actor* a2);
};

#endif // STUDENTWORLD_H_
