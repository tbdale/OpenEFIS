

#ifndef GLPFD_H
#define GLPFD_H

#include <qgl.h>
#include <qdialog.h>

void InitInstruments (void);

class GLPFD : public QGLWidget
{
    Q_OBJECT

public:

    GLPFD( QWidget* parent, const char* name, const QGLWidget* shareWidget=0 );
    GLPFD( const QGLFormat& format, QWidget* parent, const char* name, 
	   const QGLWidget* shareWidget=0 );
    ~GLPFD();

public slots:

    // Artificial Horizon
    void		setRoll( int degrees );
    void		setPitch( int degrees );
    // Airspeed Indicator
    void		setIAS( int speed );
    // Altimeter
    void		setMSL( int altitude );
    void		setBaro( float pressure );
    // Vertical Speed Indicator
    void		setVSI( int speed );
    // Dialog for testing the GUI elements
    void              testPFD();

protected:

    // OpenGL    
    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );
    // Artificial Horizon
    void 		renderFixedHorizonMarkers();
    void		renderRollMarkers();
    void		renderPitchMarkers();
    void 		renderTerrain();
    // Airspeed Indicator
    void 		renderFixedASIMarkers();
    void		renderIASMarkers();
    // Altimeter
    void 		renderFixedALTMarkers();
    void		renderMSLMarkers();
    // Vertical Speed Indicator
    void		renderFixedVSIMarkers();
    void		renderVSIValue();
    
    private:

    // OpenGL    
    GLint pixW, pixH; 		// Width & Height of window in pixels
    GLint pixW2, pixH2; 		// Half Width & Height of window in pixels
    GLfloat zfloat;			// A Z to use for layering of ortho projected markings
    QString t;
    QFont font;
    // Artificial Horizon
    GLfloat pitchInView; 		// The degrees pitch to display above and below the lubber line
    GLfloat pitch, roll;	 	// Pitch and roll in degrees 
    GLfloat pitchTranslation;	// Pitch amplified by 1/2 window pixels for use by glTranslate
    GLfloat rollRotation;		// Roll converted for glRotate
    // Airspeed Indicator
    GLfloat IASInView; 		// The indicated units to display above the center line
    GLint IASValue;	 	// Indicated Airspeed
    GLfloat IASTranslation;		// Value amplified by 1/2 window pixels for use by glTranslate
        // The following should be read from a calibration file by an init routine
        GLint Vs0, Vs1, Vfe, Vno;	// Basic Vspeeds
        GLint Vne, Va, Vy, Vx;	// More Vspeeds
        GLint IASMaxDisp;		// The highest speed to show on tape
    // Altimeter
    GLfloat MSLInView; 		// The indicated units to display above the center line
    GLint MSLValue;	 	// Altitude MSL
    GLfloat MSLTranslation;	// Value amplified by 1/2 window pixels for use by glTranslate
    GLfloat baroPressure;		// Barometric pressure in in-Hg
        // The following should be read from a calibration file by an init routine
            GLint MSLMinDisp;		// The lowest altitude to show on tape
            GLint MSLMaxDisp;		// The highest altitude to show on tape
    // VSI
    GLfloat VSIInView;		// Vertical speed to display above the centerline
    GLint VSIValue;		// Vertical speed in feet/minute
    GLfloat VSINeedleAngle;	// The angle to set the VSI needle
    
};

class TestPFD : public QDialog
{
    Q_OBJECT
public:
    TestPFD( QWidget* parent = 0, const char* name = "Test PFD",
		 bool modal = FALSE, WFlags f = 0 );
    ~TestPFD() {}
    
protected slots:

protected:

private:

};

#endif // GLPFD_H
