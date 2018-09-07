#include "gamesetup.h"


Gamesetup::Gamesetup(shared_ptr<Game> game, int players,
                     vector<QString> player_names):
    game_(game),
    players_(players),
    player_names_(player_names)

{
    //Pidetään huoli, että pariton määrä alueita
    if( players_ == 1 ){
        locations_ = 3;
    } else {
        locations_ = (players*2)-1;
    }

    setLocations();
    setPlayers();

    //Logiikkaolio luodaan tässä
    logic_ = make_shared<Logic>(game);
    runner_ = logic_->runner();
    setControls();
}

shared_ptr<Logic> Gamesetup::logic() const
{
    return logic_;
}
// valmistelee pelaajat
// asettaa pelaajat pelialueille ja asettaa pelaajille councilor ja agentti kortit
// pelaajattomille alueille nimetään councilorit myös
void Gamesetup::setPlayers(){

    try{
        //luodaan pelaajat
        for (int i = 0; i<players_; ++i){

            QString player_name = player_names_.at(i);
            shared_ptr<Player> player = game_->addPlayer(player_name);

            QString councilor_name = player_name;
            QString councilor_title ="Councilor "+player_name+" of "+ game_->locations().at(i)->name();

            shared_ptr<Councilor> councilor(make_shared<Councilor>(councilor_name,councilor_title,
                                                                   game_->locations().at(i)));
            game_->locations().at(i)->setCouncilor(councilor);

            shared_ptr<Agent> agent(make_shared<Agent>());

            player->addCard(agent);
            player->setCouncilor(councilor);
            councilor->setOwner(player);

            game_-> locations().at(i)->setInfluence(player,5);

        }

        // luo NPC alueille councilorit
        for (unsigned int i=players_; i<game_->locations().size();++i){

            QString NPC_coNAME = "coun"+QString::number((i-players_)+1);
            QString NPC_councilor_name = SettingsReader::READER.getValue(NPC_coNAME);
            QString NPC_councilor_title = "Councilor"+NPC_councilor_name+ game_->locations().at(i)->name();

            shared_ptr<Councilor> councilor(make_shared<Councilor>(NPC_councilor_name,NPC_councilor_title,
                                                                   game_->locations().at(i)));
            game_->locations().at(i)->setCouncilor(councilor);
        }

    }catch (Interface::KeyException key){
        QMessageBox mbox;
        mbox.critical(0,"ERROR",Interface::GameException(key).msg());
        exit(0);
    }
}

void Gamesetup::setControls(){
    for (int i = 0; i<players_; ++i){
        shared_ptr<Interface::Player> player = game_->players().at(i);

        shared_ptr<Interface::ManualControl> control = make_shared<Interface::ManualControl>();
        runner_->setPlayerControl(player, control);
    }
}

// luo pelilaudan alueet
void Gamesetup::setLocations(){

try{
        for (int d = 0; d<locations_;++d){

            // create and initialize a location, and add it to the game
            QString loc_key = "loc"+QString::number(d+1);
            QString loc_name = beautifyString(SettingsReader::READER.getValue(loc_key));
            shared_ptr<Location> location( make_shared<Location>(1+d, loc_name));
            location->initialize();
            game_->addLocation(location);


            // set up cards for the location deck
            int CARDS = 60;
            int AGENTS = 4;

            //influence/resurssikorttien luonti
            for (int i = 0; i<CARDS;++i){

                short int inf_amount;
                if (i <=(CARDS/2) ){
                    inf_amount = 1;
                }
                else if ((CARDS/2) < i and i < ( CARDS - (CARDS/10) )){

                    inf_amount = 3;
                }
                else{
                    inf_amount = 5;
                }
                shared_ptr<Influence> influence(make_shared<Influence>(location->name() +
                                                                   " Influence", location, inf_amount));


                location->deck()->addCard(influence);
            }

            //Agenttikorttien luonti
            for ( int i = 0 ; i<AGENTS;++i){

                shared_ptr<Interface::Agent> agent(make_shared<Interface::Agent>());
                location->deck()->addCard(agent);
            }
            for ( int i = 0 ; i<(AGENTS/2);++i ){

                shared_ptr<Interface::AgentDip> agentdip(make_shared<Interface::AgentDip>());
                location->deck()->addCard(agentdip);

                shared_ptr<Interface::AgentRS> agentrs(make_shared<Interface::AgentRS>());
                location->deck()->addCard(agentrs);
            }

            //STEVE
            shared_ptr<Interface::Steve> steve(make_shared<Interface::Steve>());
            location->deck()->addCard(steve);

            // shuffle the deck
            location->deck()->shuffle();
        }
    }catch (Interface::IoException io){
        QMessageBox mbox;
        mbox.critical(0,"ERROR",Interface::GameException(io).msg());
        exit(0);

    }catch (Interface::FormatException form){
        QMessageBox mbox;
        mbox.critical(0,"ERROR",Interface::GameException(form).msg());
        exit(0);


    }catch (Interface::KeyException key){
        QMessageBox mbox;
        mbox.critical(0,"ERROR",Interface::GameException(key).msg());
        exit(0);
    }
}


QString Gamesetup::beautifyString(QString string)
{
   return string.replace(QString("_"), QString(" "));

}

