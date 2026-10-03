#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_tableClients_clicked(const QModelIndex &index);
    void on_leRecherche_textChanged(const QString &texte);
    void on_btnTri_clicked();
    void on_btnActualiser_clicked();
    void on_btnPdf_clicked();
    void on_btnExcel_clicked();

private:
    Ui::MainWindow *ui;
    void rafraichir();
    bool saisieValide();
};

#endif // MAINWINDOW_H