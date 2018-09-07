#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include <QKeyEvent>
#include <QDateTime>
#include <memory>
#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>

#include "trackdata.h"
#include "gamegeneration.h"
#include "obstacle.h"
#include "train.h"
#include "npctrain.h"
#include "objecthandler.h"
#include "tracknetwork.h"


class GameLogic: public QObject
{
    Q_OBJECT
public:

    explicit GameLogic(QObject *parent = nullptr);

    void newGame();

    /**
      * @pre initializeTracks kutsuttu ensin
      * @brief createObstacleVector luo jokaiselle raiteelle esteet
      */
    void createObstacleVector();

    /**
      * @pre initializeTracks kutsuttu ensin
      * @brief initializeTrain asettaa junalle tarvittavat parametrit
      */
    void initializeTrain();

    /**
      * @brief initializeTracks muuttaa raiteiden määrää
      * @emit tracksChanged() eventhandlerille
      */
    void initializeTracks();

    /**
      * @brief setConnectedStations
      *  luo junan senhetkiseen asemaan yhteydessä olevat asemat vektoriin
      */
    void setConnectedStations();

    /**
      * @brief setYmaxAndYmin tallettaa raiteiden alku ja
      *          loppukohdan Y koordinaatit
      */


    /**
      * @brief moveTrain vastaa junan liikkumisesta
      * @param Qt::Key nuolinäppäimet
      * @emit trainMoved() eventhandlerille
      */
    void moveTrain(Qt::Key key);

    /**
      * @brief deleteObstacleObjects poistaa edellisen ratavälin esteet
      *
      */
    void deleteObstacleObjects();

    /**
      * @brief cleanObstacle poistaa siivousjunaan osuvat esteet
      *
      */
    void cleanObstacle();

    /**
      * @brief newDestination asettaa junalle uuden määränpään
      * @param QString asemaa vastaava koodi
      */
    void newDestination(QString code);

    /**
      * @brief setCurrentTrainStation asettaa junan asemalle
      * @param QString asemaa vastaava koodi
      */
    void setCurrentTrainStation(QString code);

    /**
      * @brief trainY palauttaa junan Y koordinaatin
      * @return double junan Y koordinaatti
      */
    double trainY();

    /**
      * @brief getStationX palauttaa koodia vastaavan aseman x koordinaatin
      * @return double X koordinaatti
      */
    double getStationX(QString code);

    /**
      * @brief getStationY koodia vastaava X koordinaatti
      * @return double Y
      */
    double getStationY(QString code);

    /**
      * @brief trainTrack palauttaa junan senhetkisen raiteen
      * @return unsigned int raiteen numero
      */
    uint trainTrack();

    /**
      * @brief getTracks palauttaa raiteiden määrän
      * @return unsigned int raiteidne määrä
      */
    uint getTracks();

    /**
      * @brief obstacleVector palauttaa estetietorakenteen
      * @return map<int,vector<Obstacle*>> mappi :D
      */
    std::map<int, std::vector<uint> > obstacleVector();

    /**
      * @brief getAdjStations luo vektorin lähiasemien koodeista
      * @param tarvittaessa aseman koodi jonka lähiasemt halutaan
      * @return vector<QString> asemakoodit
      */
    std::vector<QString> getAdjStations(QString code);

    /**
      * @brief getStationName palauttaa asemakoodia vastaavan nimen
      * @param asemakoodi
      * @return QString aseman nimi
      */
    QString getStationName(QString code);

    /**
      * @brief getCurrentStation palauttaa junan sen hetkisen aseman
      * @return QString junan aseman koodi
      */
    QString getCurrentStation();

    /**
      * @brief getDestinationStation() palauttaa junan määränpääaseman kooodin
      * @return QString määränpääaseman koodi
      */
    QString getDestinationStation();
    /**
     * @brief luo raiteet
     */
    void createTracks();

    /**
     * @brief showScore päivittää pistenäkymän
     */
    void showScore();
    /**
     * @brief showCash päivittää rahanäkymän
     */
    void showCash();
    /**
     * @brief showHP päivittää HP näkymän
     */
    void showHP();
    /**
     * @brief createNPCTrains luo NPCjunat oikeille paikoilleen, kutsuu Gamegenerationia
     *        placeNPCTrains joka asettaa junat paikoilleen
     */
    void createNPCTrains();

    /**
     * @brief buyItem
     * @param esineen id
     */
    void buyItem(uint id);

    /**
     * @brief moveNPCTrains liikuttaa NPC junia asianmukaisiin suuntiin
     */
    void moveNPCTrains();

    /**
     * @brief scoreForTrains laskee pisteet junista jotka pääsevät esteettä
     * kohdeasemalleen
     */
    void scoreForTrains();

    /**
     * @brief isBlocked
     * @param id
     * @return true jos edessä tai alla este false muuten
     */
    bool isBlocked(uint id);

    /**
     * @brief trainIsDestroyed emit gameOver()
     */
    void trainIsDestroyed();

    /**
     * @brief storeIsEmpty
     * emit gameOver()
     */
    void storeIsEmpty();

    /**
     * @brief clearAllObjects poistaa kaikki objektit pelin päättyessä
     */
    void clearAllObjects();

    /**
     * @brief clearTracks poistaa raideobjektit
     */
    void clearTracks();

    /**
     * @brief clearNPCTrains poistaa npc junaobjektit
     */
    void clearNPCTrains();

signals:
    /**
     * @brief tracksChanged ilmoittaa että raiteet muuttuneet
     */
    void tracksChanged();

    /**
     * @brief trainMoved junaliikkunut
     */
    void trainMoved();

    /**
     * @brief trainAtStation juna saapunut asemalle
     * @param code aseman koodi
     */
    void trainAtStation(QString code);

    /**
     * @brief gameOver kertoo eventhandlerille että peli on loppu
     */
    void gameOver();


private:
    //pelaajan juna, gamegeneration, tracknetwork ja objecthandler
    std::unique_ptr<Train> playerTrain_;
    std::unique_ptr<GameGeneration> gen_;
    std::unique_ptr<TrackNetwork> tnw_;
    std::shared_ptr<ObjectHandler> obj_;

    // map pää objekteista
    std::map<QString, uint> mainObjects_;
    // kaupan esineiden idt
    std::set<uint> storeItems_;
    int playerScore_;
    uint scoreID_;
    uint cashEarned_;
    int playerCash_;
    int playerHP_;
    uint cashID_;
    // raiteiden osien idt
    std::vector<uint> trackIDs_;

    // raidekohtaisesti esteiden idt
    std::map<int, std::vector<uint>> obstacles_;
    // pelaajan asemaan yhteydessä olevat asemat
    std::vector<TrackData> connectedStations_;

    // keys: arriving, departing
    std::map<std::string,std::vector<uint>> activeNPCTrains_;

    // onko pelaaja törmäyksessä
    bool collision_ = false;

};

#endif // GAMELOGIC_H
