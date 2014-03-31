#ifndef EFIS_H
#define EFIS_H

#include <qwidget.h>
#include <qdialog.h>
#include <qdatetime.h>

class GLPFD;
class GLHSI;
class GLEIS;
class StampSensors;

class EFIS : public QWidget
{
    Q_OBJECT
    
public:
    
    EFIS( QWidget *parent=0, const char *name=0, WFlags f = 0);    
    int rpm;
    
public slots:

    void		frameRateTest( );
    void        UpdateInstruments ();
    
protected slots:

private:
    GLPFD* PFD;
    GLHSI* HSI;
    QTime  time;
    GLEIS*  EIS;
    StampSensors* sensors;
};

#endif // EFIS_H
