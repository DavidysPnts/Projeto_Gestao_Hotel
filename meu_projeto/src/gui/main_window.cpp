#include "main_window.hpp"
#include "standard_room.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <memory>

MainWindow::MainWindow(HotelRepository& repository, QWidget* parent)
    : QWidget(parent),
      hotel_("Palace Hotel"),
      repository_(repository),
      service_(repository_) { // injeção de dependência (DIP) — igual ao HotelService dos testes
    setWindowTitle("Sistema de Gestao de Hotel - TP3");
    resize(480, 400);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Quartos:"));
    room_list_ = new QListWidget(this);
    layout->addWidget(room_list_);

    layout->addWidget(new QLabel("Hospedes:"));
    guest_list_ = new QListWidget(this);
    layout->addWidget(guest_list_);

    auto* button_row = new QHBoxLayout();
    auto* add_room_btn = new QPushButton("Adicionar Quarto", this);
    auto* add_guest_btn = new QPushButton("Adicionar Hospede", this);
    auto* save_btn = new QPushButton("Salvar", this);
    auto* load_btn = new QPushButton("Carregar", this);
    button_row->addWidget(add_room_btn);
    button_row->addWidget(add_guest_btn);
    button_row->addWidget(save_btn);
    button_row->addWidget(load_btn);
    layout->addLayout(button_row);

    // A janela apenas conecta sinais a chamadas da lógica já existente —
    // nenhuma regra de negócio é escrita aqui dentro.
    connect(add_room_btn, &QPushButton::clicked, this, &MainWindow::on_add_room_clicked);
    connect(add_guest_btn, &QPushButton::clicked, this, &MainWindow::on_add_guest_clicked);
    connect(save_btn, &QPushButton::clicked, this, &MainWindow::on_save_clicked);
    connect(load_btn, &QPushButton::clicked, this, &MainWindow::on_load_clicked);

    refresh_lists();
}

void MainWindow::refresh_lists() {
    room_list_->clear();
    for (const auto& [number, room] : hotel_.rooms()) {
        room_list_->addItem(QString("Quarto %1 - R$ %2/noite%3")
            .arg(number)
            .arg(room->get_price_per_night())
            .arg(room->is_occupied() ? " (ocupado)" : ""));
    }

    guest_list_->clear();
    for (const auto& guest : hotel_.guests()) {
        guest_list_->addItem(QString::fromStdString(guest->get_name() + " - " + guest->get_cpf()));
    }
}

void MainWindow::on_add_room_clicked() {
    bool ok = false;
    int number = QInputDialog::getInt(this, "Novo Quarto", "Numero do quarto:", 0, 1, 9999, 1, &ok);
    if (!ok) return;

    double price = QInputDialog::getDouble(this, "Novo Quarto", "Preco por noite:", 100.0, 0, 100000, 2, &ok);
    if (!ok) return;

    // Chama a lógica de domínio já existente (Hotel::add_room) — a janela
    // não decide nada sobre validade do quarto, só encaminha os dados.
    hotel_.add_room(std::make_shared<StandardRoom>(number, static_cast<float>(price)));
    refresh_lists();
}

void MainWindow::on_add_guest_clicked() {
    bool ok = false;
    QString name = QInputDialog::getText(this, "Novo Hospede", "Nome:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString cpf = QInputDialog::getText(this, "Novo Hospede", "CPF:", QLineEdit::Normal, "", &ok);
    if (!ok || cpf.isEmpty()) return;

    try {
        // Hotel::add_guest pode lançar HotelError (CPF duplicado) — a janela
        // só exibe o erro, não decide a regra de unicidade.
        hotel_.add_guest(std::make_shared<Guest>(name.toStdString(), cpf.toStdString()));
        refresh_lists();
    } catch (const HotelError& e) {
        QMessageBox::warning(this, "Erro", QString::fromStdString(e.what()));
    }
}

void MainWindow::on_save_clicked() {
    service_.save_state(hotel_); // delega para HotelService (Q4) — mesma lógica testada sem GUI
    QMessageBox::information(this, "Salvar", "Estado salvo com sucesso.");
}

void MainWindow::on_load_clicked() {
    try {
        hotel_ = service_.load_state(); // delega para HotelService (Q4)
        refresh_lists();
        QMessageBox::information(this, "Carregar", "Estado carregado com sucesso.");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erro ao carregar", QString::fromStdString(e.what()));
    }
}