#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


class StudentWorld;

class Actor: public GraphObject
{
public:
    Actor(int ID, double x_cord, double y_cord, int dir, StudentWorld* world, int depth);
    virtual void doSomething() = 0; //pure virtual function to force the derived classes to implement doSomething themselves
    virtual bool isWall(); //To determine whether something is wall
    virtual bool isLandmine(); //To determine whether something is landmine
    virtual bool isExit(); //For purpose of checking any human touches an exit
    virtual bool isCitizen(); //Check if someone is a citizen
    virtual bool canDie(); //Determine if an object can cause someone to die
    virtual bool isBarrier(); //Determine the objects that block movements of one another
    virtual bool isZombie(); //Determine if sth is a zombie
    virtual bool isFlame(); //Determine if something is a flame
    
    virtual void changingTicks(); // Changing ticks alters T/F every tick so zombies and citizens only do something during the 2nd, 4th ... ticks
    virtual bool getChanging(); //return the value that changingTicks changes to determine whether to move
    virtual void incInfection(); //Increase someone's infection count if infected
    virtual int getInfection(); //get the currect infection count
    virtual void deInfection(); //if applied vaccine, set the infection count back to 0
    virtual void setDead(); //make someone's alive status dead
    virtual void setAlive(); //make something alive when constructed
    virtual bool infected(); //determine if someone is infected
    virtual void setInfect(); //set the infection status
    virtual void deInfect(); //make something back to uninfected state
    virtual StudentWorld* world(); //returns the pointer to the world it's in
    virtual bool aliveStatus(); //returns the alive status of someone
    virtual void setActive(); //for setting the landmine active
    virtual bool getActive(); //get the active status of landmine
private:
    bool livingStatus; //living status of actor
    bool m_changing = false; //for determining whether to move or not
    bool infect = false; //actors start with infection status of false
    StudentWorld* m_world; //stores the world pointer
    bool m_active = false; //the landmine object starts in inactive state
    int m_infectionCount; //record ticks since infection
};

class livingClass: public Actor
{
public:
    livingClass(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh);
    virtual bool isLiving(); //indicate if something is either a citizen or penelope
    virtual bool isBarrier(); //citizen and penelope can block movements of other things with depth of 0
private:
    //bool infect = false;
    //int m_infectionCount;
};
class Penelope: public livingClass
{
public:
    Penelope(StudentWorld* world, double x_cord, double y_cord);
    virtual void doSomething(); //penelope's specialized doSomething method
    void incLandmines(); //if collected a goodie, landmine count increases by 2
    int getLandmines(); //returns the landmine count
    void incCharges(); //when collected gas can, the charges should increase by 5
    int getCharges(); //get the current charge
    void incVaccine(); //increase vaccine count when collected a vaccine goodie
    int getVaccine(); //returns the current vaccine count
private:
    int m_landmines; //number of landmines penelope holds
    int m_charges; //number of charges penelope has
    int m_vaccine; //number of vaccines penelope has
};
class citizen: public livingClass
{
public:
    citizen(StudentWorld* world, double x_cord, double y_cord);
    virtual void doSomething();//citizen's specialized do something
    virtual bool isCitizen(); //returns true since this is a citizen class
    
};


class zombies: public Actor{
public:
    zombies(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh);
    virtual bool isZombie(); //returns true since this is a zombie class
    virtual bool detectionVomit(); //for either smart or dumb zombies to detect whether to vomit
    virtual bool isBarrier(); //zombies block movements of other barriers such as citizens, penelopes ...
    virtual void superRandomMove(); //makes either smart or dumb zombies move randomly
    virtual int getMovementPlan(); //returns the steps movement plan has
    virtual void setMovementPlan(); //set movement plan to a random value between 3 and 10
    virtual void decMovementPlan(); //decrease movement plan when moved
    virtual void resetMovementPlan(); //sets movement plan to 0
    virtual int getEighty(); //movement plan when smart zombie chases someone
    virtual void setEighty(); //set the chasing movement plan to 0
    virtual void decEighty(); //decrease the chasing movement plan by 1
    virtual void resetEighty(); // reset the chasing movement plan to 0
    virtual void setDir(int someInt); //set the direction of movement when smart zombie is chasing
    virtual int getDir(); //get the direction smart zombie is moveing toward
private:
    int m_movementPlan;//movement plan number when zombies in random movement
    int eightyMovementPlan;//movement plan number when zombie is chasing
    int direct = 0; //direction when zombie in random movement
    int smartZDir= 0; //direction when zombie in chasing movement
};

class smartZombies: public zombies{
public:
    smartZombies(StudentWorld* world, double x_cord, double y_cord);
    virtual void doSomething(); //smart zombie's specialized do something
};

class dumbZombies: public zombies{
public:
    dumbZombies(StudentWorld* world, double x_cord, double y_cord);
    virtual void doSomething(); //dumb zombie's specialized do something
};


class Goodies: public Actor{
public:
    Goodies(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh);
    virtual void doSomething(); //Goodie's specialized do something
    virtual bool collected(); //indicates whether penelope overlaps with a goodie
    virtual bool isGoodie(); //returns true since this is a goodie class
};
class vaccineGoodie: public Goodies{
public:
    vaccineGoodie(StudentWorld* world, double x_cord, double y_cord);
    virtual void doSomething(); //vaccine goodie's specialized do something
};

class gasCanGoodie: public Goodies{
public:
    gasCanGoodie(StudentWorld* world, double x_cord, double y_cord);
     virtual void doSomething(); //gasCanGoodie's specialized do something
};

class landmineGoodie: public Goodies{
public:
    landmineGoodie(StudentWorld* world, double x_cord, double y_cord);
     virtual void doSomething(); //landmine goodie's specialized do something
};

class Projectile: public Actor{
public:
    Projectile(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh);
    virtual void decTicks(); //the projectile's are only alive for two ticks
    virtual int getTicks(); //return the current tick that projectiles have
    virtual bool isProjectile(); //returns true since this is a projectile class
private:
    int m_ticks; //projectiles have 2 ticks until death
};

class vomit: public Projectile{
public:
    vomit(StudentWorld* world, double x_cord, double y_cord, int direct);
    virtual void doSomething(); //vomit's specialized do something
    
};

class flame: public Projectile{
public:
    flame(StudentWorld* world, double x_cord, double y_cord, int direct);
        virtual void doSomething(); //flame's specialized do something
     virtual bool isFlame(); //returns true since this is a flame class
    virtual bool canDie(); //when something touches flame, they can die
};

class departure: public Actor{
public:
    departure(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh);
};

class Pit: public departure{
public:
    Pit(StudentWorld* world, double x_cord, double y_cord);
    virtual bool isPit(); //returns true since this is a pit class
    virtual void doSomething(); //pit's specialized do something
    virtual bool canDie(); //when something touches pit it can die
};

class Exit: public departure{
public:
    Exit(StudentWorld* world, double x_cord, double y_cord);
    virtual bool isExit(); //returns true since this is an exit class
    virtual void doSomething(); //exit's specialized do something
};


class landmine:public Actor{
public:
    landmine(double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh);
    virtual void doSomething(); //landmine's specialized do something
    void decSafeCount(); //decreases landmine's safe count
    int getSafeCount(); //returns the current safe count
    virtual bool isLandmine(); //returns true since this is a landmine class
    virtual bool canDie(); //touching landmine can kill someone
private:
    int m_safeCount; //landmines have 30 safecount before activated
};


class wall: public Actor
{
public:
    wall(double x_cord, double y_cord, int dir, StudentWorld* world);
    virtual bool isWall(); //returns true since it's wall class
        virtual bool isBarrier(); // wall can block movements of all stuff
    virtual void doSomething(); //wall's do something does nothing
private:
};

#endif
