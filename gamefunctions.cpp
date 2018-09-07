#include "gamefunctions.hh"


namespace Interface{

// tarkistaa voiko pelaaja muuttaa influence kortteja agentiksi
unsigned short canConvert(vector<shared_ptr<CardInterface> > cards){

    unsigned int influence_amount = 0;

    for (shared_ptr<CardInterface> card : cards){
        if ( card->typeName() == "Influence"){
            Influence *infCard = static_cast<Influence*>(card.get());
            influence_amount += infCard->amount();
        }
    }
    return influence_amount;
}

void showMessageBox(QString text){
    QMessageBox mb;
    mb.setText(text);
    mb.exec();
}

// muuttaa >=10 influenceä agentiksi
// poistaa kortteja pienimmästä suurimpaan
void convertToAgent(shared_ptr<Interface::Player> player){

    vector<unsigned short> card_amount = {1, 3, 5};

    if (canConvert(player->cards()) >= 10){
        unsigned short influence_amount = 0;
        for (unsigned short a:card_amount){
            if (influence_amount<10){
                for (shared_ptr<CardInterface>card : player->cards()){
                    if (card->typeName() == "Influence"){
                        Influence *infCard = static_cast<Influence*>(card.get());
                        if (influence_amount >= 10){
                            break;
                        }else if (infCard->amount() == a){
                            influence_amount += infCard->amount();
                            player->playCard(card);


                        }
                    }
                }
            } else {
                break;
            }
        }
        shared_ptr<Agent> agent(make_shared<Agent>());
        agent->setOwner(player);

        player->addCard(agent);
    }
}

// Luo ikkunan jossa kerrotaan pelin voittaja
// ja voittajan saamat äänet
void showGameOverMessage(shared_ptr<Game> game, std::map<shared_ptr<Player>,vector<unsigned short>> winner)
{
    std::map<shared_ptr<Player>,vector<unsigned short>>::iterator it;
    it = winner.begin();
    shared_ptr<Player> player = it->first;
    QString msg = player->name() +" won votes from: "+ "\n";

    for (unsigned short loc_id : winner[player]){
        for (shared_ptr<Location> location : game->locations()){
            if (location->id() == loc_id){
                if (location->influence(player) ==0){
                    msg += "\n"+ location->name() + QString(" with luck of draw!")+"\n";

                }else{
                    msg += "\n"+ location->name() + QString(" with ")+QString::number(location->influence(player))+
                            QString(" influence")+"\n";
                }
            }
        }
    }
    msg += "\n"+QString("and had ")+QString::number(canConvert(player->cards()))+
            QString(" influence on hand.");

    QMessageBox winmsg;
    winmsg.information(0,player->name()+" WINS!",msg);

}

// selvittää pelin voittajan

void gameOver(shared_ptr<Game> game){

    std::map<shared_ptr<Player>,vector<unsigned short>> score;
    std::map<shared_ptr<Player>,vector<unsigned short>> winner;
    vector<shared_ptr<Player>> most_areas;
    vector<shared_ptr<Player>> winners;

    // selvittää kustakin alueesta korkeimman influence määrän
    for (shared_ptr<Location> location : game->locations()){

        unsigned short player_score = 0;

        for (shared_ptr<Player> player : game->players()){
            if (location->influence(player)>player_score){
                player_score = location->influence(player);
            }

        }
        // vertaa maksipisteitä pelaajien pisteisiin
        // lisää alueen score mappiin pelaajalle tai pelaajille joiden pisteet ovat
        // alueen korkeimmat
        for (shared_ptr<Player> player : game->players()){
            if (location->influence(player)==player_score){
                score[player].push_back(location->id());
            }
        }
    }

    // etsii mapin avaimista pelaajat joilla oli alueiden korkeimpia pisteit
    // tallentaa pelaajien id:t vektoriin id
    vector<shared_ptr<Player>>id;
    std::map<shared_ptr<Player>,vector<unsigned short>>::iterator it;
    for ( it = score.begin(); it !=score.end(); ++it){
        id.push_back((it->first));
    }
    // selvittää mapista suurimman vektorin
    int votes = 0;
    for (shared_ptr<Player> player : id){
        if (score[player].size() > votes){
            votes = score[player].size();
        }

    }
    // vertaa suurinta vektoria pelaaja kohtaisiin vektoreihin
    // tallettaa pelaajat most_areas vektoriin
    for (shared_ptr<Player> player : id){
        if (score[player].size() == votes){
            most_areas.push_back(player);
        }
    }

    // jos vain yhdellä pelaajalla tulee eniten ääniä
    // on voittaja selvä ja funktio palauttaa osoittimen pelaajaan

    if (most_areas.size()==1){

        //testaamiseen
        winner[most_areas.at(0)] = score[most_areas.at(0)];
        showGameOverMessage(game,winner);

    }

    // jos useampi pelaaja on saanut "eniten" ääniä
    // voittaja valitaan kädessä olevan Influencen avulla
    // palauttaa osoittimen voittajaan
     else if(most_areas.size()>1){

        unsigned short winner_inf_sum;


        // selvittää mikä on suurin määrä influenceä mitä kellään kädessä
        for (shared_ptr<Player> player : most_areas){
            unsigned short player_inf_sum = 0;
            player_inf_sum = canConvert(player->cards());

            if (player_inf_sum > winner_inf_sum){
                winner_inf_sum = player_inf_sum;
            }
        }
        // vertaa pelaajien käsiä suurimpaan influence määrään
        for (shared_ptr<Player> player : most_areas){
            unsigned short player_inf_sum = 0;
            player_inf_sum = canConvert(player->cards());

            if (player_inf_sum == winner_inf_sum){
                winners.push_back(player);
            }
        }

        // jos pelaajilla myös saman verran influenceä kädessä
        // voittaja arvotaan
        if (winners.size() > 1){
            int winner_id = Random::RANDOM.uint(winners.size()-1);
            shared_ptr<Player> player = winners.at(winner_id);

                //testaamiseen
             winner[player] = score[player];
             showGameOverMessage(game,winner);

        // muuten voittaja on se jolla on eniten influencea.
        } else{
            winner[winners.at(0)] = score[winners.at(0)];
            showGameOverMessage(game,winner);
        }
    }
}

}//interface

