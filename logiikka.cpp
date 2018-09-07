#include "logiikka.h"
#include <QLabel>
#include <memory>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>


using namespace std;

// luo logiikka olion
Logiikka::Logiikka()

{
    std::cout << "logiikka luotu; muista alustaa muuttujat yms." << std::endl;
    // pääikkuna luo Logiikka-tyyppisen olion ohjelman suorituksen alkaessa
    // tämä on hyvä hetki luoda logiikalle tarvittavat privaatit muuttujat
    // ja esim. laittaa säikeet nukkumaan ja odottamaan pääikkunan puolelta tulevia
    // jäsenfunktiokutsuja muunnaKaistaleelle


    // luodaan ja käynnistetään säikeet
    for (int i=1; i<6;i++){
        new thread(&Logiikka::saikeet,this,i);

    }
    // luodaan apuvektori jossa 60000 paikkaa
    // kukin säie tallentaa tuotoksensa tähän vektoriin
    vector<QColor> apu(60000);
    tallennus_vec_=apu;
}

// toinen barrier joka varmistaa että säikeet samoissa kohdissa
// ennen kuin toista barrieria nollataan
void Logiikka::barrier1()
{
    // lukko seuraa barrierille saapuneita säikeitä
    unique_lock<mutex> lukko1(barrier1_mutex_);

    --mutex_luku1_;

    // kun kaikki säikeet barrierilla
    // ne vapautetaan
    if (mutex_luku1_ == 0)
    {
        ehtomuuttuja1_.notify_all();
    }

    // säikeet odottaa kunnes kaikki säikeet saapuneet barrierille
    else
    {
        cout<<6-mutex_luku1_<<" säiettä barrierilla 1"<<endl;
        ehtomuuttuja1_.wait(lukko1, [this]{ return 0 == mutex_luku1_; });
    }
}

//barrier toteutus
void Logiikka::barrier()
{
    // lukko seuraa barrierille saapuneita säikeitä
    unique_lock<mutex> lukko(barrier_mutex_);

    --mutex_luku_;

    // kun kaikki säikeet barrierilla
    // ne vapautetaan
    if (mutex_luku_ == 0)
    {
        ehtomuuttuja_.notify_all();
    }

    // säikeet odottaa kunnes kaikki säikeet saapuneet barrierille
    else
    {
        cout<<6-mutex_luku_<<" säiettä barrierilla"<<endl;
        ehtomuuttuja_.wait(lukko, [this]{ return 0 == mutex_luku_; });
    }
}

void Logiikka::muunnaKaistale(const QImage& kuva, int vasen, int oikea, std::vector<QColor>& tallennusVektori)
{
    // tallentaa kuvan olion muuttujaan
    // ensimmäisellä klikkaamis kerralla

    if (vasen_ == 0){
        kuva_ = kuva;
    }

    // seuraavilla kerroilla vain vasen ja oikea muuttuvat 
    // myös barrier1 ja i_ nollataan
    vasen_ = vasen;
    oikea_ = oikea;
    i_ = 0;
    mutex_luku1_ = 6;


    int palaMaara = kuva.height() / 100;

    std::vector<std::vector<QColor>> palaVektori(palaMaara);

    // alustetaan säikeiden hyödyntämä vektori
    vec_ = palaVektori;

    // pääsäie herättää muut säikeet, ja tekee ensimmäisen ruudun muunnoksen
    saikeet(0);

    cout<<"tallentaa"<<endl;

    // tallennusvektorin tallettaminen
    tallennusVektori = tallennus_vec_;

}

// säikeille rekursiivinen funktioketju, joka jakaa muunnettavan kaistaleen säikeille
void Logiikka::saikeet(int i) {

    // ohjelman käynnistyessä säikeet menevät heti barrierille
    // kun muunnosta kutsutaan, pääsäie alkaa suorittaa tätä samaa funktiota
    barrier();

    // pääsäikeen käytyä barrierilla vapauttamassa säikeet
    // suorittavat ne kukin omien palojen muuntamisen
    Logiikka::jaa_saikeille(i);
    // tämän jälkeen säikeet odottavat jälleen barrierilla
    // pääsäiettä joka vapauttaa säikeet
    barrier();
    //kukin säie tallentaa oman palansa tallennusvektoriin
    kerays_saikeille(i);
}

void Logiikka::jaa_saikeille(int a){

    // kukin säie muuntaa oman palan
    this->muunnaPala(kuva_, vasen_, a*100, oikea_ - vasen_, 100, vec_[a]);
    cout<<"pala"<<a<<endl;

    // välibarrier että kaikki varmasti päässeet edelliseltä barrierilta
    // ennen kun se nollataan
    barrier1();

    // ensimmäinen säie käy palauttamassa mutexluvun alkuperäiseksi
    // jotta säikeet jäisivät oikein barrierille
    // barrier nollaus
    if (i_== 0){
        mutex_luku_ = 6;
        i_ = 1;
    }
}

void Logiikka::kerays_saikeille(int i){
    // välibarrier nollaus
    if (i_== 1){
        mutex_luku1_ = 6;
        i_ = 0;
    }
    // säie tallentaa oman palansa tallennusvektoriin
    for (unsigned int j = 0; j < vec_[i].size(); j += 1) {
        tallennus_vec_[i*10000+j] = vec_[i][j];
    }

    // jälleen välibarrier estää keulimisen
    // kun säikeet tässä on keräys valmis
    barrier1();

    //barrier nollaus
    if (i_== 0){
        mutex_luku_ = 6;
        i_ = 1;
    }
    // lähettää muut paitsi pääsäikeen odottamaan uutta muunnos kutsua.
    if (i > 0){
        return saikeet(i);
    }
}

void Logiikka::muunnaPala(const QImage& kuva, int alkux, int alkuy, int leveys,
                          int korkeus, std::vector<QColor>& vekki) {

    for (int k = 0; k < korkeus; k += 1) {

        // rivin pikselit
        for (int l = 0; l < leveys; l += 1) {

            if (k == 0 || l == 0 || k == korkeus - 1 || l == leveys - 1) {
                vekki.push_back(QColor(0, 0, 0));
                continue;
            }

            // otetaan kuvasta tietyn pikselin kohdalta väri
            QColor vari = kuva.pixelColor(alkux + l, alkuy + k);


            //int mv = ( vari.red() + vari.green() + vari.blue() ) / 3;
            int mv = 0.2126*vari.red() + 0.7152*vari.green() + 0.0722*vari.blue();

            vekki.push_back(QColor(mv, mv, mv));
        }
    }
}
