#ifndef GAMEFUNCTIONS_HH
#define GAMEFUNCTIONS_HH

#include "settingsreader.h"
#include "location.h"
#include "game.h"
#include "cardinterface.h"
#include "influence.h"
#include "councilor.h"
#include "deckinterface.h"
#include "agent.h"
#include "player.h"
#include "random.h"
#include "settingswindow.h"
#include <iterator>
#include <memory>
#include <iostream>
#include <QObject>
#include <vector>

/**
 * @file
 * @brief sisältää pelin apufunktiot
 */
using std::make_shared;
using std::shared_ptr;
using std::weak_ptr;
using std::cout;
using std::vector;
using std::iterator;
using Interface::Agent;
using Interface::Player;
using Interface::Location;
using Interface::Game;
using Interface::CardInterface;
using Interface::Influence;
using Interface::Councilor;
using Interface::DeckInterface;
using Interface::SettingsReader;


namespace Interface{

/**
 * @brief convertToAgent muuttaa pelaajan kädessä olevat vaikutusvaltakortit
 *agenttikortiksi
 * @param player pelaaja jonka kättä tarkastellaan
 * @pre pelaajalla on kädessään tarpeeksi paljon vaikutusvaltakortteja
 * @post pelaajan kädestä on poistettu tarpeettomat vaikutusvaltakortit, ja
 * korvattu agenttikortilla.
 */
void convertToAgent(shared_ptr<Player> player);

/**
 * @brief canConvert tarkistaa onko kädessä tarpeeksi vaikutusvaltaa agenttikortiksi
 * muuttamiseen
 * @param cards korttivektori
 * @return palauttaa kädessä olevan vaikutusvallan määrän
 * @pre -
 * @post poikkeustakuu nothrow
 */
unsigned short canConvert(vector<shared_ptr<CardInterface> > cards);

/**
 * @brief gameOver
 * @param game
 */
void gameOver(shared_ptr<Game> game);
/**
 * @brief showMessageBox
 * @param text
 */
void showMessageBox(QString text);
/**
 * @brief showGameOverMessage
 * @param game
 * @param winner
 */
void showGameOverMessage(shared_ptr<Game> game,std::map<shared_ptr<Player>,vector<unsigned short>> winner);

}
#endif // GAMEFUNCTIONS_HH
