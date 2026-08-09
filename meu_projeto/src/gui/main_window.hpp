#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QWidget>
#include <QListWidget>
#include <memory>
#include "hotel.hpp"
#include "hotel_repository.hpp"
#include "hotel_service.hpp"

// Só chama métodos que já existem em Hotel/HotelService (a mesma lógica
// testada sem GUI nos TEST_CASEs de tests/test_tp3.cpp).
class MainWindow : public QWidget {
    Q_OBJECT

private:
    Hotel hotel_;              // lógica de domínio (já existente, TP1/TP2)
    HotelRepository& repository_; // abstração injetada (DIP, Q4)
    HotelService service_;        // mesma camada de serviço usada nos testes

    QListWidget* room_list_;
    QListWidget* guest_list_;

    void refresh_lists();

public:
    explicit MainWindow(HotelRepository& repository, QWidget* parent = nullptr);

private slots:
    void on_add_room_clicked();
    void on_add_guest_clicked();
    void on_save_clicked();
    void on_load_clicked();
};

#endif