#ifndef GAMEGENERATION_H
#define GAMEGENERATION_H

#include "obstacle.h"
#include "random"
#include <iostream>
#include <memory>
#include <map>
#include <QtGlobal>
#include <set>

#include "npctrain.h"
#include "objecthandler.h"

class GameGeneration
{
public:    
    explicit GameGeneration(std::shared_ptr<ObjectHandler> o);
    /**
     * @brief generateMainObjects luo pelialueen olennaiset elementit
     * @return palauttaa mapin jonne elementtien indeksit tallennettu
     */
    std::map<QString, uint> generateMainObjects();

    /**
     * @brief clearMainObjects poistaa pelialueen pääelementit
     * @param objects on map joka sisältää indeksit elementteihin
     */
    void clearMainObjects(std::map<QString, uint> objects);

    /**
     * @brief generateObstacles luo pelialueelle esteet
     * @param tracks ratojen määrä
     * @param Ymax radan maximi y koordinaatti
     * @param Ymin minimi y koordinaatti
     * @param commonId radan elementin id numero
     * @return palauttaa mapin jossa raidekohtaisesti esteiden indeksit
     */
    std::map<int, std::vector<uint> > generateObstacles(unsigned int tracks,
                                                        double Ymax,
                                                        double Ymin,
                                                        uint commonId);
    /**
     * @brief generateTracks luo raide elementit
     * @param tracks raiteiden määrä
     * @param Ymax raiteiden max y koord
     * @param Ymin raiteiden min y koord
     * @param commonId objekti mapin commonProperties kentän avain
     * @return palauttaa vektorin jossa radan osien indeksit
     */
    std::vector<uint> generateTracks(uint tracks,
                                     double Ymax,
                                     double Ymin,
                                     uint commonId);

    /**
     * @brief placeNPCTrains luo ja asettaa NPC junat oikeille paikoilleen asemavälin tarpeen mukaisesti
     * @param arr pelaajan senhetkiselle asemalle saapuvat junat
     * @param dep asemalta lähtevät junat
     * @param commonId commonProperties
     * @param currentStationID
     * @param destinationStationID
     * @return palauttaa mapin jossa avaimena "arriving" ja "departing" ja arvona kyseisten junien indeksit
     */
    std::map<std::string,std::vector<uint>> placeNPCTrains(std::vector<std::shared_ptr<NPCTrain>> arr, std::vector<std::shared_ptr<NPCTrain>> dep,
                                     uint commonId, uint currentStationID,uint destinationStationID);
    /**
     * @brief resetStore palauttaa kaupan alkutilaan
     * @param store kaupan objektien indeksit
     */
    void resetStore(std::set<uint> &store);

    /**
     * @brief clearStore poistaa kaupan objektit
     * @param store objektien indeksit
     */
    void clearStore(std::set<uint> &store);

    /**
     * @brief updateStore päivittää kaupan esineet ostoksen jälkeen
     * @param store objektien indeksit
     * @param cash pelaajan rahat
     */
    void updateStore(std::set<uint> &store, int cash);
    /**
     * @brief calculateCoordX laskee esineille niiden x koordinaatin paikan
     * @param track radan numero
     * @param commonId commonProperties avain
     * @return palauttaa lasketun x arvon
     */
    double calculateCoordX(int track, uint commonId);

private:

    // ObjectHandler vastaa objektien hallinnasta
    std::shared_ptr<ObjectHandler> obj_;
};

#endif // GAMEGENERATION_H
