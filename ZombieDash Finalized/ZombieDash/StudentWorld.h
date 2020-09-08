#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Level.h"

#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    void add(Actor* sth);//pushing a new actor onto the actors vector
    int getPeople(); //returns the number of people alive in the space
    void decPeople(); //when some people is killed, number of people decreases
    void remove(Actor* ac); //remove an actor from the vector
    void incVac(); //increase vaccine count penelope has
    void incCh(); //increase charge count penelope has
    void incLan(); //increase landmine count penelope has
    bool checkOverlap(Actor* sth); //see if anything overlaps with penelope
    bool checkOverlapVm(Actor* random, Actor* vom); //check if two actors overlap one another
    bool checkOverlapAny(Actor* attacked); //check if actors overlap with something deadly
    double distsquare(double ax, double ay, double bx, double by); //compute the distance square between two locations
    void setPeneDead(); //kill penelope
    void setLivingInfect(vomit* vomitStuff); //check if vomit infects any living thing
    double distLiving(Actor* sth);//distance square to living citizen or penelope
    bool CtExit(Actor* Cit);//indicates whether citizen touchs exit
    void citizenMovePene(Actor* someCitizen); //used by citizen to move toward penelope or away from zombies
    void zombieMoveLiving(zombies* smartZombie);//used by zombeie when moving towards living things
    bool citizenZombieParadox(Actor* land); //make sure zombie don't move into penelope
    bool flameNActiveLD(Actor* flameStuff); //when not activated landmine touched flame, return true
    bool encounterBarrier(double x_axis, double y_axis, int dir, Actor* mySelf);//make sure unoverlappable things don't overlap one another due to movement
    bool vomitCoord(double x_axis, double y_axis, int dir);//determine if there's living thing infront of zombies and whether to vomit
    bool flameCoord(double x_axis, double y_axis);//make sure flames don't penetrate walls
    bool tExit(); //see if penelope touches exit
private:
    std::vector<Actor*> actors; //stores all actors except penelope
    Penelope* pene = nullptr; //penelope pointer
    Level lev;
    int m_people = 0;//initiallly no people when non was constructed
};

#endif // STUDENTWORLD_H_
