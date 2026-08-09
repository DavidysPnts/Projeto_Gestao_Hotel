#include <QApplication>
#include "main_window.hpp"
#include "hotel_repository.hpp"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // Repositório de produção injetado na janela
    JsonHotelRepository repository("estado_hotel_gui.json");

    MainWindow window(repository);
    window.show();

    return app.exec();
}