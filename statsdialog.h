#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include <QMap>
#include <QString>

class StatsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit StatsDialog(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<QString, int> donnees;
};

#endif // STATSDIALOG_H