#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rafraichir();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::rafraichir()
{
    Client c;
    ui->tableClients->setModel(c.afficher());
}

void MainWindow::on_btnAjouter_clicked()
{
    if (ui->leNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Le nom est obligatoire.");
        return;
    }
    Client c(ui->leId->text().toInt(), ui->leNom->text(), ui->cbType->currentText(),
             ui->leTel->text(), ui->leEmail->text(), ui->leAdresse->text());
    if (c.ajouter()) {
        QMessageBox::information(this, "Succès", "Client ajouté.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Ajout impossible (ID déjà utilisé ?).");
    }
}

void MainWindow::on_btnModifier_clicked()
{
    Client c(ui->leId->text().toInt(), ui->leNom->text(), ui->cbType->currentText(),
             ui->leTel->text(), ui->leEmail->text(), ui->leAdresse->text());
    if (c.modifier()) {
        QMessageBox::information(this, "Succès", "Client modifié.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Modification impossible.");
    }
}

void MainWindow::on_btnSupprimer_clicked()
{
    int id = ui->leId->text().toInt();
    if (QMessageBox::question(this, "Confirmation", "Supprimer ce client ?")
        != QMessageBox::Yes)
        return;
    Client c;
    if (c.supprimer(id)) {
        QMessageBox::information(this, "Succès", "Client supprimé.");
        rafraichir();
    } else {
        QMessageBox::critical(this, "Erreur", "Suppression impossible.");
    }
}

void MainWindow::on_tableClients_clicked(const QModelIndex &index)
{
    int row = index.row();
    auto m = ui->tableClients->model();
    ui->leId->setText(m->index(row, 0).data().toString());
    ui->leNom->setText(m->index(row, 1).data().toString());
    ui->cbType->setCurrentText(m->index(row, 2).data().toString());
    ui->leTel->setText(m->index(row, 3).data().toString());
    ui->leEmail->setText(m->index(row, 4).data().toString());
    ui->leAdresse->setText(m->index(row, 5).data().toString());
}

void MainWindow::on_leRecherche_textChanged(const QString &texte)
{
    Client c;
    ui->tableClients->setModel(c.rechercher(texte));
}

void MainWindow::on_btnTri_clicked()
{
    Client c;
    ui->tableClients->setModel(c.trierParNbCommandes());
}

void MainWindow::on_btnActualiser_clicked()
{
    ui->leRecherche->clear();
    rafraichir();
}