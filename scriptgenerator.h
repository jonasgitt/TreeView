#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H

#include <QVector>
#include <QVariant>

class runstruct{

public:
    double angles[3];
    double uAmps[3];
    QString subtitle;
    QString sampNum;
    QString sampName;

    double concs[3];
    double flow, volume;
    int knauer;

    double JTemp, JMin, JMax;

    double area, pressure;

    double euroTemps[9];

    double heightOffsT, uAmpsT;

};

runstruct parseRun(QVector<QVariant>variables);
runstruct parseContrast(QVector<QVariant>variables);
runstruct parseTransm(QVector<QVariant>variables);
runstruct parseNIMA(QVector<QVariant>variables);
runstruct parseJulabo(QVector<QVariant>variables);
runstruct parseEurotherm(QVector<QVariant>variables);

#endif // SCRIPTGENERATOR_H
