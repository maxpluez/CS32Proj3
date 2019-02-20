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

//-----------------------------------------------------------------------------------------

//Three Must Implement Functions

int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream oss;
    oss<<"level";
    //--------------------------------
    //********************************
    int k = getLevel(); //Remember to Change It (Back)!!
    //********************************
    //--------------------------------
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
                    case Level::vaccine_goodie:
                        l.push_back(new VaccineGoodie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::gas_can_goodie:
                        l.push_back(new GasCanGoodie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::landmine_goodie:
                        l.push_back(new LandmineGoodie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::dumb_zombie:
                        l.push_back(new DumbZombie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::smart_zombie:
                        l.push_back(new SmartZombie(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        break;
                    case Level::citizen:
                        l.push_back(new Citizen(SPRITE_WIDTH*i, SPRITE_HEIGHT*j, this));
                        numOfCitizens++;
                        break;
                    default:
                        break;
                }
            }
        }
        cerr<<"created"<<endl;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if(penelope->doSomething()==GWSTATUS_PLAYER_DIED){
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    list<Actor*>::iterator it = l.begin();
    Actor* current;
    int status;
    for(;it!=l.end();it++){
        current = *it;
        status = current->doSomething();
        if(status==GWSTATUS_FINISHED_LEVEL){
            playSound(SOUND_LEVEL_FINISHED);
            return GWSTATUS_FINISHED_LEVEL;
        }
        if(status==GWSTATUS_PLAYER_DIED){
            playSound(SOUND_PLAYER_DIE);
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    list<Actor*>::iterator p = l.begin();
    while(p!=l.end()){
        current = *p;
        if(!current->isAlive()){
            p=remove(p);
            continue;
        }
        p++;
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

//-----------------------------------------------------------------------------------------

//Some commonly used functions

bool StudentWorld::canMoveTo(double dest_x, double dest_y, Actor* self){
    if(dest_x>=VIEW_WIDTH||dest_x<0||dest_y>=VIEW_HEIGHT||dest_y<0)
        return false;
    Actor* current;
    for(list<Actor*>::iterator p = l.begin();p!=l.end();p++){
        current = *p;
        if(current==self)
            continue;
        if(!current->blockMove())
            continue;
        
        if(abs(dest_x-current->getX())<SPRITE_WIDTH&&abs(dest_y-current->getY())<SPRITE_HEIGHT)
            return false;
        
        
        /*
        double currentLowerX = current->getX();
        double currentLowerY = current->getY();
        double currentUpperX = currentLowerX+SPRITE_WIDTH-1;
        double currentUpperY = currentLowerY+SPRITE_HEIGHT-1;
        
        double selfLowerX = self->getX();
        double selfLowerY = self->getY();
        double selfUpperX = selfLowerX+SPRITE_WIDTH-1;
        double selfUpperY = selfLowerY+SPRITE_HEIGHT-1;
        
        */
        
    }
    
    if(self!=penelope){
        if(abs(dest_x-penelope->getX())<SPRITE_WIDTH&&abs(dest_y-penelope->getY())<SPRITE_HEIGHT)
            return false;
    }
    return true;
}

bool StudentWorld::canFireTo(double dest_x, double dest_y){
    if(dest_x>=VIEW_WIDTH||dest_x<0||dest_y>=VIEW_HEIGHT||dest_y<0)
        return false;
    Actor* current;
    for(list<Actor*>::iterator p = l.begin();p!=l.end();p++){
        current = *p;
        if(!current->blockFlame())
            continue;
        if(abs(dest_x-current->getX())<SPRITE_WIDTH&&abs(dest_y-current->getY())<SPRITE_HEIGHT)
            return false;
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
    if(distance(a1->getX(), a2->getX(), a1->getY(), a2->getY())<=100)
        return true;
    return false;
}

void StudentWorld::addActor(Actor* a){
    l.push_back(a);
}

double StudentWorld::distance(double x1, double x2, double y1, double y2){
    return ((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

//-----------------------------------------------------------------------------------------

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
            pp=remove(pp);
            playSound(SOUND_CITIZEN_SAVED);
        }
        pp++;
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

void StudentWorld::poison(Actor* a){
    if(penelopeStepOn(a))
        penelope->infect();
    Actor* current;
    for(list<Actor*>::iterator p = l.begin();p!=l.end();p++){
        current = *p;
        if(!current->canBeInfected())
            continue;
        if(overlap(a,current))
            current->infect();
    }
}

bool StudentWorld::penelopeStepOn(Actor* a){
    if(overlap(a,penelope)){
        return true;
    }
    return false;
}

bool StudentWorld::personInFront(double vomitX, double vomitY){
    if(distance(penelope->getX(), vomitX, penelope->getY(), vomitY)<=100)
        return true;
    Actor* current;
    for(list<Actor*>::iterator p = l.begin();p!=l.end();p++){
        current = *p;
        if(!current->canBeInfected())
            continue;
        if(distance(current->getX(), vomitX, current->getY(), vomitY)<=100)
            return true;
    }
    return false;
}

bool StudentWorld::smartScan(Actor *self, double& targetX, double& targetY){
    list<Actor*>::iterator min = l.begin();
    Actor* current = *min;
    while(min!=l.end()&&!current->canBeInfected()){
        min++;
        current = *min;
    }
    if(min==l.end()){
        targetX = penelope->getX();
        targetY = penelope->getY();
        double d = distance(self->getX(), targetX, self->getY(), targetY);
        d=sqrt(d);
        if(d>80)
            return false;
        return true;
    }
    
    double minDistance = distance(self->getX(), current->getX(), self->getY(), current->getY());
    
    list<Actor*>::iterator p = min;
    p++;
    current = *p;
    while(p!=l.end()&&!current->canBeInfected()){
        p++;
        current = *p;
    }
    
    double delta;
    
    for(;p!=l.end();p++){
        current = *p;
        if(!current->canBeInfected())
            continue;
        delta = distance(self->getX(), current->getX(), self->getY(), current->getY());
        if(delta<minDistance){
            minDistance = delta;
            min = p;
        }
    }
    
    current = *min;
    
    delta = distance(self->getX(), penelope->getX(), self->getY(), penelope->getY());
    if(delta<minDistance){
        minDistance = delta;
        current = penelope;
    }
    
    minDistance = sqrt(minDistance);
    
    targetX = current->getX();
    targetY = current->getX();
    
    if(minDistance>80)
        return false;
    
    return true;
}

double StudentWorld::distp(Actor* self){
    return sqrt(distance(penelope->getX(), self->getX(), penelope->getY(), self->getY()));
}

double StudentWorld::distz(double x, double y){
    list<Actor*>::iterator min = l.begin();
    Actor* current = *min;
    while(min!=l.end()&&!(current->canBeDamaged()&&!current->canBeInfected()&&current->blockMove())){
        min++;
        current = *min;
    }
    if(min==l.end()){
        return -1;
    }
    
    double minDistance = distance(x, current->getX(), y, current->getY());
    
    list<Actor*>::iterator p = min;
    p++;
    current = *p;
    while(p!=l.end()&&!(current->canBeDamaged()&&!current->canBeInfected()&&current->blockMove())){
        p++;
        current = *p;
    }
    
    double delta;
    
    for(;p!=l.end();p++){
        current = *p;
        if(current->canBeDamaged()&&!current->canBeInfected()&&current->blockMove()){
            delta = distance(x, current->getX(), y, current->getY());
            if(delta<minDistance){
                minDistance = delta;
            }
        }
    }
    return sqrt(minDistance);
}

void StudentWorld::penelopeCoord(double& targetX, double& targetY){
    targetX = penelope->getX();
    targetY = penelope->getY();
}

bool StudentWorld::triggerLandmine(Actor *a){
    if(penelopeStepOn(a))
        return true;
    Actor* current;
    for(list<Actor*>::iterator p = l.begin();p!=l.end();p++){
        current = *p;
        if(current->blockMove()&&current->canBeDamaged()){
            return true;
        }
    }
    return false;
}

void StudentWorld::pGetVaccine(){
    penelope->incVaccines();
}

void StudentWorld::pGetFlame(){
    penelope->incFlames();
}

void StudentWorld::pGetLandmine(){
    penelope->incLandmines();
}

bool StudentWorld::decreaseCitizen(){
    if(numOfCitizens>0){
        numOfCitizens--;
        return true;
    }
    return false;
}
