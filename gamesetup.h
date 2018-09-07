#ifndef GAMESETUP_HH
#define GAMESETUP_HH

#include "logic.h"
#include "settingsreader.h"
#include "keyexception.h"
#include "formatexception.h"
#include "ioexception.h"
#include "rangeexception.h"
#include "gameexception.h"
#include "location.h"
#include "game.h"
#include "gamefunctions.hh"
#include "influence.h"
#include "councilor.h"
#include "player.h"
#include "deckinterface.h"
#include "agent.h"
#include "agentrs.h"
#include "agentdip.h"
#include "steve.h"
#include "runner.h"
#include "manualcontrol.h"
#include <memory>
#include <iostream>
#include <QObject>
#include <vector>


/**
 * @file
 * @brief Alustaa pelialueen
 */

using std::make_shared;
using std::shared_ptr;
using std::weak_ptr;
using std::cout;
using std::vector;
using Interface::Agent;
using Interface::AgentRS;
using Interface::Player;
using Interface::Location;
using Interface::Logic;
using Interface::Game;
using Interface::Influence;
using Interface::Councilor;
using Interface::Runner;
using Interface::DeckInterface;
using Interface::SettingsReader;


class Gamesetup
{


public:
    /**
     * @brief Gamesetup rakentaja
     * @param game peliolio jota muokataan
     * @param players pelaajien määrä
     * @param player_names pelaajien nimet vektorissa
     * @pre peliolio on luotu
     * @post peliolioon on luotu pelaajat, alueet, pakat ja kortit siten, että
     * peli voidaan aloittaa ja olioita voidaan muokata. Poikkeustakuu nothrow
     */
    Gamesetup(shared_ptr<Game> game, int players, vector<QString>player_names);
    /**
     * @brief logic kertoo logiikkaolion joka luodaan peliä alustaessa. Logiikkaolio
     * käsittelee vuorot ja sisältää myös Runner -ajurin.
     * @return osoitin pelin logiikkaolioon
     */
    shared_ptr<Logic> logic() const;

private:
    shared_ptr<Game> game_;
    shared_ptr<Logic> logic_;
    vector<QString> player_names_;

    shared_ptr<Runner> runner_;
    int locations_; // amount of locations
    int players_; // amount of players

    //luo pelaajat
    void setPlayers();

    //luo alueet
    void setLocations();

    //luo kontrollioliot pelaajille ja asettaa ne ajuriin ja logiikkaan
    void setControls();

    QString beautifyString(QString string);

};


#endif // GAMESETUP_HH

