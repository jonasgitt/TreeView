#include "scriptgenerator.h"
#include <QDebug>

//parseModel --> parseRun --> RunStrings --> parseRun --> parseModel --> Plaintextedit
//parseModel --> runstrings(parseRun) --> ParseModel --> PlainTextEdit


runstruct parseRun(QVector<QVariant>variables){

    runstruct runvars;

    runvars.sampNum = variables[0].toInt();

    runvars.angles[0] = variables[1].toDouble();
    runvars.uAmps[0] = variables[2].toDouble();

    runvars.angles[1] = variables[3].toDouble();
    runvars.uAmps[1] = variables[4].toDouble();

    runvars.angles[2] = variables[5].toDouble();
    runvars.uAmps[2] = variables[6].toDouble();

    qDebug() << "angle 3: " << variables[5];

    return runvars; //or call scriptlines directly?
}




runstruct parseContrast(QVector<QVariant>variables){

    runstruct runvars;

    for (int i = 0; i < 3; i++){
        runvars.concs[i] = variables[i].toDouble();
    }
    runvars.flow = variables[3].toDouble();
    runvars.volume = variables[4].toDouble();

    return runvars;
}

runstruct parseTransm(QVector<QVariant>variables){

    runstruct runvars;

    runvars.subtitle = variables[0].toString();
    runvars.heightOffsT = variables[1].toDouble();

    for (int i = 2; i < 6; i++){
        runvars.angles[i-2] = variables[i].toDouble();
    }

    return runvars;
}

runstruct parseNIMA(QVector<QVariant>variables){

    runstruct runvars;

    //NEED TO DEAL WITH NIMA COMBO

    return runvars;
}

runstruct parseJulabo(QVector<QVariant>variables){

    runstruct runvars;

    runvars.JTemp = variables[0].toDouble();
    runvars.JMax = variables[1].toDouble();
    runvars.JMin = variables[2].toDouble();

    return runvars;
}

runstruct parseEurotherm(QVector<QVariant>variables){

    runstruct runvars;

    for (int i = 0; i < 8; i++){
        runvars.euroTemps[i] = variables[0].toDouble();
    }

    return runvars;
}
