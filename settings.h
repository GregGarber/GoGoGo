#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QtCore>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
    void writeSettings();
    void readSettings();


private slots:
    void on_buttonBox_accepted();
    void blackSpeciesChanged(QString txt);
    void whiteSpeciesChanged(QString txt);

private:
    Ui::Settings *ui;
    QSettings config;
};

#endif // SETTINGS_H
