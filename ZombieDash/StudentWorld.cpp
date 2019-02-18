#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <list>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), numOfCitizens(0)
{
}

int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream oss;
    oss<<"level";
    int k = getLevel();
    if(k<10)
        oss<<'0'<<k;
    else
        oss<<k;
    oss<<".txt";
    string levelFile = oss.str();
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found)
        cerr << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << "Your level was improperly formatted" << endl;
    else if (result == Level::load_success){
        for(int i=0;i<LEVEL_WIDTH;i++){
            for(int j=0;j<LEVEL_HEIGHT;j++){
                Level::MazeEntry ge = lev.getContentsOf(i,j);
                switch(ge){
                    case Level::wall:
                        l.push_back(new Wall(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::exit:
                        l.push_back(new Exit(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::player:
                        penelope = new Penelope(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this);
                        break;
                    case Level::pit:
                        l.push_back(new Pit(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    default:
                        cerr << "I'll handle you later";
                }
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    if(penelope->doSomething()==GWSTATUS_PLAYER_DIED)
        return GWSTATUS_PLAYER_DIED;
    list<Actor*>::iterator it = l.begin();
    Actor* current;
    int status;
    for(;it!=l.end();it++){
        current = *it;
        status = current->doSomething();
        if(status==GWSTATUS_FINISHED_LEVEL)
            return GWSTATUS_FINISHED_LEVEL;
        if(status==GWSTATUS_PLAYER_DIED)
            return GWSTATUS_PLAYER_DIED;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    Actor* current;
    list<Actor*>::iterator pp = l.begin();
    while(pp!=l.end()){
        current = *pp;
        delete current;
        pp=l.erase(pp);
    }
    delete penelope;
    penelope=nullptr;
}

//-------------------------------------------------------------------------------------------

//Some commonly used functions

bool StudentWorld::canMoveTo(double dest_x, double dest_y){
    list<Actor*>::iterator p = l.begin();
    Actor* current;
    while(p!=l.end()){
        current = *p;
        if(!current->blockMove()){
            p++;
            continue;
        }
        if(abs(dest_x-current->getX())<SPRITE_WIDTH&&abs(dest_y-current->getY())<SPRITE_HEIGHT)
            return false;
        p++;
    }
    return true;
}


list<Actor*>::iterator StudentWorld::remove(list<Actor*>::iterator pp){
    if(pp==l.end())
        return l.end();
    Actor* c = *pp;
    delete c;
    pp = l.erase(pp);
    return pp;
}

/*

list<Actor*>::iterator StudentWorld::overlapCitizen(Actor* e){
    double delta;
    Actor* current;
    list<Actor*>::iterator pp=l.begin();
    for(;pp!=l.end();pp++){
        current=*pp;
        if(!current->canBeInfected())
            continue;
        delta = (current->getX()-e->getX())*(current->getX()-e->getX())+(current->getY()-e->getY())*(current->getY()-e->getY());
        if(delta<=100)
            break;
    }
    return pp;
}
 */

bool StudentWorld::overlap(Actor* a1, Actor* a2){
    double delta = (a1->getX()-a2->getX())*(a1->getX()-a2->getX())+(a1->getY()-a2->getY())*(a1->getY()-a2->getY());
    if(delta<=100)
        return true;
    return false;
}

//-------------------------------------------------------------------------------------------

//Some actions related to doSomething() that can only be done here

int StudentWorld::exitDo(Exit* e){
    list<Actor*>::iterator pp = l.begin();
    Actor* current;
    while(pp!=l.end()){
        current = *pp;
        if(!current->canBeInfected()){
            pp++;
            continue;
        }
        if(overlap(e,current)){
            increaseScore(500);
            numOfCitizens--;
            delete current;
            pp=l.erase(pp);
        }
    }
    
    if(overlap(e,penelope)){
        if(numOfCitizens==0){
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::burn(Actor* a){
    if(overlap(penelope,a))
        penelope->damage();
    
    list<Actor*>::iterator p = l.begin();
    Actor* current;
    while(p!=l.end()){
        current = *p;
        if(!current->canBeDamaged()){
            p++;
            continue;
        }
        if(overlap(a,current)){
            current->damage();
            p++;
            continue;
        }
        p++;
    }
}
