

#ifndef GLHSI_H
#define GLHSI_H

#include <qgl.h>
#include <qdialog.h>

#define HSI_AIRPLANE_SYMBOL_CANARD //Compile using Cozy MKIV based HSI symbol
// #define HSI_AIRPLANE_SYMBOL_CONVENTIONAL // Compile usingspamcan HSI symbol


class GLHSI : public QGLWidget
{
    Q_OBJECT

public:

    GLHSI( QWidget* parent, const char* name, const QGLWidget* shareWidget=0 );
    GLHSI( const QGLFormat& format, QWidget* parent, const char* name, 
	   const QGLWidget* shareWidget=0 );
    ~GLHSI();

public slots:

    void		setMagHeading( int degrees );
    void		setBugHeading( int degrees );
    void		decBugHeading( );
    void		incBugHeading( );
    void		setNAVCourse( int degrees );
    void		setNAVDeviation( float nmiles );
    void		setNAVDeviation( int nmiles );
    void		setNAVCourseActive( bool active );
    void		setILSDeviation( int dev );
    void		setILSDeviation( float dev );
    void		setILSPathActive( bool active );
    void		setGPSCourse( int degrees );
    void		setGPSDeviation( float nmiles );
    void		setGPSDeviation( int nmiles );
    void		setGPSCourseActive( bool active );
    // Dialog for testing the GUI elements
    void              testHSI();
    
    
protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );
    
    void 		renderFixedMarkers();
    void		renderCompassRose();
    void		renderRoseText();

    void		renderHeadingBug( int heading, QColor color );
    void 		renderCourse(  int heading, float deviation, QColor color, int drawCourse, int drawDeviation  );
    
    private:

    GLint pixW, pixH; 				// Width & Height of window in pixels
    GLint pixW2, pixH2; 				// Half Width & Height of window in pixels
    GLfloat roseRadius;				// Radius of the compass rose
    GLfloat magHeading;
    GLint bugHeading, gpsCourse, navCourse, locCourse;
    GLfloat gpsDeviation, navDeviation, ilsDeviation;
    GLint gpsCourseActive, navCourseActive, ilsPathActive;
    QColor gpsColor, navColor, ilsColor, bugColor;
};

class TestHSI : public QDialog
{
    Q_OBJECT
public:
    TestHSI( QWidget* parent = 0, const char* name = "Test HSI",
		 bool modal = FALSE, WFlags f = 0 );
    ~TestHSI() {}
    
protected slots:

protected:

private:

};

#endif // GLHSI_H
