// Datastructures.cc

#include "datastructures.hh"

#include <random>

#include <cmath>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}


Datastructures::Datastructures()
{

}

Datastructures::~Datastructures()
{
}

unsigned int Datastructures::size()
{
    return towns_.size();
}

// poistaa pointterit ja tyhjentää rakenteet
void Datastructures::clear()
{

    auto iter=towns_.begin();
    while (iter!=towns_.end()){
        iter->second->roads_.clear();
        delete iter->second;
        towns_.erase(iter++);
    }
    towns_.clear();
}


// palauttaa nimen
std::string Datastructures::get_name(TownID id)
{
    std::unordered_map<TownID,Town*>::const_iterator iter= towns_.find(id);
    if (iter != towns_.end()){
        return towns_[id]->name_;
    }else{

        return NO_NAME;
    }

}

// palauttaa koordinaatit
Coord Datastructures::get_coordinates(TownID id)
{

    std::unordered_map<TownID,Town*>::const_iterator iter= towns_.find(id);
    if (iter != towns_.end()){
        return towns_[id]->coordinates_;
    }else{

        return {NO_VALUE, NO_VALUE};
    }

}

// palauttaa verot
int Datastructures::get_tax(TownID id)
{

    std::unordered_map<TownID,Town*>::const_iterator iter= towns_.find(id);
    if (iter != towns_.end()){
        return towns_[id]->tax_;
    }else{

        return NO_VALUE;
    }

}

// palauttaa verotettavat
std::vector<TownID> Datastructures::get_vassals(TownID id)
{

    std::unordered_map<TownID,Town*>::const_iterator iter= towns_.find(id);
    if (iter != towns_.end()){
        return towns_[id]->vassals_;
    }else{

        return {NO_ID};
    }

}

// palauttaa kaikki kaupungit
std::vector<TownID> Datastructures::all_towns()
{

    std::vector<TownID> allTowns;
    for (auto town :towns_){
        allTowns.push_back(town.first);

    }
    return allTowns;
}

// lisää kaupungin
bool Datastructures::add_town(TownID id, const std::string& name, int x, int y, int tax)
{

    auto iter= towns_.find(id);
    if (iter == towns_.end()){

        // luo kaupunkirakenne pointterin ja tallettaa siihen tarvittavat tiedot
        std::pair<int,int>co(x,y);
        Town* new_town =new Town;
        new_town->name_=name;
        new_town->tax_=tax;
        new_town->coordinates_=co;

        // lisää kaupungin unordered_mappiin jonka avaimena on kaupungin id
        towns_[id] =new_town;
        return true;


    }else{

        return false;
    }

}

// vaihtaa kaupungin nimen
bool Datastructures::change_town_name(TownID id, const std::string& newname)
{
    std::unordered_map<TownID,Town*>::const_iterator iter= towns_.find(id);
    if (iter != towns_.end()){
        towns_[id]->name_ =newname;
        return true;

    }else{

        return false;
    }
}

// poistaa kaupungin ja kaiken siihen liittyvän
bool Datastructures::remove_town(TownID id)
{

    auto iter= towns_.find(id);
    if (iter != towns_.end()){
        if (iter->second->master_ != NO_ID){
            // jos kaupungilla on master siirtretään sen verotettavat sille
            for (uint i =0;i<iter->second->vassals_.size();++i){
                towns_[iter->second->vassals_[i]]->master_= iter->second->master_;
                auto it = std::find(towns_[iter->second->master_]->vassals_.begin(),towns_[iter->second->master_]->vassals_.end(),id);
                towns_[iter->second->master_]->vassals_.erase(it);
                towns_[iter->second->master_]->vassals_.push_back(iter->second->vassals_[i]);
            }
        }else{
            // muuten verotettavilla ei ole enää verottajaa
            for (uint i =0;i<iter->second->vassals_.size();++i){
                towns_[iter->second->vassals_[i]]->master_= NO_ID;

            }
        }
        // poistaa kaikki kaupungista lähtevät ja kaupunkiin tulevat tiet
        auto road  =towns_[id]->roads_.begin();
        while (road  !=towns_[id]->roads_.end()){
            towns_[road->first]->roads_.erase(road->second.first);
            towns_[id]->roads_.erase(road++);
        }
        // poistaa Town struct pointterin
        delete iter->second;
        towns_.erase(iter);
        return true;


    }else{
        return false;
    }

}

// aakkostaa kaupungit
std::vector<TownID> Datastructures::towns_alphabetically()
{

    // kopioi mappiin, nimet avaimena id arvona
    std::map<std::string,TownID> alph;
    for (auto town : towns_){
        alph[town.second->name_]=town.first;
    }

    // idt vektoriin
    std::vector<TownID>alphvec;
    for (auto iter = alph.begin();iter!=alph.end();++iter){
        alphvec.push_back(iter->second);
    }
    return alphvec;
}

// kaupungit järjestyksessä origosta
std::vector<TownID> Datastructures::towns_distance_increasing()
{
    // samaan tyyliin kun aakkostus
    std::map<int,TownID>dist;
    for (auto t :towns_){
        dist[abs(t.second->coordinates_.first+t.second->coordinates_.second)] = t.first;
    }
    std::vector<TownID> incr;
    for (auto iter =dist.begin();iter!=dist.end();++iter){
        incr.push_back(iter->second);
    }

    return incr;
}

// etsii kysytyn nimiset kaupungit
std::vector<TownID> Datastructures::find_towns(std::string const& name)
{

    std::vector<TownID> found;
    for (auto iter=towns_.begin();iter!=towns_.end();++iter){
        if (iter->second->name_ ==name){
            found.push_back(iter->first);
        }
    }
    return found;

}

// lähimpänä origoa
TownID Datastructures::min_distance()
{

    if (towns_.size()!=0){
        auto iter = std::min_element(towns_.begin(),towns_.end(),[&](auto &t, auto &y){
            return abs(t.second->coordinates_.first+t.second->coordinates_.second)<abs(y.second->coordinates_.first+y.second->coordinates_.second);
        });
        return iter->first;
    }else{
        return NO_ID;
    }

}

// kauimpana origoa
TownID Datastructures::max_distance()
{

    if (towns_.size()!=0){
        auto iter = std::min_element(towns_.begin(),towns_.end(),[&](auto &t,auto &y){
            return abs(t.second->coordinates_.first+t.second->coordinates_.second)>abs(y.second->coordinates_.first+y.second->coordinates_.second);
        });
        return iter->first;
    }else{
        return NO_ID;
    }
}

// n:s origosta
TownID Datastructures::nth_distance(unsigned int n)
{

    if (n>towns_.size()){
        return towns_distance_increasing().at(n-1);
    }else{
        return NO_ID;
    }
}

// järjestys etäisyyden mukaan pisteestä
std::vector<TownID> Datastructures::towns_distance_increasing_from(int x, int y)
{

    std::map<int,TownID>distfrom;
    for (auto t :towns_){
        distfrom[abs(t.second->coordinates_.first-x+t.second->coordinates_.second-y)] = t.first;
    }
    std::vector<TownID> incrf;
    for (auto iter =distfrom.begin();iter!=distfrom.end();++iter){
        incrf.push_back(iter->second);
    }

    return incrf;
}

// lisää verotettava
bool Datastructures::add_vassalship(TownID vassalid, TownID masterid)
{


    auto iter = towns_.find(vassalid);
    auto iter1= towns_.find(masterid);
    if (iter !=towns_.end() and iter1 != towns_.end()){
        iter1->second->vassals_.push_back(vassalid);
        iter->second->master_=masterid;
        return true;
    }
    return false;

}

std::vector<TownID> Datastructures::taxer_path(TownID id)
{
    return {};
}

// tulostaa kaikki tiet
std::vector<std::pair<TownID, TownID>> Datastructures::all_roads()
{

    std::vector<std::pair<TownID,TownID>> allroads;

    // käy kaikki kaupungit ja tiet läpi
    for (auto it = towns_.begin();it!=towns_.end();++it){
        for (auto road =it->second->roads_.begin();road!=it->second->roads_.end();++road){

            // lisää all roads vektoriin vain sellaiset tiet joissa ensimmäinen townid on pienempi kuin toinen.
            if (road->second.first <road->second.second){
                allroads.push_back(road->second);
            }
        }
    }
    // järjestää vektorin ensisijaisesti ensimmäisen towniden mukaan ja toissijaisesti toisen mukaan.
    std::sort(allroads.begin(),allroads.end(),[=](auto a, auto b){
        if (a.first==b.first){
            return a.second<b.second;
        }else{
            return a.first<b.first;
        }
    });

    return allroads;
}

// palauttaa kaikki tiet jotka lähtevät annetusta kaupungista
std::vector<TownID> Datastructures::get_roads_from(TownID id)
{

    auto iter = towns_.find(id);
    std::vector<TownID>roadsto;

    if (iter != towns_.end()){

        for (auto road: iter->second->roads_){
            roadsto.push_back(road.second.second);
        }
        // järjestää kaupunki id:t
        std::sort(roadsto.begin(),roadsto.end());
    }
    return roadsto;
}

// lisää tien kaupunkien välille
bool Datastructures::add_road(TownID town1, TownID town2)
{

    auto iter1 = towns_.find(town1);
    auto iter2 = towns_.find(town2);
    if(iter1!=towns_.end() and iter2!= towns_.end()){
        auto road = iter1->second->roads_.find(town2);
        // lisää tien molempiin suuntiin
        // tiet ovat unordered_mapissa jossa avain on kaupungin id johon tie vie
        if (road ==iter1->second->roads_.end()){
            iter1->second->roads_[town2]=std::pair<TownID,TownID>(town1,town2);
            iter2->second->roads_[town1]=std::pair<TownID,TownID>(town2,town1);
            return true;
        }
    }
    return false;
}

// poistaa tien
bool Datastructures::remove_road(TownID town1, TownID town2)
{

    auto iter1 = towns_.find(town1);
    auto iter2 = towns_.find(town2);
    if(iter1!=towns_.end() and iter2!= towns_.end()){

        auto road1 = iter1->second->roads_.find(town2);
        auto road2 = iter2->second->roads_.find(town1);
        // jos toiselta kaupungilta löydetään tie joka vie toiseen, tiet molempiin suuntiin poistetaan.
        if (road1 !=iter1->second->roads_.end()){
            iter1->second->roads_.erase(road1);
            iter2->second->roads_.erase(road2);
            return true;
        }
    }

    return false;
}

// poistaa kaikki tiet
void Datastructures::clear_roads()
{

    for (auto it = towns_.begin();it!=towns_.end();++it){

        auto road  =it->second->roads_.begin();

        // poistaa tiet molempiin suuntiin
        while(road!=it->second->roads_.end()){
            towns_[road->first]->roads_.erase(road->second.first);
            it->second->roads_.erase(road++);
        }
    }
}

// etsii minkä tahansa reitin kaupunkien välillä
// käyttää hieman muokattua depth first hakua
std::vector<TownID> Datastructures::any_route(TownID fromid, TownID toid)
{


    if (fromid == toid){
        return {fromid,toid};
    }
    auto it1 = towns_.find(fromid);
    auto it2 = towns_.find(toid);

    if (it1 != towns_.end() and it2 != towns_.end()){
        std::vector<TownID>path;
        towns_[fromid]->color_=1;
        std::stack<TownID> s;

        // vektori johon tallennetaan kaikki kaupungit joiden tietoja muokattiin algoritmin aikana
        std::vector<TownID> v;
        int found = 0;
        s.push(fromid);
        v.push_back(fromid);
        while (s.size()!=0 and found != 1){
            TownID next =s.top();
            s.pop();
            for (auto iter=towns_[next]->roads_.begin();iter!=towns_[next]->roads_.end();++iter){
                if (towns_[iter->first]->color_ == 0){
                    towns_[iter->first]->color_ =1;
                    towns_[iter->first]->fromnode_=next;
                    s.push(iter->first);
                    v.push_back(iter->first);

                    // lopettaa heti kun määränpää löytyy
                    if (iter->first ==toid){
                        found =1;
                        break;
                    }
                }
            }
        }

        // tallentaa algoritmin muodostaman puun haaran joka johtaa kaupungista toiseen
        if (found==1){
            path.push_back(toid);
            TownID from = towns_[toid]->fromnode_;
            while (from != NO_ID){
                path.push_back(from);
                from = towns_[from]->fromnode_;
            }

            // kääntää vektorin
            std::reverse(path.begin(),path.end());

        }
        // palauttaa muokatut arvot alkuperäisiksi
        for (auto i:v){
            towns_[i]->color_=0;
            towns_[i]->fromnode_=NO_ID;
        }
        return path;


    }else{
        return {NO_ID};
    }


}

std::vector<TownID> Datastructures::longest_vassal_path(TownID id)
{
    return {};
}

int Datastructures::total_net_tax(TownID id)
{
    return NO_VALUE;
}

// vähiten kaupunkeja kaupunkien välissä
// käytää hieman muokattua breadth first hakua
std::vector<TownID> Datastructures::least_towns_route(TownID fromid, TownID toid)
{
    auto it1 = towns_.find(fromid);
    auto it2 = towns_.find(toid);
    if (it1 != towns_.end() and it2 != towns_.end()){
        std::vector<TownID>path;
        towns_[fromid]->color_=1;

        // tässä käytettiin prujuissa muistaakseni jonoa
        // mutta käytin vektoria jotta ei tarvitsisi tallettaa muokkauksia erikseen
        std::vector<TownID> q;
        int found = 0;
        q.push_back(fromid);

        uint i=0;
        // käy vektorin alkioita läpi kunnes käynyt koko vektorin
        while (i<q.size() and found != 1){
            TownID next =q[i];
            for (auto iter=towns_[next]->roads_.begin();iter!=towns_[next]->roads_.end();++iter){
                if (towns_[iter->first]->color_ == 0){
                    towns_[iter->first]->color_ =1;
                    towns_[iter->first]->fromnode_=next;
                    q.push_back(iter->first);
                    // lopettaa kun määränpää löytyy
                    if (iter->first ==toid){
                        found =1;
                        break;
                    }
                }
            }
            towns_[next]->color_=2;
            ++i;
        }
        // kokoaa reitin samalla tavalla kun any_route
        if (found==1){
            path.push_back(toid);
            TownID from = towns_[toid]->fromnode_;
            while (from != NO_ID){
                path.push_back(from);
                from = towns_[from]->fromnode_;
            }

            std::reverse(path.begin(),path.end());

        }
        // palauttaa arvot
        for (auto i : q){
            towns_[i]->color_=0;
            towns_[i]->fromnode_=NO_ID;
        }
        return path;


    }else{
        return {NO_ID};
    }
}

// etsii lyhyimmän reitin etäisyydeltään.
// käytää A* algoritmiä
std::vector<TownID> Datastructures::shortest_route(TownID fromid, TownID toid)
{
    if (fromid == toid){
        return {fromid,toid};
    }
    auto it1 = towns_.find(fromid);
    auto it2 = towns_.find(toid);
    if (it1 != towns_.end() and it2 != towns_.end()){
        std::vector<TownID>path;
        towns_[fromid]->color_=1;
        towns_[fromid]->dist_=0;
        // lambda funktio estimated distancen avulla vertailuun.
        // järjestää "käänteisesti"
        auto cmp = [=](auto a,auto b){
            return towns_[a]->diste_ > towns_[b]->diste_;};

        // alustaa prioriteetti jonon
        std::priority_queue<TownID,std::vector<TownID>,decltype(cmp)> pq(cmp);
        std::vector<TownID> v;
        int found = 0;

        pq.push(fromid);
        v.push_back(fromid);
        while (pq.size()!=0 and found != 1){
            TownID next =pq.top();
            pq.pop();
            for (auto iter=towns_[next]->roads_.begin();iter!=towns_[next]->roads_.end();++iter){
                if (towns_[iter->first]->color_ == 0){
                    towns_[iter->first]->color_ =1;

                    v.push_back(iter->first);
                    relax(next,iter->first,toid);
                    pq.push(iter->first);
                    // jälleen lopettaa kun kohde löytyy
                    if (iter->first ==toid){
                        found =1;
                        break;
                    }
                }
            }

            towns_[next]->color_=2;
        }

        if (found==1){
            path.push_back(toid);
            TownID from = towns_[toid]->fromnode_;
            while (from != NO_ID){
                path.push_back(from);
                from = towns_[from]->fromnode_;
            }

            std::reverse(path.begin(),path.end());

        }

        // palauttaa alkuperäiset arvot
        for (auto i:v){
            towns_[i]->color_=0;
            towns_[i]->fromnode_=NO_ID;
            towns_[i]->dist_ =std::numeric_limits<int>::max();
        }
        return path;


    }else{
        return {NO_ID};
    }

}

// muuttaa reittien painoja tarvittaessa ja rakentaa puun
void Datastructures::relax(TownID a, TownID b,TownID toid){



    // weight laskee kaupunkien välisen etäisyyden
    int w  =weight(a,b);
    if (towns_[b]->dist_ > towns_[a]->dist_+w){
        towns_[b]->dist_ = towns_[a]->dist_+w;

        // tässä weight laskee tutkittavan silmun etäisyyden maaliin
        towns_[b]->diste_= towns_[b]->dist_+weight(b,toid);
        towns_[b]->fromnode_ = a;
    }


}
// laskee etäisyyden kaupunkien välillä ja palauttaa sen.
int Datastructures::weight(TownID a, TownID b){

    return abs((towns_[a]->coordinates_.first-towns_[b]->coordinates_.first))
            +abs((towns_[a]->coordinates_.second-towns_[b]->coordinates_.second));
}

std::vector<TownID> Datastructures::road_cycle_route(TownID startid)
{
//    auto it1 = towns_.find(startid);

//    if (it1 != towns_.end()){
//        std::vector<TownID>path;
//        towns_[startid]->color_=0;
//        std::vector<TownID> q;
//        int found = 0;
//        q.push_back(startid);
//        uint i=0;
//        while (i<q.size() and found != 1){
//            TownID next =q[i];
//            for (auto iter=towns_[next]->roads_.begin();iter!=towns_[next]->roads_.end();++iter){
//                if (towns_[iter->first]->color_ == 0){
//                    towns_[iter->first]->color_ =1;
//                    towns_[iter->first]->fromnode_=next;
//                    q.push_back(iter->first);
//                    std::cout<<get_name(next)<<std::endl;
//                    if (iter->first ==startid){
//                        found =1;
//                        break;
//                    }
//                }
//            }

//        }
//        if (found==1){
//            path.push_back(startid);
//            TownID from = towns_[startid]->fromnode_;
//            while (from != NO_ID){
//                path.push_back(from);
//                from = towns_[from]->fromnode_;
//            }

//            std::reverse(path.begin(),path.end());

//        }
//        for (auto i : q){
//            towns_[i]->color_=0;
//            towns_[i]->fromnode_=NO_ID;
//        }
//        return path;


//    }else{
        return {NO_ID};
    //}
}

Dist Datastructures::trim_road_network()
{
    return NO_DIST;
}
