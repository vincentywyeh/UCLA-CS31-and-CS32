#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <cmath>
#include <string>
#include <sstream>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath),lev(assetPath)
{
}

//destructor for student world
StudentWorld::~StudentWorld(){
    if (pene != nullptr){ //delete penelope if it's not already destructed
        delete pene;
        pene = nullptr;
    }
    //delete all actors
    while (!actors.empty()) {
        delete actors.back();
        actors.pop_back();
    }
}

int StudentWorld::init()
{
    //loading current levels
    ostringstream levelName;
    levelName.setf(ios::fixed);
    levelName.precision(2);
    if (getLevel() < 10){
        levelName << "level0"<<getLevel() << ".txt";
    }else{
        levelName << "level" <<getLevel()<<".txt";
    }
    string realLevelName = levelName.str();
    
    //string levelFile = "level01.txt";
    lev.loadLevel(realLevelName);
    Level::LoadResult result = lev.loadLevel(realLevelName);
    //if player completed all levels or reached 100, return GWSTATUS_PLAYER_WON
    if (result == Level::load_fail_file_not_found || getLevel() == 100){
        return GWSTATUS_PLAYER_WON;
    }
    //if level in bad format, return an error value
    else if (result == Level::load_fail_bad_format){
        return GWSTATUS_LEVEL_ERROR;
    }
    else if (result == Level::load_success){
        
        //construct actors at respective grids at the particular locations
        for (int i = 0; i < 16; i++){
            for (int j = 0; j < 16; j++){
                Level::MazeEntry ge = lev.getContentsOf(i,j);
                //determines each types of characters and deploy at certain locations
                switch (ge) {
                    case Level::player:
                        pene = new Penelope(this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT);
                        break;
                    case Level::wall:
                        actors.push_back(new wall(i*SPRITE_WIDTH,j*SPRITE_HEIGHT,0,this));
                        break;
                    case Level::citizen:
                        m_people++;
                        actors.push_back(new citizen(this, i*SPRITE_WIDTH,j*SPRITE_HEIGHT));
                        break;
                    case Level::dumb_zombie:
                        actors.push_back(new dumbZombies(this, i * SPRITE_WIDTH, j*SPRITE_HEIGHT));
                        break;
                    case Level::smart_zombie:
                        actors.push_back(new smartZombies(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                        break;
                    case Level::vaccine_goodie:
                        actors.push_back(new vaccineGoodie(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                        break;
                    case Level::gas_can_goodie:
                        actors.push_back(new gasCanGoodie(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                        break;
                    case Level::landmine_goodie:
                        actors.push_back(new landmineGoodie(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                        break;
                    case Level::pit:
                        actors.push_back(new Pit(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                        break;
                    case Level::exit:
                        actors.push_back(new Exit(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                        break;
                    default:
                        break;
                }
                
                
            }
        }
    }
    //after deploying, continue the game
    return GWSTATUS_CONTINUE_GAME;
}




int StudentWorld::move()
{
    //used to store stats line
    ostringstream oss;
    oss.setf(ios::fixed);
    oss.precision(2);
    int numDigit = 0;
    int scoreVal = getScore();
    //calculate the number of digits of scores
    while (scoreVal != 0) {
        scoreVal/=10;
        numDigit++;
    }
    //take absolute value of the score, (since negative symbol is later added to the string)
    int realScore = abs(getScore());
    string digitHolder= "";
    //if the score is positive, determine the 0's needed in front of the score
    if (getScore()>= 0){
        if (getScore() == 0){
            digitHolder = "00000";
        }
        if (numDigit == 2){
            digitHolder = "0000";
        }
        if (numDigit == 3){
            digitHolder = "000";
        }
        if (numDigit == 4){
            digitHolder = "00";
        }
        if (numDigit == 5){
            digitHolder = "0";
        }
    }else{
        //if the score is negative, determine the number of 0's with - symbol in front of it
        if (numDigit == 3){
            digitHolder = "-00";
        }
        if (numDigit == 4){
            digitHolder = "-0";
        }
        if (numDigit == 5){
            digitHolder = "-";
        }
    }
    //store stats into oss according to order described by spec
    oss << "Score: "<< digitHolder << realScore << "  " << "Level: " << getLevel() << "  "<<"Lives: " << getLives() << "  " << "Vaccines: " << pene->getVaccine() << "  " << "Flames: " << pene->getCharges() << "  " << "Mines: " << pene->getLandmines() << "  " << "Infected: " << pene->getInfection();
    //convert oss to string and display it
    string line = oss.str();
    setGameStatText(line);
    if (!pene ->aliveStatus()){
        decLives();
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
        
    }
    //iterate the actor vector, and if actors are alive, let it do something
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if ((*it) -> aliveStatus()){
            (*it) -> doSomething();
        }
        it++;
    }
    //if penelope is constructed but is not alive, return GWSTATUS_PLAYER_DIE and play destinated sound
    if (pene != nullptr && !pene ->aliveStatus()){
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    //if penelope is alive, do something
    if (pene->aliveStatus()){
        pene->doSomething();
    }
    
    //iterate through actors and delete actors that died
    vector<Actor*>::iterator itt = actors.begin();
    while (itt != actors.end()){
        if (!(*itt)->aliveStatus()){
            if ((*itt) -> isCitizen()){
                //if citizen dies, score -1000 (if dies because exitting, the score is already added 1500 in citizen's doSomething so that change sums up to 500)
                increaseScore(-1000);
                decPeople();
            }
            //killing a zombie increases score by 1000 (smartZombie dies with 1000 additional points that's encountered for in smartzombie class)
            if ((*itt)->isZombie()){
                increaseScore(1000);
            }
            if (!(*itt) -> isLandmine()){
                delete (*itt);
                itt = actors.erase(itt);
            }
        }else{
            itt++;
        }
    }
    
    //if no people left in the space and penelope touches exit, return finished level
    if (getPeople() == 0 && tExit()){
        playSound(SOUND_LEVEL_FINISHED);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}


//iterate through the actor vector to find destined actor (ac) to remove
void StudentWorld::remove(Actor *ac){
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if ((*it) == ac){
            delete (*it);
            it = actors.erase(it);
            break;
        }
        it++;
    }
}

//see if penelope intersects exit by iterating through the vector in search for overlap of exit
bool StudentWorld::tExit(){
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()){
        if ((*it)->isExit()){
            if (checkOverlap(*it)){
                return true;
            }
        }
        it++;
    }
    return false;
}

//check if flames overlapped landmines so that we can remove landmine in advance before move() access invalid pointer (flamestuff parameter is pretty much landmine)
bool StudentWorld::flameNActiveLD(Actor *flameStuff){
    vector<Actor*>::iterator itt = actors.begin();
    while (itt!= actors.end()) {
        if ((*itt)->isFlame()){
            if (checkOverlapVm((*itt), flameStuff)){
                return true;
            }
        }
        itt++;
    }
    return false;
}

//check if citizen touches exit and exits by iterating through the vector
bool StudentWorld::CtExit(Actor *Cit){
    vector<Actor*>::iterator it = actors.begin();
    while (it!= actors.end()) {
        if ((*it)->isExit() && (*it)->aliveStatus()){
            if (checkOverlapVm(Cit, (*it))){
                return true;
            }
        }
        it++;
    }
    return false;
}


//cleanup() for the system to call
void StudentWorld::cleanUp()
{
    if (pene != nullptr){
        delete pene;
        pene = nullptr;
    }
    while (!actors.empty()) {
        delete actors.back();
        actors.pop_back();
    }
}

//add a new acotor to the vector
void StudentWorld::add(Actor* sth){ actors.push_back(sth); }
//increases charges penelope has
void StudentWorld::incCh(){ pene->incCharges(); }
//increases vacine penelope has
void StudentWorld::incVac(){ pene->incVaccine(); }
//increases landmine penelope has
void StudentWorld::incLan(){ pene->incLandmines(); }
//kill penelope
void StudentWorld::setPeneDead(){ pene->setDead(); }

//check if penelope or citizen is infected by checking on penelope and iterate through the vector to find citizen
void StudentWorld::setLivingInfect(vomit* vomitStuff){
    if (checkOverlap(vomitStuff)){
        pene->setInfect();
        //only play sound when penelope is first infected
        if (pene->getInfection() == 0){
            playSound(SOUND_CITIZEN_INFECTED);
        }
    }
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if ((*it)->isCitizen()){
            if (checkOverlapVm((*it), vomitStuff)){
                (*it)->setInfect();
                //only play sound when citizen is first infected
                if ((*it)->getInfection() == 0){
                    playSound(SOUND_CITIZEN_INFECTED);
                }
            }
        }
        it++;
    }
}

//check of two objects in the parameter overlaps one another
bool StudentWorld::checkOverlapVm(Actor *random, Actor* vom){
    if ((pow(random->getX() - (vom)->getX(), 2) + pow(random->getY()-(vom)->getY(), 2)) <= 10*10 ){
        return true;
    }
    return false;
}
//check if something overlaps penelope
bool StudentWorld::checkOverlap(Actor *sth){
    if ((pow(sth->getX() - (pene)->getX(), 2) + pow(sth->getY()-(pene)->getY(), 2)) <= 10*10 ){
        return true;
    }
    return false;
}
//iterating through the actor to find zombues and make sure zombies don't over lap living organisms
bool StudentWorld::citizenZombieParadox(Actor *land){
    vector<Actor*>::iterator it = actors.begin();
    while (it!=actors.end()) {
        if ((*it)->isCitizen() || (*it)->isZombie()){
            if (checkOverlapVm((*it), land)){
                return true;
            }
        }
        it++;
    }
    return false;
}

//if something is overlapped by deadly stuff, the function returns true
bool StudentWorld::checkOverlapAny(Actor *attacked){
    //iterate through the vector to find things that are deadly overlapping attacked stuff
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if (*it == attacked){
            it++;
            if (it == actors.end()){
                break;
            }
        }
        if ((pow(attacked->getX() - (*it)->getX(), 2) + pow(attacked->getY()-(*it)->getY(), 2)) <= 10*10 ){
            if (((*it)->canDie() || (*it)->isWall())){
                return true;
            }
        }
        it++;
    }
    return false;
    
}

//make sure unoverlappable thing's bounding box don't intersect one another
bool StudentWorld::encounterBarrier(double x_axis, double y_axis, int dir, Actor* mySelf){
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if (((*it)->isBarrier()) && (*it) != mySelf ){
            double xhold = (*it)->getX(); //x axis of unoverlappable things
            double yhold = (*it)->getY(); //y axis of unoverlappable things
            double penex = pene->getX(); //penelope's x axis
            double peney = pene->getY(); //penelope's y axis
            double divx = abs((*it)->getX()- x_axis); //absolute value difference in x axis
            double divy = abs((*it)->getY()-y_axis); //absolute value difference in y axis
            double divpenx = abs(penex-x_axis); //absolute. difference to penelope x axis
            double divpeny = abs(peney-y_axis);// absolute/ difference to penelope y axis
            //if facing up, check if the upper bound overlaps some barrier's y axis
            if (dir == 90){
                if ((y_axis+SPRITE_HEIGHT > yhold && yhold > y_axis && divx <SPRITE_WIDTH)
                    || (y_axis+SPRITE_HEIGHT > peney && peney > y_axis && divpenx <SPRITE_WIDTH && mySelf!=pene)
                    ){
                    return true;
                }
            }
            //if facing down, check if the y axis overlaps some barrier's upper bound
            if (dir == 270){
                if ((y_axis < yhold+SPRITE_HEIGHT && yhold < y_axis && divx <SPRITE_WIDTH)
                    || (y_axis < peney+SPRITE_HEIGHT && peney < y_axis && divpenx<SPRITE_WIDTH && mySelf != pene)
                    ){
                    return true;
                }
            }
            //if facing left, check if the x axis overlaps some barrier's rightmost bound
            if (dir == 180){
                if ((x_axis< xhold+SPRITE_WIDTH && xhold < x_axis && divy <SPRITE_HEIGHT)
                    || (x_axis< penex+SPRITE_WIDTH && penex < x_axis && divpeny<SPRITE_HEIGHT && mySelf != pene)
                    ){
                    return true;
                }
            }
            //if facing right, check if the rightmost bound overlaps some barrier's x axis
            if (dir == 0){
                if ((x_axis+SPRITE_WIDTH > xhold && xhold > x_axis && divy <SPRITE_HEIGHT)
                    || (x_axis+SPRITE_WIDTH > penex && penex > x_axis && divpeny <SPRITE_HEIGHT && mySelf != pene)
                    ){
                    return true;
                }
            }
        }
        it++;
        
    }
    //if not overlapping any, return false
    return false;
}

//compute distance square of a set of x y axis
double StudentWorld::distsquare(double ax, double ay, double bx, double by){
    return pow(ax-bx, 2)+pow(ay-by, 2);
}


bool StudentWorld::vomitCoord(double x_axis, double y_axis, int dir){
    double xcor = pene->getX();
    double ycor = pene->getY();
    int tsq = 10*10;
    //check if distance of penelope is above or below zombie and within 10 pixels of its computed vomit coordinate
    if ((dir == 90 && (y_axis < ycor&& distsquare(xcor, ycor, x_axis, y_axis+SPRITE_HEIGHT) <= tsq)) || (dir == 270 && (y_axis > ycor&&distsquare(xcor, ycor, x_axis, y_axis-SPRITE_HEIGHT) <=tsq))){
        return true;
    }
     //check if distance of penelope is on right or left zombie and within 10 pixels of its computed vomit coordinate
    if ((dir == 0 && (x_axis < pene->getX() && distsquare(xcor, ycor, x_axis+SPRITE_WIDTH, y_axis) <= tsq)) || (dir == 180 && (x_axis > pene->getX()&&distsquare(xcor, ycor, x_axis-SPRITE_WIDTH, y_axis)<=tsq))){
        return true;
    }
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        double xxcor = (*it)->getX();
        double yycor = (*it)->getY();
        if ((*it)->isCitizen()){
            //check if distance of citizen is above or below zombie and within 10 pixels of its computed vomit coordinate
            if ((dir == 90 && (y_axis < yycor &&distsquare(xxcor, yycor, x_axis, y_axis +SPRITE_HEIGHT)<= tsq)) || (dir == 270 && (y_axis > yycor&&distsquare(xxcor, yycor, x_axis, y_axis-SPRITE_HEIGHT) <= tsq))){
                return true;
            }
            //check if distance of citizen is on left or right of zombie and within 10 pixels of its computed vomit coordinate
            if ((dir == 0 && (x_axis < (*it)->getX()&& distsquare(xxcor, yycor, x_axis +SPRITE_WIDTH, y_axis) <= tsq)) || (dir == 180 && (x_axis > (*it)->getX()&& distsquare(xxcor, yycor, x_axis-SPRITE_WIDTH, y_axis) <= tsq))){
                return true;
            }
        }
        it++;
    }
    return false;
}

//make sure flames don't penatrate walls
bool StudentWorld::flameCoord(double x_axis, double y_axis){
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if (((*it)->isWall())&& ((pow(x_axis - (*it)->getX(), 2) + pow(y_axis-(*it)->getY(), 2)) < SPRITE_WIDTH*SPRITE_HEIGHT )){
            return true;
        }
        it++;
    }
    return false;
}



//returns the closest distance to either citizen or penelope
double StudentWorld::distLiving(Actor *sth){
    //check distance to penelope
    double tempdist = distsquare(pene -> getX(), pene->getY(), sth->getX(), sth->getY());
    //iterating through the actor vector to find the smallest distance to citizens
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        double holder = -1;
        if ((*it)->isCitizen()){
            holder = distsquare((*it)->getX(), (*it)->getY(), sth->getX(), sth->getY());
        }
        //if distance previously set by penelope or other citizens is more than the newly found distance, the tepdist is set to the particular value
        if (holder != -1 && holder < tempdist){
            tempdist = holder;
        }
        it++;
    }
    return tempdist;
}


void StudentWorld::zombieMoveLiving(zombies *smartZombie){
    double ZombX = smartZombie->getX();
    double ZombY = smartZombie->getY();
    double peneX = pene->getX();
    double peneY = pene->getY();
    double dist_p = distsquare(ZombX, ZombY, peneX, peneY);
    double dist_closest = -1;
    Actor* closestCit = pene;
    //iterating through actors to find the distance and pointer to the closes citizen or penelope
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if ((*it)->isCitizen()){
            double distholder = distsquare((*it)->getX(), (*it)->getY(), ZombX, ZombY);
            if (dist_closest == -1){
                dist_closest = distholder;
            }
            //if previously decided closest object isn't the real closest, reset the distance and pointer to the appropriate pointer and distance value
            if (distholder < dist_closest){
                dist_closest = distholder;
                closestCit = (*it);
            }
        }
        it++;
    }
    
    Actor* realClosest = closestCit;
    double realDist = dist_closest;
    //see if penelope is closer than the closest citizen
    if (dist_p < dist_closest){
        realClosest = pene;
    }
    if (dist_p < dist_closest){
        realDist = dist_p;
    }
    double realX = realClosest->getX();
    double realY = realClosest->getY();

    int dir1 = -1;//x coord dir
    int dir2 = -1;//y coord dir
    
    //if the movement plan when chasing reaches 0
    if (smartZombie->getEighty() ==0){
        //reset the direction to determine the random of two directions that will get closer to living things
        int realDir = -1;
        //set movement plan to a number between 3 and 10
        smartZombie->setEighty();
        //check the relative direction of citizen/ penelope with respect to the zombie (doesn't have to check same row/column since the citizen already checked and made the moves)
            if (realX - ZombX > 0 && realY - ZombY > 0){
                dir1 = 0;
                dir2 = 90;
            }
            else if (realX - ZombX > 0 && realY - ZombY < 0){
                dir1 = 0;
                dir2 = 270;
            }
            else if (realX - ZombX < 0 && realY - ZombY > 0){
                dir1 = 180;
                dir2 = 90;
            }
            else{
                dir1 = 180;
                dir2 = 270;
            }
        //randomly decide which of the two directions to move to
        if (realDir == -1){
            if (randInt(0, 1)){
                realDir = dir2;
                if (realDir == -1){
                    realDir = dir1;
                }
            }else{
                realDir = dir1;
                if (realDir == -1){
                    realDir = dir2;
                }
            }
        }
        //set the direction of movement plan to the direction eventually decided
        smartZombie->setDir(realDir);
    }
    
    //move in the direction decided by previous portion of this code
    
    switch (smartZombie->getDir()) {
            //in each direction, if will enounter barrier, set the movement plan to 0 so that the function will determine a new one during next available tick
            //move one pixels at a time until direction not available
            // similar for rest of the cases
        case 90:
            if (!encounterBarrier(ZombX, ZombY+1, 90, smartZombie)){
                smartZombie->setDirection(90);
                smartZombie->moveTo(ZombX, ZombY+1);
            }else{
                smartZombie->resetEighty();
                break;
            }
            smartZombie->decEighty();
            break;
        case 270:
            if (!encounterBarrier(ZombX, ZombY-1, 270, smartZombie)){
                smartZombie->setDirection(270);
                smartZombie->moveTo(ZombX, ZombY-1);
            }else{
                smartZombie->resetEighty();
                break;
            }
            smartZombie->decEighty();
            break;
        case 180:
            if (!encounterBarrier(ZombX-1, ZombY, 180, smartZombie)){
                smartZombie->setDirection(180);
                smartZombie->moveTo(ZombX-1, ZombY);
            }else{
                smartZombie->resetEighty();
                break;
            }
            smartZombie->decEighty();
            break;
        case 0:
            if (!encounterBarrier(ZombX+1, ZombY, 0, smartZombie)){
                smartZombie->setDirection(0);
                smartZombie->moveTo(ZombX+1, ZombY);
            }else{
                smartZombie->resetEighty();
                break;
            }
            smartZombie->decEighty();
            break;
        default:
            break;
    }
}

//citizen's movement
void StudentWorld::citizenMovePene(Actor *someCitizen){
    double citX = someCitizen->getX();
    double citY = someCitizen->getY();
    double peneX = pene->getX();
    double peneY= pene->getY();
    //determine the distance to penelope
    double dist_p = distsquare(citX, citY, peneX, peneY);
    double dist_z = -1;
    Actor* closestObj = nullptr;
    
    //determine the distance to the closest object and the closest object's pointer by iterating through the vector
    vector<Actor*>::iterator it = actors.begin();
    while (it != actors.end()) {
        if ((*it)->isZombie() && (*it) != someCitizen){
            double distholder = distsquare((*it)->getX(), (*it)->getY(), citX, citY);
            if (dist_z == -1){
                dist_z = distholder;
                closestObj= (*it);
            }
            if (distholder < dist_z){
                dist_z = distholder;
                closestObj = (*it);
            }
        }
        it++;
    }
    
    //absolute distance in x and y coordinate repectively to penelope
    double absXCoord = abs(citX-peneX);
    double absYCoord = abs(citY-peneY);
    //if the distance to penelope is less than zombie and distance of penelope is less than 80 pixels, it'll chase penelope
    if ((dist_p < dist_z && dist_p <= 80*80) || dist_z == -1){
        //if on the same column or row (range of x and y intersects with penelope's)
        if (absXCoord <= SPRITE_WIDTH || absYCoord <= SPRITE_HEIGHT ){
            //same column
            if (absXCoord<= SPRITE_WIDTH){
                if (peneY-citY > 0){ //if penelope is at higher position
                    if (!encounterBarrier(citX, citY+2, 90, someCitizen)){
                        //citizen move up 2 pixels and if move is valid, return
                        someCitizen->setDirection(90);
                        someCitizen->moveTo(citX, citY+2);
                        return;
                    }
                }
                else if (peneY - citY < 0){
                    //penelope is lower
                    if (!encounterBarrier(citX, citY-2, 270, someCitizen)){
                        //citizen moves down 2 pixels if move is valid and return if actually moved
                        someCitizen->setDirection(270);
                        someCitizen->moveTo(citX, citY-2);
                        return;
                    }
                }
            }
            //same row
            if (absYCoord <= SPRITE_HEIGHT){
                //penelope is on the right
                    if (peneX-citX > 0){
                        if (!encounterBarrier(citX+2, citY, 0, someCitizen)){
                            //if right move is available, move right and return
                            someCitizen->setDirection(0);
                            someCitizen->moveTo(citX+2, citY);
                            return;
                        }
                    }
                //penelope on the left
                    else if (peneX - citX < 0){
                        if (!encounterBarrier(citX-2, citY, 180, someCitizen)){
                            //if left move is available, move left and return
                            someCitizen->setDirection(180);
                            someCitizen->moveTo(citX-2, citY);
                            return;
                        }

                    }
                }
            //else if not at same row or column
            }else{
                //determine the two directions that can get citizen closer to penelope
                int dir1 = -1;
                int dir2 = -1;
                if (peneX - citX > 0 && peneY - citY > 0){
                    dir1 = 0;
                    dir2 = 90;
                }
                else if (peneX - citX > 0 && peneY - citY < 0){
                    dir1 = 0;
                    dir2 = 270;
                }
                else if (peneX - citX < 0 && peneY - citY > 0){
                    dir1 = 180;
                    dir2 = 90;
                }
                else{
                    dir1 = 180;
                    dir2 = 270;
                }
                
                //randomly choose one of the direction to move
                int randDir = randInt(1, 2);
                int realOption = dir2; 
                bool moved = false;
                if (randDir == 1){
                    realOption = dir1;
                }
               //opCount counts the attempted number that citizen tries to move (if attempted success the function just returns)
                int opCount = 0;
                while (!moved && opCount < 2 && realOption!= -1) {
                    //attempt to move in the direction chosen and if actually moved, return immediately
                    switch (realOption) {
                        case 90:
                            if (!encounterBarrier(citX, citY+2, 90, someCitizen)){
                                someCitizen->setDirection(90);
                                someCitizen->moveTo(citX, citY+2);
                                moved = true;
                                return;
                            }
                            break;
                        case 270:
                            if (!encounterBarrier(citX, citY-2, 270, someCitizen)){
                                someCitizen->setDirection(270);
                                someCitizen->moveTo(citX, citY-2);
                                moved = true;
                                return;
                            }
                            break;
                        case 180:
                            if (!encounterBarrier(citX-2, citY, 180, someCitizen)){
                                someCitizen->setDirection(180);
                                someCitizen->moveTo(citX-2, citY);
                                moved = true;
                                return;
                            }
                            break;
                        case 0:
                            if (!encounterBarrier(citX+2, citY, 0, someCitizen)){
                                someCitizen->setDirection(0);
                                someCitizen->moveTo(citX+2, citY);
                                moved = true;
                                return;
                            }
                            break;
                        default:
                            break;
                    }
                    //if the direction isn't valid, change in the other direction and attempt to move
                    
                    if (realOption == dir1){
                        realOption = dir2;
                    }else{
                        realOption = dir1;
                    }
                    opCount++;
                }
            }
        }
        
        //else if the distance of citizen to zombie is less than 80 pixels
        else if (dist_z <= 80*80 && closestObj != nullptr){
            double closX = closestObj->getX();
            double closY = closestObj->getY();
            double differenceAfter = dist_z;
            int rightDir = -1;
            //check if each direction will cause the citizen to get far away from zombies and not encounter barriers and set the direction of movement if valid
            //check up
            if (!encounterBarrier(citX, citY+2, 90, someCitizen)){
                if (differenceAfter < distsquare(closX, closY, citX, citY+2)){
                    differenceAfter=distsquare(closX, closY, citX, citY+2);                                  rightDir = 90;
                }
            }
            //check down
            if (!encounterBarrier(citX, citY-2, 270, someCitizen)){
                if (differenceAfter < distsquare(closX, closY, citX, citY-2)){
                    differenceAfter=distsquare(closX, closY, citX, citY-2);
                    rightDir = 270;
                }
            }
            //check left
            if (!encounterBarrier(citX-2, citY, 180, someCitizen)){
                if (differenceAfter < distsquare(closX, closY, citX-2, citY)){
                    differenceAfter=distsquare(closX, closY, citX-2, citY);
                    rightDir = 180;
                }
            }
            //check right
            if (!encounterBarrier(citX+2, citY, 0, someCitizen)){
                if (differenceAfter < distsquare(closX, closY, citX+2, citY)){
                    differenceAfter=distsquare(closX, closY, citX+2, citY);
                    rightDir = 0;
                }
            }
            
            //use the rightDir set to determine movement (the direction of movement is already valid since it's checked in previous segament)
            if (rightDir == 0){
                someCitizen->setDirection(0);
                someCitizen->moveTo(citX+2, citY);
            }
            else if (rightDir == 180){
                someCitizen->setDirection(180);
                someCitizen->moveTo(citX-2, citY);
            }
            else if (rightDir == 90){
                someCitizen->setDirection(90);
                someCitizen->moveTo(citX, citY + 2);
            }
            else if (rightDir == 270){
                someCitizen->setDirection(270);
                someCitizen->moveTo(citX, citY-2);
            }
            return;
        }
    }

int StudentWorld::getPeople(){ return m_people; } //returns the number of people

void StudentWorld::decPeople(){ m_people--; } //decreases the number of people 




