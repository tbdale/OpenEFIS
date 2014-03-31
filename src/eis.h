#ifndef EIS_H
#define EIS_H

#include <unistd.h>

#include <qgl.h>
#include <qwidget.h>
#include <qdialog.h>

class GLEIS : public QGLWidget
{
    Q_OBJECT
public:
    GLEIS(QWidget    *parent    = 0, 
          const char *name      = 0,
          const      QGLWidget* = 0);

    GLEIS(const QGLFormat& format, 
          QWidget*         parent, 
          const char*      name, 
          const QGLWidget* shareWidget=0 );

    //void setRPM(int rpm);
    //void setMAP(GLfloat val);
    void refresh();

public slots:
    void setRPM(int rpm);
    void setMAP(GLfloat val);
    void setMAP(int val);

    void testEIS();
    //void setOilPressure(int oilP);
    //void setOilTemp(int oilT)
    
    

protected:
    void initializeGL();
    void resizeGL(int width, int height);
    GLfloat rpmValue;
    GLfloat manifoldPressure;
    void paintGL();
    
    

private:
    static const int MAXRPM=3000;
    static const GLfloat MAXMAP=40.00f;
    static const GLint MAXOILP=80;
    static const GLint MAXOILT=400;
    void drawTach();
    void drawMap();
    void drawOilPressure();
    void drawOilTemp();
    void draw();
    void drawBigDial(  int     maxVal,
                       GLfloat angle,
                       GLfloat centerX,
                       GLfloat centerY,
                       GLfloat radius,
                       GLfloat z);
    void drawSmallDial(int     maxVal,
                       GLfloat angle,
                       GLfloat centerX,
                       GLfloat centerY,
                       GLfloat radius,
                       GLfloat z);
};
class TestEIS : public QDialog
{
    Q_OBJECT
public:
    TestEIS( QWidget*    parent = 0, 
             const char* name   = "Test EIS",
             bool        modal  = FALSE, WFlags f = 0 );
    ~TestEIS() {}
    
protected slots:

protected:

private:

};

#endif
