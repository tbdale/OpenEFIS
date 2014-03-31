#include "eis.h"

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
