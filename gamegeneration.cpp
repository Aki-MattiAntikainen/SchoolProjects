#include "gamegeneration.h"

#include <math.h>

using std::map;
using std::vector;


GameGeneration::GameGeneration(std::shared_ptr<ObjectHandler> o):
    obj_(o)
{
}

map<QString, uint> GameGeneration::generateMainObjects()
{

    map<QString, uint> objects;
    uint scoreID = obj_->createObject(Obj::StatLine);
    obj_->setProperty(scoreID, {"name", "Score"});
    obj_->setProperty(scoreID, {"value", 0});
    uint cashID = obj_->createObject(Obj::StatLine);
    obj_->setProperty(cashID, {"name", "Cash"});
    obj_->setProperty(cashID, {"value", 0});
    uint hpID = obj_->createObject(Obj::StatLine);
    obj_->setProperty(hpID, {"name", "Hitpoints"});
    obj_->setProperty(hpID, {"value", 50});
    objects["playerScore"] = scoreID;
    objects["playerCash"] = cashID;
    objects["playerHP"] = hpID;

    // Määritetään objekti, johon tallennetaan yleisesti tarvittuja arvoja
    uint id = obj_->createObject(Obj::Line);
    objects["commonProperties"] = id;
    obj_->setProperty(id, {"linecolor", "transparent"});
    int playAreaWidth = 360;
    obj_->setProperty(id, {"playAreaWidth", playAreaWidth});
    double playAreaMaxY = 362.5;
    obj_->setProperty(id, {"playAreaMaxY", playAreaMaxY});
    double playAreaMinY = 42.5;
    obj_->setProperty(id, {"playAreaMinY", playAreaMinY});
    uint trackAmount = 0;
    obj_->setProperty(id, {"numberOfTracks", trackAmount});

    // Pelaajan juna
    id = obj_->createObject(Obj::Train);
    objects["playerTrain"] = id;
    obj_->setProperty(id, {"z", 100});
    obj_->setProperty(id, {"speed", 1});
    obj_->setProperty(id,{"hp",50});

    // Lähtöasema
    id = obj_->createObject(Obj::GameStation);
    objects["currentStation"] = id;
    obj_->setProperty(id, {"x", playAreaWidth/2});
    obj_->setProperty(id, {"y", playAreaMaxY + 16});
    obj_->setProperty(id, {"code", "TPE"});
    obj_->setProperty(id, {"nameText", "Tampere asema"});

    // Kohdeasema
    id = obj_->createObject(Obj::GameStation);
    objects["destinationStation"] = id;
    obj_->setProperty(id, {"x", playAreaWidth/2});
    obj_->setProperty(id, {"y", playAreaMinY - 16});
    obj_->setProperty(id, {"code", "JVS"});
    obj_->setProperty(id, {"nameText", "Tampere Järvensivu"});

    return objects;
}

void GameGeneration::clearMainObjects(std::map<QString, uint> objects)
{
    for (auto i = objects.begin(); i != objects.end(); i++) {
        obj_->deleteObject(i->second);
    }
    objects.clear();
}

map<int, vector<uint>> GameGeneration::generateObstacles(unsigned int tracks,
                                                         double Ymax,
                                                         double Ymin,
                                                         uint commonId){

    map<int,vector<uint>> obstacles;

    std::random_device rng;
    std::mt19937 eng(rng());
    std::uniform_int_distribution<int> distr(Ymin,Ymax);

    unsigned int tracknr;
    unsigned int amount;

    // luodaan jokaiselle raiteelle esteet
    for (unsigned int i = 0; i<tracks;++i){

        // jokainen raide lisää maksimissaan 5 estettä satunnaiselle raiteelle
        std::uniform_int_distribution<int> forthistrack(0,5);
        amount = forthistrack(eng);
        std::uniform_int_distribution<int> thistrack(0,tracks-1);
        tracknr=thistrack(eng);

        // luo esteet
        for (unsigned int a = 0; a<amount;++a){
            uint id = obj_->createObject(Obj::Obstacle);
            obj_->setProperty(id, {"x", calculateCoordX(tracknr, commonId)});
            obj_->setProperty(id, {"y", distr(eng)});
            obj_->setProperty(id, {"z", 5});
            obj_->setProperty(id, {"state", QString::number(distr(eng)%4)});
            obstacles[tracknr].push_back(id);
        }
    }

    return obstacles;

}
vector<uint> GameGeneration::generateTracks(
        unsigned int tracks, double Ymax, double Ymin, uint commonId){

    // luo raiteet pala kerrallaan ja tallettaa indeksti vektoriin
    vector<uint> trackIDs_;
    for (unsigned int i = 0; i<tracks;++i){
        for (uint j = 0;j<((Ymax-Ymin)/32);j++){
            unsigned int id = obj_->createObject(Obj::Track);
            obj_->setProperty(id,{"x",calculateCoordX(i, commonId)-16});
            obj_->setProperty(id, {"y",Ymin+j*32});
            trackIDs_.push_back(id);
        }
    }
    return trackIDs_;
}


double GameGeneration::calculateCoordX(int track, uint commonId)
{
    double width =  obj_->getProperty(commonId, "playAreaWidth").toDouble();
    uint trackAmount =  obj_->getProperty(commonId, "numberOfTracks").toUInt();
    double margin = (width/2) - (trackAmount*16) + 16;
    return margin + track*32;
}


std::map<std::string,vector<uint>> GameGeneration::placeNPCTrains(std::vector<std::shared_ptr<NPCTrain> > arr, std::vector<std::shared_ptr<NPCTrain> > dep,
                                                                 uint commonId, uint currentStationID, uint destinationStationID){
    map<std::string,vector<uint>> npcids;
    uint tracks = obj_->getProperty(commonId,"numberOfTracks").toUInt();
    QString currentStation = obj_->getProperty(currentStationID, "code").toString();
    QString destination = obj_->getProperty(destinationStationID, "code").toString();

    // raiteiden y koordinaattien maksimi ja minimi
    double Ymax=obj_->getProperty(commonId,"playAreaMaxY").toDouble();
    double Ymin=obj_->getProperty(commonId,"playAreaMinY").toDouble();
    // vain pelaajan sen hetkisen asemavälin junat talletetaan npcid mappiin

    //saapuvat
    for (uint i = 0;i<arr.size();i++){
        if (arr.at(i)->destination() == currentStation && arr.at(i)->currentStation() == destination){
            unsigned int id = obj_->createObject(Obj::NPCTrainObject);
            obj_->setProperty(id,{"x",calculateCoordX(0,commonId)});
            obj_->setProperty(id,{"track",0});
            obj_->setProperty(id, {"y",(Ymax-Ymin)*arr.at(i)->getProgress()+Ymin});
            obj_->setProperty(id,{"z",10});
            obj_->setProperty(id,{"satisfaction",100});
            obj_->setProperty(id,{"state","4"});
            npcids["arriving"].push_back(id);
        }
    }
    // lähtevät
    for (uint a = 0;a<dep.size();a++){
        if (dep.at(a)->destination() == destination){
            unsigned int id = obj_->createObject(Obj::NPCTrainObject);
            obj_->setProperty(id,{"x",calculateCoordX(tracks-1,commonId)});
            obj_->setProperty(id,{"track",tracks-1});
            obj_->setProperty(id, {"y",Ymax-((Ymax-Ymin)*dep.at(a)->getProgress())});
            obj_->setProperty(id,{"z",10});
            obj_->setProperty(id,{"satisfaction",100});
            obj_->setProperty(id,{"state","4"});
            npcids["departing"].push_back(id);
        }
    }

    // palauttaa mapin saapuvista ja llähtevistä junista ja niiden id:stä. Gamelogicissa tämä on activeNPCTrains_
    return npcids;
}

void GameGeneration::resetStore(std::set<uint> &store)
{
    clearStore(store);

    for (uint x = 1; x < 5; x++) {
        uint id = obj_->createObject(Obj::StoreItem);
        obj_->setProperty(id, {"x", x-1});
        obj_->setProperty(id, {"y", 0});
        obj_->setProperty(id, {"effectType", "hp"});
        obj_->setProperty(id, {"effectValue", x*10});
        obj_->setProperty(id, {"name", "HP +"+QString::number(x*10)});
        obj_->setProperty(id, {"price", 500*x});
        store.insert(id);

        id = obj_->createObject(Obj::StoreItem);
        obj_->setProperty(id, {"x", x-1});
        obj_->setProperty(id, {"y", 1});
        obj_->setProperty(id, {"effectType", "speed"});
        obj_->setProperty(id, {"effectValue", 1});
        obj_->setProperty(id, {"name", "Speed +1"});
        obj_->setProperty(id, {"price", 1000*x});
        store.insert(id);

        id = obj_->createObject(Obj::StoreItem);
        obj_->setProperty(id, {"x", x-1});
        obj_->setProperty(id, {"y", 2});
        obj_->setProperty(id, {"effectType", "hp"});
        obj_->setProperty(id, {"effectValue", x*50});
        obj_->setProperty(id, {"name", "HP +"+QString::number(x*50)});
        obj_->setProperty(id, {"price", 1000*x+1000*(x-1)});
        store.insert(id);

        vector<QString> trainTypes = {"New train", "Better train", "Supertrain", "???"};
        id = obj_->createObject(Obj::StoreItem);
        obj_->setProperty(id, {"x", x-1});
        obj_->setProperty(id, {"y", 3});
        obj_->setProperty(id, {"effectType", "state"});
        obj_->setProperty(id, {"effectValue", 1});
        obj_->setProperty(id, {"name", trainTypes.at(x-1)});
        obj_->setProperty(id, {"price", 2500*x});
        store.insert(id);
    }
}

void GameGeneration::clearStore(std::set<uint> &store)
{
    for (auto id : store) {
        obj_->deleteObject(id);
    }
    store.clear();
}

void GameGeneration::updateStore(std::set<uint> &store, int cash)
{
    for (auto id : store) {
        if (obj_->getProperty(id, "state") != "BOUGHT") {
            if (obj_->getProperty(id, "price") > cash) {
                obj_->setProperty(id, {"state", "INACTIVE"});
            } else {
                obj_->setProperty(id, {"state", "ACTIVE"});
            }
        }
    }
}
