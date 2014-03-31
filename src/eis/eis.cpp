#include <qgl.h>
#include <qapplication.h>
#include <qfont.h>
#include <qstring.h>
#include <math.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qapplication.h>
#include <qdialog.h>
#include <qslider.h>
#include <qlayout.h>
#include <qaction.h>

#include "eis.h"


GLEIS::GLEIS( QWidget* parent, const char* name, const QGLWidget* shareWidget )
    : QGLWidget( parent, name, shareWidget )
{
    setFormat(QGLFormat(DoubleBuffer | DepthBuffer));
    rpmValue=0;
}
GLEIS::GLEIS(const QGLFormat& format, QWidget* parent, const char*  name, const QGLWidget* shareWidget )
{
    setFormat(QGLFormat(DoubleBuffer | DepthBuffer));
    rpmValue=0;
}
void GLEIS::initializeGL()
{
    qglClearColor(black);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}


void GLEIS::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat x = (GLfloat)width / height;
    glFrustum(-x, x, -1.0, 1.0, 4.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void GLEIS::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}

void GLEIS::draw()
{
    
    drawTach();
    drawMap();
    drawOilPressure();
    drawOilTemp();
}



void GLEIS::refresh()
{
    paintGL();
}
/**
 * Draw a large dial indicator.
 * TODO: Try to get this into a display list instead.
 * 
 * @param maxVal
 * @param angle
 * @param centerX
 * @param centerY
 * @param radius
 * @param z
 */
void GLEIS::drawBigDial(int maxVal,GLfloat angle,GLfloat centerX,GLfloat centerY,GLfloat radius,GLfloat z)
{

    /*
     *      Draw power curve (for lack of a better term)     
     */
    qglColor( QColor( "green" ) );
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    for (angle=-2.0f;angle<0.8;angle+=0.1) 
    {
        if (angle > 0.4 && angle < 0.6)
            qglColor( QColor( "yellow" ) );
        if (angle > 0.6 )
            qglColor( QColor( "red" ) );
        glVertex3f(centerX+(radius*sin(angle)),centerY+(radius*cos(angle)),z);
    }
    glEnd();

 }
void GLEIS::drawSmallDial(int maxVal,GLfloat angle,GLfloat centerX,GLfloat centerY,GLfloat radius,GLfloat z)
{

    /*
     *      Draw power curve (for lack of a better term)     
     */
    qglColor( QColor( "green" ) );
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    for (angle=-2.0f;angle<0.8;angle+=0.1) 
    {
        if (angle > 0.4 && angle < 0.6)
            qglColor( QColor( "yellow" ) );
        if (angle > 0.6 )
            qglColor( QColor( "red" ) );
        glVertex3f(centerX+(radius*sin(angle)),centerY+(radius*cos(angle)),z);
    }
    glEnd();

 }
void GLEIS::testEIS()
{
    int r;
    TestEIS testEIS( this,0,TRUE,Qt::WStyle_Customize|Qt::WStyle_DialogBorder|Qt::Qt::WStyle_StaysOnTop);
    testEIS.setCaption("EIS Input Overrides");
    testEIS.resize( 300, 150 );
    r = testEIS.exec();
}

TestEIS::TestEIS( QWidget* parent, const char* name,
			  bool modal, WFlags f )
    : QDialog( parent, name, modal, f )
{
    setCaption( "Engine Monitor -- Manual Control" );
    // The following Actions & Widgets are for the test rig only, and must be replaced by a/d connections

    QSlider* rpmSlider = new QSlider ( 0, 3600, 1, 90, QSlider::Horizontal, this, "rpmSlider" );
    
    rpmSlider->setTickmarks( QSlider::Above);
    connect( rpmSlider, SIGNAL(valueChanged(int)), parent, SLOT(setRPM(int)) );
    QLabel* rpmSliderLabel = new QLabel( "RPM Setting", this, "rpmSettingLabel" );
    
    QSlider* mapSlider = new QSlider ( 0, 400, 10, 300, QSlider::Horizontal, this, "mapSlider" );
    mapSlider->setTickmarks( QSlider::NoMarks );
    connect( mapSlider, SIGNAL(valueChanged(int)), parent, SLOT(setMAP(int)) );
    QLabel* mapSliderLabel = new QLabel( "mapSetting", this, "mapSettingLabel" );
    
    
    QGridLayout* gridLayout = new QGridLayout( this, 5, 2, 5, 5, "gridLayout");
    gridLayout->addWidget( rpmSliderLabel, 0, 0);
    gridLayout->addWidget( rpmSlider, 0, 1);
    gridLayout->addWidget( mapSliderLabel, 1, 0);
    gridLayout->addWidget( mapSlider, 1, 1);
    //gridLayout->addWidget( rpmSliderLabel, 1, 0);
    //gridLayout->addWidget( mapSliderLabel, 1, 1);
}

