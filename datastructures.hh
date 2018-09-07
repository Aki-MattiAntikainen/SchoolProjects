// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <utility>
#include <limits>
#include <unordered_map>
#include <map>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <limits>
// Type for town IDs
using TownID = std::string;

// Return value for cases where required town was not found
TownID const NO_ID = "----------";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
std::string const NO_NAME = "-- unknown --";

// Type for a coordinate (x, y)
using Coord = std::pair<int, int>;

// Return value for cases where coordinates were not found
Coord const NO_COORD{NO_VALUE, NO_VALUE};

// Type for distance
using Dist = int;

// Return value for cases where distance is unknown
Dist const NO_DIST = NO_VALUE;


// kaupunki rakenne
struct Town{

    std::string name_;
    // tiet siten että avain on kaupunki johon tie vie
    std::unordered_map<TownID,std::pair<TownID,TownID>> roads_;
    std::vector<TownID> vassals_;
    std::pair<int,int> coordinates_;
    TownID master_ = NO_ID;
    int tax_;
    TownID fromnode_ =NO_ID;
    // etäisyys lähtösilmusta
    int dist_=std::numeric_limits<int>::max();
    // arvioitu etäisyys maaliin
    int diste_=0;
    // väri 0 == valkoinen 1==harmaa 2==musta
    int color_=0;
};

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: constant
    // Short rationale for estimate: cppreference states that its complexity is constant
    unsigned int size();

    // Estimate of performance: 2T+2R jossa T on kaupungit ja R tiet
    // Short rationale for estimate: T+R clear käy kaikki kaupungit läpi kerran ja poistaa kadut, osoittimet ja pyyhkii rakenteen.
    void clear();

    // Estimate of performance:
    // Short rationale for estimate:
    std::string get_name(TownID id);

    // Estimate of performance:
    // Short rationale for estimate:
    Coord get_coordinates(TownID id);

    // Estimate of performance:
    // Short rationale for estimate:
    int get_tax(TownID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> get_vassals(TownID id);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> all_towns();

    // Estimate of performance: O(T) jossa kaupunkien lukumäärä
    // Short rationale for estimate: etsii onko kaupunki olemassa, jos ei niin lisää sen unordered mappiin
    bool add_town(TownID id, std::string const& name, int x, int y, int tax);

    // Estimate of performance:
    // Short rationale for estimate:
    bool change_town_name(TownID id, std::string const& newname);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> towns_alphabetically();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> towns_distance_increasing();

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> find_towns(std::string const& name);

    // Estimate of performance:
    // Short rationale for estimate:
    TownID min_distance();

    // Estimate of performance:
    // Short rationale for estimate:
    TownID max_distance();

    // Estimate of performance:
    // Short rationale for estimate:
    TownID nth_distance(unsigned int n);

    // Estimate of performance:
    // Short rationale for estimate:
    bool add_vassalship(TownID vassalid, TownID masterid);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> taxer_path(TownID id);

    // Estimate of performance: R+ (R*log(R) jossa R on teiden lukumäärä
    // Short rationale for estimate: käy ensin kaikki tiet läpi joista valitsee ne joissa ensimmäinen alkio on pienempi kuin toinen
    // sitten järjestelee vektorin std::sortilla jonka kompleksisuus on n*log(n)
    std::vector<std::pair<TownID, TownID>> all_roads();

    // Estimate of performance: kt + kt*log(kt) jossa kt on kaupungista lähtevien teiden määrä
    // Short rationale for estimate: unordered_map find on keskimäärin vakio, käydään tiet läpi ja lisätään ne vektoriin, järjestellään vektori std::sortilla
    std::vector<TownID> get_roads_from(TownID id);

    // Estimate of performance: kt jossa kt on kaupungista lähtevien teiden määrä
    // Short rationale for estimate: tiet on unorderd mapissa jossa alkion haku jota ei löydy kesto on lineaarinen alkioiden määrään nähden.
    bool add_road(TownID town1, TownID town2);

    // Estimate of performance:vakio,T jos kaupunkia ei löydy
    // Short rationale for estimate:sekä tiet että kaupungit ovat unordered mapissa joissa haku aika on vakio, tai n jos alkio ei ole olemassa.
    bool remove_road(TownID town1, TownID town2);

    // Estimate of performance:T+R
    // Short rationale for estimate:käy kaikki kaupungit läpi, poistaa kaikki tiet.
    void clear_roads();

    // Estimate of performance:O(V+E)
    // Short rationale for estimate: syvyys ensin algoritmi, lopettaa kun reitti löytyy.
    std::vector<TownID> any_route(TownID fromid, TownID toid);

    // Non-compulsory operations

    // Estimate of performance:vassals+roads
    // Short rationale for estimate: käy läpi vassallit ja kadut ja poistaa ne. haku on vakioaikainen.
    bool remove_town(TownID id);

    // Estimate of performance:T+log(t) jossa t on townssien määrä
    // Short rationale for estimate: käy läpij kaupunkien kokoisen rakenteen kaksi kertaa, lisää mappiin ja lisää vektoriin
    std::vector<TownID> towns_distance_increasing_from(int x, int y);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> longest_vassal_path(TownID id);

    // Estimate of performance:
    // Short rationale for estimate:
    int total_net_tax(TownID id);

    // Estimate of performance:T+R+Reitinpituus+T pahimmillaan
    // Short rationale for estimate: Leveys ensin algoritmi, lopettaa kun kohde löytyy, tallentaa reitin vektoriin,
    // ja palauttaa läpikäytyjen kaupunkien arvot alkuperäisiin
    std::vector<TownID> least_towns_route(TownID fromid, TownID toid);

    // Estimate of performance: O((T+R)lg(v)+reitinpituus+käydytkaupungit)
    // Short rationale for estimate: A* algoritmi, plus luo reitistä vektorin ja palauttaa tiedot alkuperäisiksi.
    std::vector<TownID> shortest_route(TownID fromid, TownID toid);

    // Estimate of performance:
    // Short rationale for estimate:
    std::vector<TownID> road_cycle_route(TownID startid);

    // Estimate of performance:
    // Short rationale for estimate:
    Dist trim_road_network();

private:
    // Add stuff needed for your class implementation here
    std::unordered_map<TownID,Town*> towns_;

    //A* relax
    void relax(TownID a, TownID b,TownID toid);

    // kaupunkien välinen etäisyys
    int weight(TownID a, TownID b);

};

#endif // DATASTRUCTURES_HH
