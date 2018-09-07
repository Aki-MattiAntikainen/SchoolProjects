#include "gamelogic.h"

#include <algorithm>

using std::vector;

GameLogic::GameLogic(QObject *parent):QObject(parent)

{
    //playerTrain_ = std::make_unique<Train>();
    obj_ = std::make_shared<ObjectHandler>(parent);

    gen_ = std::make_unique<GameGeneration>(obj_);
    tnw_ = std::make_unique<TrackNetwork>();
    tnw_->init();


    connect(this, SIGNAL(tracksChanged()), parent, SLOT(playAreaReady()));

    //connect(this,SIGNAL(trainMoved()), parent, SLOT(trainPositionChanged()));

    connect(this,SIGNAL(trainAtStation(QString)),parent,
            SLOT(arrivedToStation(QString)));
    connect(this,SIGNAL(gameOver()),parent, SLOT(gameEnd()));
}

void GameLogic::storeIsEmpty(){
    clearAllObjects();
    emit gameOver();
}

void GameLogic::trainIsDestroyed(){
    clearAllObjects();
    emit gameOver();
}

void GameLogic::showCash(){

    obj_->setProperty(mainObjects_["playerCash"], {"name", "Cash"});
    obj_->setProperty(mainObjects_["playerCash"], {"value", playerCash_});
}

void GameLogic::showScore(){

    obj_->setProperty(mainObjects_["playerScore"], {"name", "Score"});
    obj_->setProperty(mainObjects_["playerScore"], {"value", playerScore_});
}
void GameLogic::showHP(){
    obj_->setProperty(mainObjects_["playerTrain"],{"hp",playerHP_});
    obj_->setProperty(mainObjects_["playerHP"],{"name", "Hitpoints"});
    obj_->setProperty(mainObjects_["playerHP"],{"value", obj_->getProperty(mainObjects_["playerTrain"],"hp").toInt()});

}

void GameLogic::moveTrain(Qt::Key key){

    moveNPCTrains();

    uint trainId = mainObjects_["playerTrain"];
    double oldY = obj_->getProperty(trainId, "y").toDouble();
    uint oldTrackNr = obj_->getProperty(trainId, "track").toUInt();
    int speed = obj_->getProperty(trainId, "speed").toInt();
    double minY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMinY").toDouble();
    double maxY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMaxY").toDouble();
    uint trackAmount = obj_->getProperty(mainObjects_["commonProperties"], "numberOfTracks").toUInt();

    double newY = oldY;
    uint newTrackNr = oldTrackNr;

    if (key==Qt::Key_Up ){

        if (oldY >= minY ){

            newY = oldY - speed;
            obj_->setProperty(trainId, {"y", newY});
            obj_->setProperty(trainId, {"lastDirection", -1});

            // isBlocked varmistaa ettei pelaaja liiku esteiden läpi
            // vain npc junat estää pelaajan liikkumisen
            if (isBlocked(trainId)){
                obj_->setProperty(trainId,{"y",oldY});
                // varmistaa että pelaaja menettää elämää ja pisteitä
                // törmäyksessä vain kerran
                if (!collision_){
                    collision_ = true;
                    playerScore_-=100;
                    playerHP_-=25;
                    showHP();
                    showScore();
                }
            }else{
                collision_ =false;
            }

            if(newY < minY){
                // asemalle päädyttäessä päivitetään lähiasemat kaupat pisteet ja rahat
                obj_->setProperty(trainId, {"y", minY});
                QString newCurrent = obj_->getProperty(mainObjects_["destinationStation"], "code").toString();
                setCurrentTrainStation(newCurrent);
                setConnectedStations();
                emit trainAtStation(newCurrent);
                scoreForTrains();
                playerCash_ +=cashEarned_;
                showCash();
                gen_->updateStore(storeItems_, playerCash_);
            }
        }

    }else if (key == Qt::Key_Down ){
        if(oldY <= maxY){

            newY = oldY + speed;
            obj_->setProperty(trainId, {"y", newY});
            obj_->setProperty(trainId, {"lastDirection", 1});
            if (isBlocked(trainId)){
                obj_->setProperty(trainId,{"y",oldY});
                if (!collision_){
                    collision_ = true;
                    playerScore_-=100;
                    playerHP_-=25;
                    showHP();
                    showScore();
                }

            }else{
                collision_ =false;
            }

            if(newY > maxY){

                obj_->setProperty(trainId, {"y", maxY});
                QString current = obj_->getProperty(mainObjects_["currentStation"], "code").toString();
                setConnectedStations();
                emit trainAtStation(current);
                scoreForTrains();
                playerCash_ +=cashEarned_;
                if (cashEarned_ == 0){
                    playerHP_ -=2;
                    showHP();
                }
                showCash();
                gen_->updateStore(storeItems_, playerCash_);
            }
        }

    }else if (key==Qt::Key_Left){

        if (oldTrackNr != 0){
            newTrackNr = oldTrackNr - 1;
            obj_->setProperty(trainId, {"track", newTrackNr});
            if (!isBlocked(trainId)){
                obj_->setProperty(trainId, {"x", gen_->calculateCoordX(newTrackNr, mainObjects_["commonProperties"])});

            }else{
                obj_->setProperty(trainId, {"track", oldTrackNr});
                showScore();
            }

        }

    }else if (key==Qt::Key_Right){
        if (oldTrackNr != trackAmount-1){
            newTrackNr = oldTrackNr + 1;
            obj_->setProperty(trainId, {"track", newTrackNr});
            if (!isBlocked(trainId)){
                obj_->setProperty(trainId, {"x", gen_->calculateCoordX(newTrackNr, mainObjects_["commonProperties"])});
            }else{
                obj_->setProperty(trainId, {"track", oldTrackNr});
                showScore();
            }
        }
    }
    if (oldY != newY || oldTrackNr != newTrackNr){
        cleanObstacle();
        //emit trainMoved();
    }

    // tarkistaa onko pelaaja hengissä
    if (playerHP_ <= 0){
        trainIsDestroyed();
    }
}

void GameLogic::cleanObstacle(){

    // pelaajan radan numero
    int track = obj_->getProperty(mainObjects_["playerTrain"], "track").toInt();
    std::vector<uint> &trackObstacles = obstacles_.at(track);
    std::vector<uint> obstaclesToErase;

    if (trackObstacles.size() != 0){
        for (auto i : trackObstacles){
            int obsY = obj_->getProperty(i, "y").toInt();
            double trainY = obj_->getProperty(mainObjects_["playerTrain"], "y").toDouble();

            // eri estetyypit vaikuttaa tilastoihin eri tavallla
            if (obsY > trainY-10 && obsY < trainY+10){
                QString state = obj_->getProperty(i,"state").toString();
                if(state=="0"){
                    playerScore_ +=10;
                    cashEarned_ +=100;
                    playerHP_ -=1;

                }else if(state=="1"){
                    playerScore_+=15;
                    cashEarned_ +=150;
                    playerHP_ -=2;
                }else if(state=="2"){
                    playerScore_+=20;
                    cashEarned_ +=200;
                    playerHP_-= 3;
                }else if(state == "3"){
                    playerScore_+=40;
                    cashEarned_ +=300;
                    playerHP_-=5;
                }

                // varmistaa että kaikki esteet rajojen sisällä poistetaan
                obj_->deleteObject(i);
                obstaclesToErase.push_back(i);
            }
        }
        //poistaa esteiden idt
        for (auto a : obstaclesToErase) {
            trackObstacles.erase(find(trackObstacles.begin(), trackObstacles.end(), a));
        }
        showScore();
        showHP();
    }
}

void GameLogic::initializeTracks(){

    // raiteden määrä valitaan asemien koordinaattien perusteella
    // joten tietyllä rata välillä on aina saman verran ratoja
    auto doubleToStringC =std::to_string(
                    tnw_->getStationByCode(getCurrentStation())->lat);
    char lastDigC =doubleToStringC.back();
    int lastIntC = lastDigC;
    auto doubleToStringD =std::to_string(
                    tnw_->getStationByCode(getDestinationStation())->lat);
    char lastDigD =doubleToStringD.back();
    int lastIntD = lastDigD;

    //ratoja 2 tai 3 että menevillä ja tulevilla junilla omat raiteet
    uint trackAmount = (lastIntC+lastIntD)%3+1;
    if (trackAmount == 1){
        trackAmount+=1;
    }
    obj_->setProperty(mainObjects_["commonProperties"], {"numberOfTracks", trackAmount});

    // asettaa pelaajan junan, npct, raiteet ja esteet
    initializeTrain();
    createNPCTrains();
    createTracks();
    createObstacleVector();

    emit tracksChanged();

}

void GameLogic::initializeTrain(){
    uint trainId = mainObjects_["playerTrain"];
    double maxY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMaxY").toDouble();
    uint trackAmount = obj_->getProperty(mainObjects_["commonProperties"], "numberOfTracks").toUInt();

    //asettaa pelaajan junan oikealle paikalle
    obj_->setProperty(trainId, {"x", gen_->calculateCoordX(trackAmount/2, mainObjects_["commonProperties"])});
    obj_->setProperty(trainId, {"track", trackAmount/2});
    obj_->setProperty(trainId, {"y", maxY});
    setConnectedStations();

    // kertoo eventhandlerille millä asemalla pelaaja on
    emit trainAtStation(obj_->getProperty(mainObjects_["currentStation"], "code").toString());

}

//asettaa yhteysasemat
void GameLogic::setConnectedStations(){

    QString currentStation = obj_->getProperty(mainObjects_["currentStation"], "code").toString();
    connectedStations_ = tnw_->getStationByCode(currentStation)->adj;
}

uint GameLogic::getTracks()
{
    return obj_->getProperty(mainObjects_["commonProperties"], "numberOfTracks").toUInt();
}


void GameLogic::createObstacleVector(){

    // poistaa vanhat esteet
    deleteObstacleObjects();

    double minY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMinY").toDouble();
    double maxY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMaxY").toDouble();
    uint trackAmount = obj_->getProperty(mainObjects_["commonProperties"], "numberOfTracks").toUInt();

    // luo esteet radoille
    obstacles_ = gen_->generateObstacles(trackAmount, maxY, minY, mainObjects_["commonProperties"]);

    // luo tyhjät vektorit radoille joilla ei esteitä
    for (uint i = 0; i < trackAmount; ++i){
        obstacles_[i].size();
    }
}

void GameLogic::deleteObstacleObjects(){

    //poistaa kaikki esteet
    for (uint i = 0;i<obstacles_.size();++i){

        if(obstacles_[i].size() != 0){
            for (uint a = 0;a<obstacles_[i].size();++a){
                obj_->deleteObject(obstacles_[i].at(a));
            }
        }
    }
    obstacles_.clear();
}

std::map<int, std::vector<uint> > GameLogic::obstacleVector(){

    //esteiden indeksit raiteittain
    return obstacles_;
}

double GameLogic::trainY(){
    return obj_->getProperty(mainObjects_["playerTrain"], "y").toDouble();
}

uint GameLogic::trainTrack(){
    return obj_->getProperty(mainObjects_["playerTrain"], "track").toUInt();
}

QString GameLogic::getStationName(QString code){
    return tnw_->getStationByCode(code)->name;
}

double GameLogic::getStationX(QString code){
    return tnw_->getStationByCode(code)->lon;
}
double GameLogic::getStationY(QString code){
    return tnw_->getStationByCode(code)->lat;
}
QString GameLogic::getCurrentStation(){
    return obj_->getProperty(mainObjects_["currentStation"], "code").toString();
}
QString GameLogic::getDestinationStation(){
    return obj_->getProperty(mainObjects_["destinationStation"], "code").toString();
}

void GameLogic::setCurrentTrainStation(QString code){
    QString fullName = getStationName(code);
    obj_->setProperty(mainObjects_["currentStation"], {"code", code});
    obj_->setProperty(mainObjects_["currentStation"], {"nameText", fullName});
}

void GameLogic::newDestination(QString code){
    QString fullName = getStationName(code);
    obj_->setProperty(mainObjects_["destinationStation"], {"code", code});
    obj_->setProperty(mainObjects_["destinationStation"], {"nameText", fullName});
}

void GameLogic::newGame(){

    // tyhjentää objektit ja luo uudet painettaessa New Game
    clearAllObjects();
    mainObjects_ = gen_->generateMainObjects();
    gen_->resetStore(storeItems_);
    playerCash_=0;
    playerScore_=0;
    playerHP_ = 50;
    initializeTracks();
    showCash();
    showHP();
    showScore();

}

// yhteysasemat koodilla
std::vector<QString> GameLogic::getAdjStations(QString code){

    std::vector<QString> adjStationCodes;
    for (uint i=0;i<connectedStations_.size();++i){
        adjStationCodes.push_back(connectedStations_.at(i).station.lock()->code);
    }

    return adjStationCodes;
}

// poistaa vanhat radat ja tekee uudet
void GameLogic::createTracks(){

    // cashEarned kertoo paljonko pelaaja on ansainnut siivoamillaan esteillä
    cashEarned_ = 0;
    clearTracks();

    double minY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMinY").toDouble();
    double maxY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMaxY").toDouble();
    uint trackAmount = obj_->getProperty(mainObjects_["commonProperties"], "numberOfTracks").toUInt();
    trackIDs_ = gen_->generateTracks(trackAmount, maxY, minY, mainObjects_["commonProperties"]);
}

// selvittää asemalle päädyttäessä miten raiteilla oleville junille kävi
// ja minkä verran antaa niistä pisteitä
void GameLogic::scoreForTrains(){

    vector<uint> &arriving = activeNPCTrains_["arriving"];
    vector<uint> &departing = activeNPCTrains_["departing"];
    double minY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMinY").toDouble();
    double maxY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMaxY").toDouble();
    uint trackAmount = obj_->getProperty(mainObjects_["commonProperties"], "numberOfTracks").toUInt();

    for (auto i : arriving){
        bool trainLate = false;
        for (auto obs:obstacles_[0]){

            // jos junalla ei vapaa reitti asemalle
            if (obj_->getProperty(obs,"y").toDouble()
                    >obj_->getProperty(i,"y").toDouble()){
                playerScore_ -=100;
                playerCash_ -=100;
                trainLate=true;
            }
        }
        // jos juna jätettiin jumiin
        if (isBlocked(i)){
            playerScore_ -= 100;
            playerCash_ -= 100;
            trainLate=true;
        }
        // jos juna pääsi asemalle ongelmitta
        if (!trainLate){
            // matkustajien tyytyväisyys vaikuttaa pisteisiin ja ansioihin
            int satisfaction = obj_->getProperty(i,"satisfaction").toDouble()+(maxY-obj_->getProperty(i,"y").toDouble())*10;
            playerCash_ += satisfaction/3;
            playerScore_ += satisfaction/5;
        }
        // poistaa npc junan
        obj_->deleteObject(i);
    }
    for (auto d : departing){
        bool trainLate = false;
        for (auto obs:obstacles_[trackAmount-1]){
            if (obj_->getProperty(obs,"y").toDouble()
                    <obj_->getProperty(d,"y").toDouble()){
                playerScore_ -=100;
                playerCash_ -=100;
                trainLate=true;
            }
        }
        if (isBlocked(d)){
            playerScore_ -= 100;
            playerCash_ -= 100;
            trainLate = true;
        }
        if(!trainLate){
            int satisfaction = obj_->getProperty(d,"satisfaction").toDouble()+(obj_->getProperty(d,"y").toDouble()-minY)*10;
            playerCash_ += satisfaction/3;
            playerScore_ += satisfaction/5;
        }
        obj_->deleteObject(d);
    }

    showCash();
    showScore();

}

void GameLogic::createNPCTrains(){

    vector<std::shared_ptr<NPCTrain>> arr;
    vector<std::shared_ptr<NPCTrain>> dep;
    arr = tnw_->getStationByCode(getCurrentStation())->arr;
    dep = tnw_->getStationByCode(getCurrentStation())->dep;
    // luo radoille aktiiviset npc junat
    activeNPCTrains_ = gen_->placeNPCTrains(arr,dep,mainObjects_["commonProperties"],
            mainObjects_["currentStation"],mainObjects_["destinationStation"]);
}


void GameLogic::clearNPCTrains()
{
    // poistaa kaikki aktiiviset npc junat
    for (auto i : activeNPCTrains_) {
        for (auto j : i.second) {
            obj_->deleteObject(j);
        }
    }
    activeNPCTrains_.clear();
}


void GameLogic::moveNPCTrains(){

    vector<uint> &arriving = activeNPCTrains_["arriving"];
    vector<uint> &departing = activeNPCTrains_["departing"];
    int satisfaction;
    double minY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMinY").toDouble();
    double maxY = obj_->getProperty(mainObjects_["commonProperties"], "playAreaMaxY").toDouble();

    // liikuttaa saapuvia junia
    for (uint a = 0;a<arriving.size();a++){
        satisfaction = obj_->getProperty(arriving.at(a),"satisfaction").toInt();

        if (!isBlocked(arriving.at(a))){
            obj_->setProperty(arriving.at(a), {"y",obj_->getProperty(arriving.at(a),"y").toDouble()+0.1});
            obj_->setProperty(arriving.at(a),{"satisfaction",satisfaction+2});
            if (obj_->getProperty(arriving.at(a),"y").toDouble()>=maxY){
                // lisää pisteitä kun juna pääsee asemalle
                playerScore_ += satisfaction/5;
                playerCash_ += satisfaction/3;
                showCash();
                showScore();
                // poistaa junan
                obj_->deleteObject(arriving.at(a));
                arriving.erase(find(arriving.begin(), arriving.end(), arriving.at(a)));
            }
        }else{
            // pisteet kärsivät jos juna on jumissa liian kauan
            if (satisfaction%10==0 &&satisfaction<0){
                playerScore_ += (satisfaction/200);
                showScore();
            }
            // matkustajien pinna alkaa palamaan jos juna jää jumiin
            obj_->setProperty(arriving.at(a),{"satisfaction",satisfaction-4});
        }
    }

    //liikuttaa lähteviä junia
    for (uint d = 0; d<departing.size();d++){
        satisfaction = obj_->getProperty(departing.at(d),"satisfaction").toInt();
        obj_->setProperty(departing.at(d),{"satisfaction",satisfaction+2});
        if (!isBlocked(departing.at(d))){

            obj_->setProperty(departing.at(d), {"y",obj_->getProperty(departing.at(d),"y").toDouble()-0.1});
            if (obj_->getProperty(departing.at(d),"y").toDouble()<=minY){
                playerScore_ += satisfaction/5;
                playerCash_ += satisfaction/3;
                showCash();
                showScore();
                obj_->deleteObject(departing.at(d));
                departing.erase(find(departing.begin(), departing.end(), departing.at(d)));
            }

        }else{
            if (satisfaction%10 == 0 &&satisfaction<0){
                playerScore_ += satisfaction/200;
                showScore();
            }
            obj_->setProperty(departing.at(d),{"satisfaction",satisfaction-4});
        }
    }
}


bool GameLogic::isBlocked(uint id){

    int track = obj_->getProperty(id,"track").toInt();
    double trainY = obj_->getProperty(id, "y").toDouble();

    // npcjunien käsittely
    if (id != mainObjects_["playerTrain"]){

        std::vector<uint> &trackObstacles = obstacles_.at(track);
        // onko esteitä radalla
        if (trackObstacles.size() != 0){
            for (auto i : trackObstacles){
                int obsY = obj_->getProperty(i, "y").toInt();
                // onko jumissa esteessä
                if (obsY > trainY-10 && obsY < trainY+10){
                    return true;
                }
            }
        }
        // onko pelaaja samalla radalla
        if (track == obj_->getProperty(mainObjects_["playerTrain"],"track").toInt()){
            //estääkö pelaaja liikkumisen
            if (obj_->getProperty(mainObjects_["playerTrain"],"y").toDouble()>trainY-20
                      && obj_->getProperty(mainObjects_["playerTrain"],"y").toDouble() <trainY+20){
                return true;

            }
        }
        return false;
    }
    // pelaajan junan käsittely
    else{
        // npc junat estävät pelaajan liikkumisen
        for (auto it =activeNPCTrains_.begin(); it!= activeNPCTrains_.end();it++){
            for (uint i =0;i<it->second.size();i++){
                double npcY = obj_->getProperty(it->second.at(i),"y").toDouble();
                int npcTrack =obj_->getProperty(it->second.at(i),"track").toInt();
                if (track == npcTrack){
                    // pelaaja voi siivota junan jumittavan esteen
                    if (npcY > trainY-10 && npcY < trainY+10){
                        if (isBlocked(it->second.at(i))){
                            cleanObstacle();
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
}

void GameLogic::buyItem(uint id)
{
    obj_->setProperty(id, {"state", "BOUGHT"});

    playerCash_ -= obj_->getProperty(id, "price").toInt();
    showCash();
    gen_->updateStore(storeItems_, playerCash_);

    QString effect = obj_->getProperty(id, "effectType").toString();
    double effectValue = obj_->getProperty(id, "effectValue").toDouble();
    uint trainId = mainObjects_["playerTrain"];
    double oldValue = obj_->getProperty(trainId, effect).toDouble();
    obj_->setProperty(trainId, {effect, oldValue+effectValue});
    playerHP_ = obj_->getProperty(trainId,"hp").toInt();
    showHP();
}

void GameLogic::clearAllObjects()
{
    // poistaa kaikki objektit
    deleteObstacleObjects();
    gen_->clearMainObjects(mainObjects_);
    gen_->clearStore(storeItems_);
    clearTracks();
    clearNPCTrains();
}

void GameLogic::clearTracks()
{
    if (trackIDs_.size()!= 0){
        for (uint a = 0; a<trackIDs_.size();a++){
            obj_->deleteObject(trackIDs_.at(a));
        }
        trackIDs_.clear();
    }
}
