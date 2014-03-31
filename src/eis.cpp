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

void GLEIS::drawTach()
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-2.5,0.0,-10);
    GLfloat z = 0.0f;
    GLfloat angle = 0.0f;
    GLfloat defPointerAngle =0.0f;
    GLfloat radius=0.8f;
    GLfloat centerX=1.7f;
    GLfloat centerY=1.2f;
    GLfloat rpmPointerDeg=0.0f;

    /*
     *      Draw label "RPM"
     */
     
    qglColor( QColor( "white" ) );
    QFont tachLabelFont = QFont("Fixed", 14, QFont::Normal);
    qglColor( QColor( "white" ) );    
    QString label = "RPM";
    QGLWidget::renderText ( 1.2,2.25,0.1f, label, tachLabelFont, 2000 ) ;

    drawBigDial(MAXRPM,angle,centerX,centerY,radius,z);    
    
    /*
     *      Draw indicator pointer.
     */
    qglColor( QColor( "white" ) );
    QFont font = QFont("Fixed", 14, QFont::Normal);
    QFontMetrics fm = fontMetrics();
    qglColor( QColor( "white" ) );    
    QString t = (QString( "%1" ).arg(rpmValue));
    QGLWidget::renderText ( centerX+.05f,centerY-0.15f , z+0.1f, t, font, 2000 ) ;

    glLineWidth(1);
    if (rpmValue < (MAXRPM-(MAXRPM*0.05)))
        qglColor( QColor( "white" ) );
    else
        qglColor( QColor( "red" ) );

    glTranslatef( centerX, centerY, 0.0 );

    centerX=0;centerY=0;
    GLfloat point1X=0.0f; GLfloat point1Y=0.0f;
    GLfloat point2X=0.0f; GLfloat point2Y=0.0f;
    GLfloat point3X=0.0f; GLfloat point3Y=0.0f;
    GLfloat point4X=0.0f; GLfloat point4Y=0.0f;

    if (rpmValue > MAXRPM)
        rpmPointerDeg=-40;
    else
        rpmPointerDeg=115-(rpmValue/(MAXRPM/156));
    
    point1X=(centerX-0.1f)+(0.05f*sin(defPointerAngle)); point1Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point2X=(centerX)+(0.05f*sin(defPointerAngle)); point2Y=(centerY+0.1f)+(0.05f*cos(defPointerAngle));
    point3X=(centerX+0.1f)+(0.05f*sin(defPointerAngle)); point3Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point4X=(centerX)+(radius*sin(defPointerAngle)); point4Y=(centerY)+(radius*cos(defPointerAngle));

    
    glRotatef( rpmPointerDeg, 0.0, 0.0, 1.0 );
    glBegin(GL_QUADS);
        glVertex3f(point1X,point1Y,z);
        glVertex3f(point2X,point2Y,z);
        glVertex3f(point3X,point3Y,z);
        glVertex3f(point4X,point4Y,z);
    glEnd();
    glTranslatef( -centerX, -centerY, 0.0 );

}
void GLEIS::drawMap()
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-2.5,0.0,-10);
    GLfloat z = 0.0f;
    GLfloat angle = 0.0f;
    GLfloat defPointerAngle =0.0f;
    GLfloat radius=0.8f;
    GLfloat centerX=3.2f;
    GLfloat centerY=1.2f;
    GLfloat pointerDeg=0.0f;

    /*
     *      Draw label "MAP"
     */
     
    qglColor( QColor( "white" ) );
    QFont tachLabelFont = QFont("Fixed", 14, QFont::Normal);
    qglColor( QColor( "white" ) );    
    QString label = "MAP";
    QGLWidget::renderText ( 2.9,2.25,0.1f, label, tachLabelFont, 2000 ) ;

   drawBigDial(MAXMAP,angle,centerX,centerY,radius,z);    

    /*
     *      Draw indicator pointer.
     */
    qglColor( QColor( "white" ) );
    QFont font = QFont("Fixed", 14, QFont::Normal);
    QFontMetrics fm = fontMetrics();
    qglColor( QColor( "white" ) );    
    QString t = (QString( "%1" ).arg((float) manifoldPressure));
    QGLWidget::renderText ( centerX+.05f,centerY-0.15f , z+0.1f, t, font, 2000 ) ;

    glLineWidth(1);
    if (manifoldPressure < (MAXMAP-(MAXMAP*0.05)))
        qglColor( QColor( "white" ) );
    else
        qglColor( QColor( "red" ) );

    glTranslatef( centerX, centerY, 0.0 );

    centerX=0;centerY=0;
    GLfloat point1X=0.0f; GLfloat point1Y=0.0f;
    GLfloat point2X=0.0f; GLfloat point2Y=0.0f;
    GLfloat point3X=0.0f; GLfloat point3Y=0.0f;
    GLfloat point4X=0.0f; GLfloat point4Y=0.0f;

    if (manifoldPressure > MAXMAP)
        pointerDeg=-40;
    else
        pointerDeg=115-(manifoldPressure/(MAXMAP/156));
    
    point1X=(centerX-0.1f)+(0.05f*sin(defPointerAngle)); point1Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point2X=(centerX)+(0.05f*sin(defPointerAngle)); point2Y=(centerY+0.1f)+(0.05f*cos(defPointerAngle));
    point3X=(centerX+0.1f)+(0.05f*sin(defPointerAngle)); point3Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point4X=(centerX)+(radius*sin(defPointerAngle)); point4Y=(centerY)+(radius*cos(defPointerAngle));

    
    glRotatef( pointerDeg, 0.0, 0.0, 1.0 );
    glBegin(GL_QUADS);
        glVertex3f(point1X,point1Y,z);
        glVertex3f(point2X,point2Y,z);
        glVertex3f(point3X,point3Y,z);
        glVertex3f(point4X,point4Y,z);
    glEnd();
    glTranslatef( -centerX, -centerY, 0.0 );

}
void GLEIS::drawOilPressure()
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-2.5,0.0,-10);
    GLfloat z = 0.0f;
    GLfloat angle = 0.0f;
    GLfloat defPointerAngle =0.0f;
    GLfloat radius=0.5f;
    GLfloat centerX=1.5f;
    GLfloat centerY=-0.3f;
    GLfloat pointerDeg=0.0f;

    /*
     *      Draw label "Oil P."
     */
     
    qglColor( QColor( "white" ) );
    QFont tachLabelFont = QFont("Fixed", 10, QFont::Normal);
    qglColor( QColor( "white" ) );    
    QString label = "Oil P.";
    QGLWidget::renderText ( 1.2,0.5,0.1f, label, tachLabelFont, 2000 ) ;

   drawSmallDial( (GLfloat)MAXOILP , angle, centerX, centerY, radius, z);    

    /*
     *      Draw indicator pointer.
     */
    qglColor( QColor( "white" ) );
    QFont font = QFont("Fixed", 14, QFont::Normal);
    QFontMetrics fm = fontMetrics();
    qglColor( QColor( "white" ) );    
    QString t = (QString( "%1" ).arg((float) manifoldPressure));
    QGLWidget::renderText ( centerX+.05f,centerY-0.15f , z+0.1f, t, font, 2000 ) ;

    glLineWidth(1);
    if (manifoldPressure < (MAXMAP-(MAXMAP*0.05)))
        qglColor( QColor( "white" ) );
    else
        qglColor( QColor( "red" ) );

    glTranslatef( centerX, centerY, 0.0 );

    centerX=0;centerY=0;
    GLfloat point1X=0.0f; GLfloat point1Y=0.0f;
    GLfloat point2X=0.0f; GLfloat point2Y=0.0f;
    GLfloat point3X=0.0f; GLfloat point3Y=0.0f;
    GLfloat point4X=0.0f; GLfloat point4Y=0.0f;

    if (manifoldPressure > MAXMAP)
        pointerDeg=-40;
    else
        pointerDeg=115-(manifoldPressure/(MAXMAP/156));
    
    point1X=(centerX-0.1f)+(0.05f*sin(defPointerAngle)); point1Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point2X=(centerX)+(0.05f*sin(defPointerAngle)); point2Y=(centerY+0.1f)+(0.05f*cos(defPointerAngle));
    point3X=(centerX+0.1f)+(0.05f*sin(defPointerAngle)); point3Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point4X=(centerX)+(radius*sin(defPointerAngle)); point4Y=(centerY)+(radius*cos(defPointerAngle));

    
    glRotatef( pointerDeg, 0.0, 0.0, 1.0 );
    glBegin(GL_QUADS);
        glVertex3f(point1X,point1Y,z);
        glVertex3f(point2X,point2Y,z);
        glVertex3f(point3X,point3Y,z);
        glVertex3f(point4X,point4Y,z);
    glEnd();
    glTranslatef( -centerX, -centerY, 0.0 );

}
void GLEIS::drawOilTemp()
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-2.5,0.0,-10);
    GLfloat z = 0.0f;
    GLfloat angle = 0.0f;
    GLfloat defPointerAngle =0.0f;
    GLfloat radius=0.5f;
    GLfloat centerX=2.5f;
    GLfloat centerY=-0.3f;
    GLfloat pointerDeg=0.0f;

    /*
     *      Draw label "Oil T."
     */
     
    qglColor( QColor( "white" ) );
    QFont tachLabelFont = QFont("Fixed", 10, QFont::Normal);
    qglColor( QColor( "white" ) );    
    QString label = "Oil T.";
    QGLWidget::renderText ( 2.3,0.5,0.1f, label, tachLabelFont, 2000 ) ;

   drawSmallDial(MAXMAP,angle,centerX,centerY,radius,z);    

    /*
     *      Draw indicator pointer.
     */
    qglColor( QColor( "white" ) );
    QFont font = QFont("Fixed", 14, QFont::Normal);
    QFontMetrics fm = fontMetrics();
    qglColor( QColor( "white" ) );    
    QString t = (QString( "%1" ).arg((float) manifoldPressure));
    QGLWidget::renderText ( centerX+.05f,centerY-0.15f , z+0.1f, t, font, 2000 ) ;

    glLineWidth(1);
    if (manifoldPressure < (MAXMAP-(MAXMAP*0.05)))
        qglColor( QColor( "white" ) );
    else
        qglColor( QColor( "red" ) );

    glTranslatef( centerX, centerY, 0.0 );

    centerX=0;centerY=0;
    GLfloat point1X=0.0f; GLfloat point1Y=0.0f;
    GLfloat point2X=0.0f; GLfloat point2Y=0.0f;
    GLfloat point3X=0.0f; GLfloat point3Y=0.0f;
    GLfloat point4X=0.0f; GLfloat point4Y=0.0f;

    if (manifoldPressure > MAXMAP)
        pointerDeg=-40;
    else
        pointerDeg=115-(manifoldPressure/(MAXMAP/156));
    
    point1X=(centerX-0.1f)+(0.05f*sin(defPointerAngle)); point1Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point2X=(centerX)+(0.05f*sin(defPointerAngle)); point2Y=(centerY+0.1f)+(0.05f*cos(defPointerAngle));
    point3X=(centerX+0.1f)+(0.05f*sin(defPointerAngle)); point3Y=(centerY-0.1f)+(0.05f*cos(defPointerAngle));
    point4X=(centerX)+(radius*sin(defPointerAngle)); point4Y=(centerY)+(radius*cos(defPointerAngle));

    
    glRotatef( pointerDeg, 0.0, 0.0, 1.0 );
    glBegin(GL_QUADS);
        glVertex3f(point1X,point1Y,z);
        glVertex3f(point2X,point2Y,z);
        glVertex3f(point3X,point3Y,z);
        glVertex3f(point4X,point4Y,z);
    glEnd();
    glTranslatef( -centerX, -centerY, 0.0 );

}


void GLEIS::setRPM(int val)
{
    rpmValue=val;
    updateGL();

}
void GLEIS::setMAP(GLfloat val)
{
    manifoldPressure=(float) ((int) ceil(val*100)) / 100;
    //printf("Setting MAP to %f \n",manifoldPressure);
    updateGL();

}
void GLEIS::setMAP(int val)
{
    manifoldPressure=(float) val / 10;
    //printf("Setting MAP to %f \n",manifoldPressure);
    updateGL();

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
    
    QSlider* mapSlider = new QSlider ( 0, 350, 10, 300, QSlider::Horizontal, this, "mapSlider" );
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

