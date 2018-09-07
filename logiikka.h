#ifndef LOGIIKKA_H
#define LOGIIKKA_H
#include <QImage>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>


class Logiikka
{
public:
    Logiikka();
    void barrier1();
    void barrier();

    void muunnaKaistale(const QImage& kuva, int vasen, int oikea,
                        std::vector<QColor>& vekki);
    void muunnaPala(const QImage &kuva, int alkux, int alkuy,
                    int leveys, int korkeus, std::vector<QColor>& vekki);

    void saikeet(int i);
    void jaa_saikeille(int i);
    void kerays_saikeille(int i);

private:

    QImage kuva_;

    int vasen_= 0;
    int oikea_;
    int i_ = 0;

    std::vector<std::vector<QColor>> vec_;
    std::vector<QColor> tallennus_vec_;

    std::mutex barrier_mutex_;
    std::mutex barrier1_mutex_;

    std::condition_variable ehtomuuttuja_;
    std::condition_variable ehtomuuttuja1_;

    unsigned int mutex_luku_ = 6;
    unsigned int mutex_luku1_ = 6;

};

#endif // LOGIIKKA_H
