#include "Actor.h"
#include "StudentWorld.h"

//constructor of actor with initial infection count of 0
Actor::Actor(int ID, double x_cord, double y_cord, int dir, StudentWorld* world, int depth ):GraphObject(ID, x_cord, y_cord, dir, depth), m_world(world){
    m_infectionCount = 0;
}

void Actor::changingTicks(){
    if (m_changing == false){
        m_changing = true;
    }else{
        m_changing = false;
    }
}

bool Actor::isFlame(){ return false; } //all actors except flames are not flames

bool Actor::getChanging(){ return m_changing; } //return the bool value of current tick to determine movement
void Actor::setAlive(){ livingStatus = true; }//set actors starting at alive state

void Actor::setDead(){ livingStatus = false; } //make someone's alive status false

bool Actor::isCitizen(){ return false; } //Check if someone is a citizen

bool Actor::canDie(){ return false; } //Determine if an object can cause someone to die

bool Actor::infected(){ return infect; } //determine if someone is infected

void Actor::setInfect(){ infect = true; } //set the infection status

bool Actor::isBarrier(){ return false; } //Determine the objects that aren't suppose to run into one another

bool Actor::isWall(){ return false; }  //To determine whether something is wall

bool Actor::isLandmine(){ return false; } //To determine whether something is landmine

bool Actor::isExit(){ return false; } //For purpose of checking any human touches an exit

bool Actor::isZombie(){ return false; } //Determine if sth is a zombie

void Actor::deInfect(){ infect = false; } //if applied vaccine, set the infection count back to 0
StudentWorld* Actor::world(){ return m_world; } //returns the pointer to the world it's in

bool Actor::aliveStatus(){ return livingStatus; } //returns the alive status of someone

void Actor::setActive(){ m_active= true; } //for setting the landmine active

bool Actor::getActive(){ return m_active; } //get the active status of landmine

void Actor::deInfection(){ m_infectionCount = 0; } //if applied vaccine, set the infection count back to 0
void Actor::incInfection(){ m_infectionCount++; } //Increase someone's infection count if infected
int Actor::getInfection(){ return m_infectionCount; } //get the currect infection count


//constructor of livingClass with initial state of alive and depth of 0
livingClass::livingClass(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh):Actor(IDD, x_cordd, y_cordd, dirr, worldd, 0){
    setAlive();
}

bool livingClass::isLiving(){ return true; } //indicate if something is either a citizen or penelope
bool livingClass::isBarrier(){ return true; } //citizen and penelope can block movements of other things that're also barriers

//constructor of penelope with initial number of goods to be all 0, and it's facing right with depth of 0
Penelope::Penelope(StudentWorld* world, double x_cord, double y_cord):livingClass(IID_PLAYER, x_cord, y_cord, 0, world, 0){
    m_landmines = 0;
    m_charges = 0;
    m_vaccine = 0;
}



void Penelope::doSomething(){
    //if penelope is dead, do nothing and just reture
    if(!aliveStatus()){
        return;
    }
    //if the infection status indicate that penelope is infected, the infection count increases by one every tick
    if (infected()){
        incInfection();
    }
    //when the infection count reaches 500, penelope loses a live
    if (getInfection() == 500){
        setDead();
    }
    //get the user input
    int instruct;
    if (world()->getKey(instruct)){
    switch (instruct) {
        
            //if not encounter barriers when user input up down left or right, move accordingly
        case KEY_PRESS_UP:
            setDirection(90);
            if (getY() < VIEW_HEIGHT &&!world()->encounterBarrier(getX(), getY()+4,getDirection(), this)){
                moveTo(getX(), getY()+4);
            }
            break;
        case KEY_PRESS_DOWN:
            setDirection(270);
            if(getY() >0 && !world()->encounterBarrier(getX(), getY()-4,getDirection(), this)){
                moveTo(getX(), getY()-4);
            }
            break;
        case KEY_PRESS_LEFT:
            setDirection(180);
             if(getX() > 0&& !world()->encounterBarrier(getX()-4, getY(),getDirection(), this)){
                moveTo(getX()-4, getY());
            }
            break;
        case KEY_PRESS_RIGHT:
            setDirection(0);
            if(getX() < VIEW_WIDTH && !world()->encounterBarrier(getX()+4, getY(),getDirection(), this)){
                moveTo(getX()+4, getY());
            }
            break;
            
            //if user has more than 0 landmines, deploy a landmine at the particular location where the player is at
        case KEY_PRESS_TAB:
            if (m_landmines>0){
                m_landmines--; //the player now has one less landmine
                world()->add(new landmine(getX(), getY(), 0,this->world(),1));
            }
            break;
            //if player has more than 0 vaccines, use it on the player and set the infection status back to false and infection count back to 0
        case KEY_PRESS_ENTER:
            if (m_vaccine > 0){
                deInfect();
                deInfection();
                m_vaccine--; //the player now has one less vaccine
            }
            break;
            //when presses space, if the player has more than one charges, try to fire the flames
        case KEY_PRESS_SPACE:
            if (m_charges > 0){
                m_charges--; //firing decreases the charges player has
                world()->playSound(SOUND_PLAYER_FIRE);
                //fire in the direction player is facing
                switch (getDirection()) {
                        //if not encounter barriers in the direction of firing, deploy new flame objects respectively with same direction as the player is facing
                    case right:
                        for (int i = 1; i <= 3 && !world()->flameCoord(getX()+i*SPRITE_WIDTH, getY()); i++){
                             world()->add(new flame(this->world(), getX()+i*SPRITE_WIDTH, getY(), this->getDirection()));

                        }
                        break;
                    case left:
                        for (int i = 1; i <= 3 && !world()->flameCoord(getX()-i*SPRITE_WIDTH, getY()); i++){
                            world()->add(new flame(this->world(), getX()-i*SPRITE_WIDTH, getY(), this->getDirection()));

                        }
                        break;
                    case up:
                        for (int i = 1; i <= 3 && !world()->flameCoord(getX(), getY() + i * SPRITE_HEIGHT); i++){
                            world()->add(new flame(this->world(), getX(), getY()+i*SPRITE_HEIGHT, this->getDirection()));
                            
                        }
                        break;
                    case down:
                        for (int i = 1; i <= 3 && !world()->flameCoord(getX(), getY() - i * SPRITE_HEIGHT); i++){
                             world()->add(new flame(this->world(), getX(), getY()-i*SPRITE_HEIGHT, this->getDirection()));
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        default:
            break;
    }
    }
    //if something deadly overlaps with penelope, the penelope is set to dead (I put the sound of dying is in the student world instead of here)
    if (world()->checkOverlapAny(this)){
        setDead();
    }
}

void Penelope::incCharges(){ m_charges+=5; } //when collected gas can, the charges should increase by 5
void Penelope::incLandmines(){ m_landmines+=2; } //if collected a goodie, landmine count increases by 2
int Penelope::getCharges(){ return m_charges; } //get the current charge penelope has

int Penelope:: getLandmines(){ return m_landmines; } //returns the landmine count

int Penelope::getVaccine(){ return m_vaccine; } //returns the current vaccine count

void Penelope::incVaccine(){ m_vaccine++; } //increase vaccine count when collected a vaccine goodie

//constructor of citizen with direction facing right and depth of 0
citizen::citizen(StudentWorld* world, double x_cord, double y_cord):livingClass(IID_CITIZEN, x_cord, y_cord, 0, world, 0){
}

void citizen::doSomething(){
    //since citizens do things every other tick (2nd, 4th, ...) changing ticks changes a movement determining boolean variable to opposite every tick, enabling citizens to move when the bool is true every other tick
    changingTicks();
    if (!aliveStatus()){
        return;
    }
    //if citizen is infected, increase its infection count every single tick (whether it's infected is set in student world instead of here)
    if (infected()){
        incInfection();
    }
    //if the citizen touches an exit and saved
    if (world()->CtExit(this)){
        setDead();
        world()->playSound(SOUND_CITIZEN_SAVED);
        world()->increaseScore(1500); //since I let Move() in studentworld decrease 1000 points when citizen dies, in this case citizen dying would increase 500 points, so in total the increased score should be 1500 so that when the point is deducted in Move() the change in score sums up to be 500
        return;
    }
    //if something deadly touches citizen, it dies
    if (world()->checkOverlapAny(this)){
        world()->playSound(SOUND_CITIZEN_DIE);
        setDead();
        return;
    }
    //when infection count of citizen reaches 500, it dies and there's 30 percent of chance it'll become smart zombie and 70 percent it'll become dumb zombie
    if (getInfection() == 500){
        setDead();
        int randvar = randInt(1, 10);
        if (randvar >= 1 && randvar <=3){
            world()->add(new smartZombies(world(), getX(), getY()));
        }else{
            world()->add(new dumbZombies(world(), getX(), getY()));
        }
        world()->playSound(SOUND_ZOMBIE_BORN);
        return;
    }
    
    //if the tick is currently the "true" tick, the citizen will see if it want to dodge zombies or chase penelope given distance is less than 80 pixels
    if (getChanging()){
        world()->citizenMovePene(this);
    }
}


//since this is a citizen class, the identification of isCitizen() comes out to be true
bool citizen::isCitizen(){ return true; }

//zombie's constructor with random movement's movement plan of 0 and chasing movement plan of 0. It has depth of 0, facing right
zombies::zombies(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh):Actor(IDD, x_cordd, y_cordd, dirr, worldd, depthh){
    setAlive();
    m_movementPlan = 0;
    eightyMovementPlan = 0;
}

bool zombies::isZombie(){ return true; } //returns true since this is a zombie class

int zombies::getMovementPlan(){ return m_movementPlan;} //returns the steps movement plan has

void zombies::setMovementPlan(){ m_movementPlan = randInt(3, 10);} //set movement plan to a random value between 3 and 10
int zombies::getEighty(){ return eightyMovementPlan;} //movement plan when smart zombie chases someone
void zombies::setEighty(){eightyMovementPlan = randInt(3, 10);} //set the chasing movement plan to 0
void zombies::decEighty(){ eightyMovementPlan--;} //decrease the chasing movement plan by 1

void zombies::resetEighty(){eightyMovementPlan = 0;} // reset the chasing movement plan to 0

bool zombies::isBarrier(){ return true; } //zombies can't run into other barrier objects such as penelope and citizen

void zombies::decMovementPlan(){ m_movementPlan--;} //decrease movement plan when moved

void zombies::resetMovementPlan(){ m_movementPlan = 0;} //sets movement plan to 0

void zombies::setDir(int someInt){ smartZDir = someInt;} //set the direction of movement when smart zombie is chasing
int zombies::getDir(){ return smartZDir;} //get the direction smart zombie is moveing toward

//makes zombies move randomly
void zombies::superRandomMove(){
    //if it's not alive, don't do anything
    if (!aliveStatus()){
        return;
    }
    //when movement plan is 0, try to determine a random direction to move toward
    if (m_movementPlan == 0){
        direct = randInt(1, 4);
        setMovementPlan();
        switch (direct) {
            case 1:
                direct = 0;
                break;
            case 2:
                direct = 90;
                break;
            case 3:
                direct = 180;
                break;
            case 4:
                direct = 270;
                break;
            default:
                break;
        }
    }
    
    //set the direction it's moving toward for the current movement plan
    setDirection(direct);
    
    //move to the chosen direction accordingly, if the direction is available, move to the corresponding position and decrease the movementPlan steps
    
    //is no longer available, set the movement plan to zero so that function will choose a new direction the next tick
    switch (direct) {
        case up:
            if (!world()->encounterBarrier(getX(), getY()+1, getDirection(), this)){
                moveTo(getX(), getY()+1);
            }else{
                m_movementPlan = 0;
                break;
            }
            m_movementPlan--;
            break;
        case down:
            if (!world()->encounterBarrier(getX(), getY()-1, getDirection(), this)){
                moveTo(getX(), getY()-1);
            }else{
                m_movementPlan = 0;
                break;
            }
            m_movementPlan--;
            break;
        case left:
            if (!world()->encounterBarrier(getX()-1, getY(), getDirection(), this)){
                moveTo(getX()-1, getY());
            }else{
                m_movementPlan = 0;
                break;
            }
            m_movementPlan--;
            break;
        case right:
            if (!world()->encounterBarrier(getX()+1, getY(), getDirection(), this)){
                moveTo(getX()+1, getY());
            }else{
                m_movementPlan = 0;
                break;
            }
            m_movementPlan--;
            break;
        default:
            break;
    }
    
}

//detects whether to vomit or not and whether there's citizen or penelope in front of it
bool zombies::detectionVomit(){
    //if not alive, just don't do anything
    if (!aliveStatus()){
        return false;
    }
    //there's one in three chance that it's going to vomit
        int rand = randInt(1, 3);
        if (rand == 1){
            //the vomitcoord in student world helps determine if there's a person in front of the zombie, returns true if it does
            if (world()->vomitCoord(getX(), getY(), getDirection())){
                //if it vomitted, play the destinated sound
                
                //add the vomit in front of zombie with initial direction of whatever direction it's facing
                world()->playSound(SOUND_ZOMBIE_VOMIT);
                switch (getDirection()) {
                    case up:
                        world()->add(new vomit(world(), getX(), getY() + SPRITE_HEIGHT, this->getDirection()));
                        break;
                    case down:
                        world()->add(new vomit(world(), getX(), getY() - SPRITE_HEIGHT, this->getDirection()));
                        break;
                    case left:
                        world()->add(new vomit(world(), getX()-SPRITE_WIDTH, getY(), this->getDirection()));
                        break;
                    case right:
                        world()->add(new vomit(world(), getX()+SPRITE_WIDTH, getY(), this->getDirection()));
                        break;
                    default:
                        break;
                }
                return true;
            }
        }
    return false;
}

//smartzombies constructed facing right with depth of 0
smartZombies::smartZombies(StudentWorld* world, double x_cord, double y_cord):zombies(IID_ZOMBIE, x_cord, y_cord, 0, world,0){
}
void smartZombies::doSomething(){
    //changing ticks to determine the ticks it should move
    changingTicks();
    //if it's not alive, do nothing and return
    if (!aliveStatus()){
        return;
    }
    //if smart zombie is overlapped by something deadly, let it die and the score is increased by 1000 (additional to the increase of 1000 in student world since smartzombies worth 1000 points more than dumb zombies, and student world only adds 1000 whenever zombie dies)
    if (world()->checkOverlapAny(this)){
        setDead();
        world()->playSound(SOUND_ZOMBIE_DIE);
        world()->increaseScore(1000);
    }
    //if it's the moving tick
    if (getChanging()){
        //if the zombie vomits, return and do nothing more
        if (detectionVomit()){
            return;
        }
        //if the distance to citizen or penelope is less than 80 pixels, the zombie chases (the movement is coded in student world)
        if (world()->distLiving(this) <= 80*80){
            //zombie tries to chase something
                world()->zombieMoveLiving(this);
            return;
        }else{
            superRandomMove();
        }
        return;
    }
    
}

//dumbZombies constructed with initial direction of right and depth of 0
dumbZombies::dumbZombies(StudentWorld* world, double x_cord, double y_cord):zombies(IID_ZOMBIE, x_cord, y_cord, 0, world,0){
}

void dumbZombies::doSomething(){
    //changing ticks provides the ticks dumbZombies should move
    changingTicks();
    //if it's not alive, return and do nothing more
    if (!aliveStatus()){
        return;
    }
    //if it's currently a moving tick
    if (getChanging()){
        //if dumb zombie is hit by something deadly, there's a one in ten chance that it'll fling a vaccine goodie next to one side of its bounding box chosen by random
        //creation of vaccine in each direction chosen shown as follows
        if (world()->checkOverlapAny(this)){
            int vac = randInt(1, 10);
            if (vac == 1){
                int vacDir = randInt(0, 3);
                if(vacDir == 0 && !world()->encounterBarrier(getX()+2*SPRITE_WIDTH, getY(), 0, this)){
                    world()->add(new vaccineGoodie(world(), getX()+SPRITE_WIDTH, getY()));
                }
                if(vacDir == 1 && !world()->encounterBarrier(getX(), getY()+2*SPRITE_HEIGHT, 90, this)){
                    world()->add(new vaccineGoodie(world(), getX(), getY()+SPRITE_WIDTH));
                }
                if(vacDir == 2 && !world()->encounterBarrier(getX()-SPRITE_WIDTH, getY(), 180, this)){
                    world()->add(new vaccineGoodie(world(), getX()-SPRITE_WIDTH, getY()));
                }
                if(vacDir == 3 && !world()->encounterBarrier(getX(), getY()-SPRITE_HEIGHT, 270, this)){
                    world()->add(new vaccineGoodie(world(), getX(), getY() - SPRITE_HEIGHT));
                }
            }
            //the zombie dies
            world()->playSound(SOUND_ZOMBIE_DIE);
            setDead();
            return;
       }
        //if the zombie vomits, return and do nothing more
        if (detectionVomit()){
            return;
        }
        //move randomly in whatever direction possible
        superRandomMove();
    }else{
        //if the current tick isn't suppose to move, return immediately
        return;
    }
}


//constructor for goodies
Goodies::Goodies(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh):Actor(IDD, x_cordd, y_cordd, dirr, worldd, depthh){
    setAlive();
}
void Goodies::doSomething(){
}

//helper function for do something functionality shared by three all goodies
bool Goodies::collected(){
    //when goodie is collected by penelope, increase score by 50 and plays the destinated sound
    if (world()->checkOverlap(this)){
        world()->increaseScore(50);
        world()->playSound(SOUND_GOT_GOODIE);
        setDead();
        return true;
    }
    return false;
}
bool Goodies::isGoodie(){
    return true;
}
vaccineGoodie::vaccineGoodie(StudentWorld* world, double x_cord, double y_cord):Goodies(IID_VACCINE_GOODIE, x_cord, y_cord, 0, world, 1){
    
}
void vaccineGoodie::doSomething(){
    //if not alive, return and do nothing more
    if (!aliveStatus()){
        return;
    }
    //if it's overlapped by something deadly, set its condition to dead
    if (world()->checkOverlapAny(this)){
        setDead();
    }
    //if it's touched by penelope, penelope's vaccine count is increased by one
    if (collected()){
        world()->incVac();
    }
}

//gasCan constructed with initial direction of right and depth of 1
gasCanGoodie::gasCanGoodie(StudentWorld* world, double x_cord, double y_cord):Goodies(IID_GAS_CAN_GOODIE, x_cord, y_cord, 0, world, 1){
    
}
void gasCanGoodie::doSomething(){
    //if not alive, return and do nothing more
    if (!aliveStatus()){
        return;
    }
    //if overlapped by something deadly, set itself to dead
    if (world()->checkOverlapAny(this)){
        setDead();
    }
    //if collected the charge is increased by 5
    if (collected()){
        world()->incCh();
    }
}
//landmine constructed with initial direction of right and depth of 1
landmineGoodie::landmineGoodie(StudentWorld* world, double x_cord, double y_cord):Goodies(IID_LANDMINE_GOODIE, x_cord, y_cord, 0, world, 1){
    
}
void landmineGoodie::doSomething(){
    // if not alive return and do nothing more
    if (!aliveStatus()){
        return;
    }
    //if overlapped by something deadly, set itself to dead
    if (world()->checkOverlapAny(this)){
        setDead();
    }
    //if it's collected, increase the landmine count of player
    if (collected()){
        world()->incLan();
    }
}

//landmine constructed with safe count of 30 in an alive state, facing right with depth of 1
landmine::landmine(double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh):Actor(IID_LANDMINE, x_cordd, y_cordd, 0, worldd, 1){
    m_safeCount = 30;
    setAlive();
}
bool landmine::isLandmine(){ return true; } //returns true since this is a landmine class

void landmine::decSafeCount(){ m_safeCount--; } //decreases landmine's safe count

int landmine::getSafeCount(){ return m_safeCount; } //returns the current safe count

//landmine can kill someone if activated
bool landmine::canDie(){
    if (getActive()){
        return true;
    }else{
        return false;
    }
}


void landmine::doSomething(){
    //if not alive, do nothing more
    if(!aliveStatus()){
        return;
    }
    //if it's not activated
    if (!getActive()){
        //the safeCount decreases every tick and once it's 0 , it's activated
        decSafeCount();
        if (getSafeCount() == 0){
            setActive();
        }
        //if flame touches a non-activated landmine
        if (world()->flameNActiveLD(this)){
            setActive(); //the landmine is directed activated and dies immediately
            setDead();
            world()->add(new Pit(this->world(), getX(), getY())); //death of landmine creates a pit on the floor
            world()->playSound(SOUND_LANDMINE_EXPLODE); //the sound of landmine exploding
            //deploy flames in the eight spots next to it and the location it's at if possible. Flame has direction of up when constructed in this case
            for (int i = -1; i < 2; i++){
                for (int j = -1; j < 2; j++){
                    if (!world()->flameCoord(getX()+i*SPRITE_WIDTH, getY()+j*SPRITE_HEIGHT)){
                        world()->add(new flame(this->world(), getX()+i*SPRITE_WIDTH, getY()+j*SPRITE_WIDTH, up));
                   
                    }
                }
            }
            //when landmine dies, the world removes it so that the world() wouldn't double kill a landmine and access it's invalid location
            world()->remove(this);
        }
        
    }else{
        //if anything touches landmine, the land mine explodes with eight flames surrounding it and and flame at its location
    if (world()->checkOverlapAny(this) || world()->citizenZombieParadox(this)){
        setDead();
        world()->add(new Pit(this->world(), getX(), getY())); //produces a pit when exploded
        world()->playSound(SOUND_LANDMINE_EXPLODE);
            for (int i = -1; i < 2; i++){
                for (int j = -1; j < 2; j++){
                    if (!world()->flameCoord(getX()+i*SPRITE_WIDTH, getY()+j*SPRITE_HEIGHT)){
                        world()->add(new flame(this->world(), getX()+i*SPRITE_WIDTH, getY()+j*SPRITE_WIDTH, up));
                    }
                }
            }
        //tell world to remove landmine
        world()->remove(this);
    }
    }
    }


//projectile constructed with 2 ticks initially and direction specified when constructed
Projectile::Projectile(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh):Actor(IDD, x_cordd, y_cordd, dirr, worldd, depthh){
    setAlive();
    m_ticks = 2;
}

bool Projectile::isProjectile(){ return true; } //returns true since this is a projectile class
void Projectile::decTicks(){ m_ticks--; } //the projectile's are only alive for two ticks

int Projectile::getTicks(){ return m_ticks; } //return the current tick that projectiles have

//vomit constructed with direction specified when constructed in the parameter and depth of 0
vomit::vomit(StudentWorld* world, double x_cord, double y_cord, int direct):Projectile(IID_VOMIT, x_cord, y_cord, direct, world, 0){
    
}

void vomit::doSomething(){
    //if nor alive, do nothing more
    if (!aliveStatus()){
        return;
    }
    //if anything touches vomit, ask world to set the object's infection status to true
    world()->setLivingInfect(this);
    decTicks();
    //when tick reaches 0 , it's dead
    if (getTicks() == 0){
        setDead();
    }
    
}
//flame constructed with initial direction constructed in parameter during construction and depth of 0
flame::flame(StudentWorld* world, double x_cord, double y_cord, int direct):Projectile(IID_FLAME, x_cord, y_cord, direct, world, 0){
    
}
void flame::doSomething(){
    //if not alive, do nothing more
    if (!aliveStatus()){
        return;
    }
    //it's dead after two ticks
    //doesn't have to check if it touches someone since other living things' classes checks if they touch flames themselves
    decTicks();
    if (getTicks() == 0){
        setDead();
    }
}
bool flame::isFlame(){ return true; } //returns true since this is a flame class

bool flame::canDie(){ return true; } //when something touches flame, they can die

//constructor departure that's the parent class for pit and exit
departure::departure(int IDD, double x_cordd, double y_cordd, int dirr, StudentWorld* worldd, int depthh):Actor(IDD, x_cordd, y_cordd, dirr, worldd, depthh){
    setAlive();
}

//pit created with initial direction of right and depth of 1
Pit::Pit(StudentWorld* world, double x_cord, double y_cord):departure(IID_PIT, x_cord, y_cord, 0, world, 1){
}

void Pit::doSomething(){
    //if not alive, do nothing more
    if(!aliveStatus()){
        return;
    }
    //if penelope touches this, set dead (others like citizens or zombies check themselves)
    if (world()->checkOverlap(this)){
        world() ->setPeneDead();
        setDead();
    }
}

bool Pit::canDie(){ return true; } //when something touches pit it can die

bool Pit::isPit(){ return true; } //returns true since this is a pit class

//exit constructed with direction of right and depth of 0
Exit::Exit(StudentWorld* world, double x_cord, double y_cord):departure(IID_EXIT, x_cord, y_cord, 0, world, 1){
    
}
bool Exit::isExit(){ return true; } //returns true since this is an exit class

void Exit::doSomething(){ } //exits don't do much, just sit there, and people will detect whether they touched the exits themselves

//wall constructed with depth of 0 and direction of right
wall::wall(double x_cord, double y_cord, int dir, StudentWorld* world):Actor(IID_WALL, x_cord, y_cord, 0, world, 0){
    setAlive();
}

bool wall::isBarrier(){ return true; } // wall can block movements of all stuff

bool wall::isWall(){ return true; } //returns true since it's wall class

void wall::doSomething(){ } // walls don't do much except sitting there
