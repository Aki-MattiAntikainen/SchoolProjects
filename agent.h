#ifndef AGENT_H
#define AGENT_H

#include "agentinterface.h"
#include "location.h"
#include "stateexception.h"

#include <math.h>
#include <memory>

/**
  * @file
  * @brief Kuvaa pelin yleistä agenttikorttia
  */
namespace Interface
{

class Player;

/**
  * @brief Agent kuvaa pelin agenttikorttia.
  *
  * Agenttikorttien avulla suoritetaan kaikki muut siirrot pelissä.
  * Agenttikortin ollessa alueella, voidaan alueen pakasta nostaa kortteja,
  * lisätä vaikutusvaltaa alueelle, tai lisätä resursseja itse agentille.
  *
  * Luokasta periytyy myös lisäominaisuutena toteutetut Diplomaatti- ja Resurssispesialisti- kortit.
  *
  * .
  */
class Agent: public Interface::AgentInterface
{
public:
    /**
     * @brief Agent rakentaja
     * @pre -
     * @post agentin resurssit 0
     */
    explicit Agent();

    ~Agent() = default;

    /**
     * @brief typeName palauttaa agenttikortin tyypin
     * @return "Agent"
     * @post palauttaa aina "Agent". Poikkeustakuu nothrow
     */
    QString typeName() const;

    /**
     * @brief name kertoo agenttkortin nimen
     * @return QString jossa agentin nimi
     * @pre -
     * @post poikkeustakuu nothrow
     */
    QString name() const;

    /**
     * @brief title palauttaa agenttikortin tittlein
     * @return QString jossa agentin titteli (Basic, resourse tai diplomat)
     * @pre -
     * @post poikkeustakuu nothrow
     */
    QString title() const;

    /**
     * @brief location kertoo agenttikortin
     * @return osoitin agenttikortin alueeseen
     * @pre agenttikortti on pelattu alueelle
     * @post poikkeustakuu perus
     * @exception StateException jos location_ == nullptr
     */
    std::weak_ptr<Interface::Location> location() const;

    /**
     * @brief owner palauttaa agentin omistajan osoittimen
     * @return pelaajan osoitin
     * @pre kortti nostettu pakasta ja sillä on omistaja
     * @post poikkeustakuu perus
     * @exception StateException jos omistaja nullptr
     */
    std::weak_ptr<Interface::Player> owner() const;

    /**
     * @brief setOwner asettaa kortille omistajan
     * @param owner osoitin kortin uuteen omistajaan
     * @pre omistaja ei ole nullptr
     * @post owner_ osoittaa omistajaan
     */
    void setOwner(std::weak_ptr<Interface::Player> owner);

    /**
     * @brief isCommon periytetty AgentInterface
     * @return
     */
    bool isCommon() const;

    /**
     * @brief placement periytetty AgentInterface
     * @return
     */
    std::weak_ptr<Interface::Location> placement() const;

    /**
     * @brief setPlacement periytetty AgentInterface
     * @param placement
     */
    void setPlacement(std::weak_ptr<Interface::Location> placement);

    /**
     * @brief connections periytetty AgentInterface
     * @return
     */
    unsigned short connections() const;

    /**
     * @brief setConnections periytetty AgentInterface
     * @param connections
     */
    void setConnections(unsigned short connections);

    /**
     * @brief modifyConnections periytetty AgentInterface
     * @param amount
     */
    virtual void modifyConnections(short amount);

    /**
     * @brief addResources lisää agentille resursseja jotka vaikuttavat
     * jatko-operaatiohin
     * @param amount lisättävien resurssien määrä
     * @pre -
     * @post resursseja kasvatettu määrän mukaisesti. Poikkeustakuu: nothrow
     */
    virtual void addResources(unsigned short amount);

    /**
    * @brief resources kertoo resurssien määrän agentilla
    * @return  palauttaa resurssien määrän
    * @pre-
    * @post poikkeustakuu: nothrow
    */
    virtual unsigned short resources() const;

    /**
     * @brief draws nostojen määrä joita agentti voi tehdä. Laskettu resurssien
     * määrästä.
     * @return palauttaa nostojen määrän
     * @pre-
     * @post poikkeustakuu nothrow
     */
    virtual int draws() const;


private:
    std::weak_ptr<Interface::Player> owner_;
    std::weak_ptr<Interface::Location> location_;
    unsigned short resources_;
    // influence total
};
}
#endif // AGENT_H
