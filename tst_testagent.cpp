#include "agent.h"
#include "location.h"
#include "player.h"
#include "qtestcase.h"
#include "game.h"
#include "QObject"
#include "stateexception.h"
#include <memory>
#include <QSignalSpy>
#include <QtTest>

using Interface::Player;
using Interface::Agent;
using Interface::Location;
using Interface::Game;

class TestAgent : public QObject
{
    Q_OBJECT

public:
    TestAgent();

private Q_SLOTS:
    void testAgentConstructor();
    void testTypeNameIsAgent();
    void testNameIsEmpty();
    void testAmountOfDraws();
    void testAddResources();
    void testIsAgentCommon();
    void testOwnerNull();
    void testTitle();
    void testIncorrectAddResources();
    void testSetPlacement();
    void testLocationNull();
    void testSetConnections();
    void testModifyConnections();
    void testSetOwner();
};

TestAgent::TestAgent()
{
}

void TestAgent::testSetOwner(){
    std::shared_ptr<Game> game = std::make_shared<Game>();
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());
    std::shared_ptr<Player>player(std::make_shared<Player>(game,1,"nam1"));

    agent->setOwner(player);


    QVERIFY2(agent->owner().lock()==player,"FAilure");
}

void TestAgent::testAgentConstructor(){
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());
    QVERIFY2(agent->typeName() == "Agent","Wrong Typename");
    QVERIFY2(agent->title()=="Basic agent","Wrong title");
    QVERIFY2(agent->isCommon()==true,"Not common");
    QVERIFY2(agent->resources() == 0,"resources not zero");
    QVERIFY_EXCEPTION_THROWN(agent->owner(),Interface::StateException);
    QVERIFY_EXCEPTION_THROWN(agent->location(),Interface::StateException);



}

void TestAgent::testTypeNameIsAgent(){

    std::shared_ptr<Agent>agent(std::make_shared<Agent>());

    QVERIFY2(agent->typeName() == "Agent", "Failure");
}

void TestAgent::testNameIsEmpty()
{
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());

    QVERIFY2(agent->name() == "", "Failure");

}
void TestAgent::testAmountOfDraws(){
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());

    QVERIFY2(agent->draws() == (1+floor((agent->resources()/5))), "Failure");
}
void TestAgent::testAddResources(){

    for (unsigned short i = 0; i<50;++i){
        std::shared_ptr<Agent>agent(std::make_shared<Agent>());
        agent->addResources(i);
        QCOMPARE(agent->resources(),i);
    }

}

void TestAgent::testIsAgentCommon(){
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());

    QVERIFY2(agent->isCommon()==true,"Failure");
}

void TestAgent::testOwnerNull(){
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());

    QVERIFY_EXCEPTION_THROWN(agent->owner(),Interface::StateException);
}

void TestAgent::testTitle(){

    std::shared_ptr<Agent>agent(std::make_shared<Agent>());
    QVERIFY2(agent->title() =="Basic agent","Failure");

}
void TestAgent::testIncorrectAddResources(){

    std::shared_ptr<Agent>agent(std::make_shared<Agent>());
    agent->addResources(20.3);
    QVERIFY2(agent->resources() != 20.3, "Failure");
}
void TestAgent::testSetPlacement(){
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());
    std::shared_ptr<Location>loc(std::make_shared<Location>(1,"name"));
    agent->setPlacement(loc);
    QCOMPARE(agent->location().lock(),loc);
}
void TestAgent::testLocationNull(){
    std::shared_ptr<Agent>agent(std::make_shared<Agent>());
    QVERIFY_EXCEPTION_THROWN(agent->location(),Interface::StateException);
}
void TestAgent::testSetConnections(){
    for (unsigned short i = 0; i<10;++i){
        std::shared_ptr<Game> game = std::make_shared<Game>();
        std::shared_ptr<Agent>agent(std::make_shared<Agent>());
        std::shared_ptr<Location>loc(std::make_shared<Location>(1,"name"));
        std::shared_ptr<Player>player(std::make_shared<Player>(game,1,"nam1"));

        agent->setOwner(player);
        agent->setPlacement(loc);
        agent->setConnections(i);
        QVERIFY2(loc->influence(player) == i,"FAilure");
    }
}

void TestAgent::testModifyConnections(){

    for (unsigned short i = 0; i<10;++i){
        std::shared_ptr<Game> game = std::make_shared<Game>();
        std::shared_ptr<Agent>agent(std::make_shared<Agent>());
        std::shared_ptr<Location>loc(std::make_shared<Location>(1,"name"));
        std::shared_ptr<Player>player(std::make_shared<Player>(game,1,"nam1"));
        agent->setOwner(player);
        agent->setPlacement(loc);
        agent->addResources(6);
        agent->modifyConnections(i);
        QVERIFY2(loc->influence(player) == (i+1),"FAilure");
    }
}

QTEST_APPLESS_MAIN(TestAgent)

#include "tst_testagent.moc"
