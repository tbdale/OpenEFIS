/*   
    hsi.cpp - A Horizontal Situation Indicator OpenGL Avionics Widget
    Copyright (C)  2003 Tom Dollmeyer (tom@dollmeyer.com)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
*/

/*
 ToDo List
    - much ...
*/

#include <math.h>

#include "hsi.h"
#include <qstring.h>
#include <qfont.h>
#include <qgl.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qaction.h>

#if defined(Q_CC_MSVC)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

/*!
  Create a GLHSI widget
*/

GLHSI::GLHSI( QWidget* parent, const char* name, const QGLWidget* shareWidget )
    : QGLWidget( parent, name, shareWidget )
{
    roseRadius = 0.78;	// In frame units
    gpsColor = QColor( "cyan" ); 
    navColor = QColor( "green" );
    ilsColor = QColor( "green" );
    bugColor = QColor( "orange" );
     
    magHeading = 0;
    bugHeading = 0;
    gpsCourseActive = FALSE;
    navCourseActive = FALSE;
    ilsPathActive = FALSE;
    gpsCourse = 0;
    gpsDeviation = 0.0;
    navCourse = 0;
    navDeviation = 0.0;
    ilsDeviation = 0.0;
    
    // This section is a test rig that must be removed
    gpsCourse = 30;
    gpsDeviation = 1.0;
    gpsCourseActive = FALSE;
    navCourse = 330;
    navDeviation = -3.0;
    navCourseActive = FALSE;
    ilsDeviation = 1.5;
    ilsPathActive = FALSE;
    // end test rig
}


/*!
  Create a GLHSI widget
*/

GLHSI::GLHSI( const QGLFormat& format, QWidget* parent, const char* name, 
	      const QGLWidget* shareWidget )
    : QGLWidget( format, parent, name, shareWidget )
{
    roseRadius = 0.78;	// In frame units
    gpsColor = QColor( "cyan" ); 
    navColor = QColor( "green" );
    ilsColor = QColor( "green" );
    bugColor = QColor( "orange" );
     
    magHeading = 0;
    bugHeading = 0;
    gpsCourseActive = FALSE;
    navCourseActive = FALSE;
    ilsPathActive = FALSE;
    gpsCourse = 0;
    gpsDeviation = 0.0;
    navCourse = 0;
    navDeviation = 0.0;
    ilsDeviation = 0.0;
    
    // This section is a test rig that must be removed
    gpsCourse = 30;
    gpsDeviation = 1.0;
    gpsCourseActive = TRUE;
    navCourse = 330;
    navDeviation = -3.0;
    navCourseActive = FALSE;
    ilsDeviation = 1.5;
    ilsPathActive = FALSE;
    // end test rig
}


/*!
  Release allocated resources
*/

GLHSI::~GLHSI()
{
    makeCurrent();
}


/*!
  Set up the OpenGL rendering state, and define display list
*/

void GLHSI::initializeGL()
{
    qglClearColor( QColor( eraseColor()) );		// Let OpenGL clear to background color
    glEnable( GL_DEPTH_TEST );
}

/*!
  Set up the OpenGL view port, matrix mode, etc.
*/

void GLHSI::resizeGL( int w, int h )
{
    // Capture the window scaling for use by the rendering functions
    pixW = w;
    pixH = h;
    pixW2 = w/2;
    pixH2 = h/2;
    
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(-(float) w / (float) h, (float) w / (float) h, -1.0, 1.0, 0.0, 10.0);
}


void GLHSI::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, -5.0 );
    glScalef( 1.0, 1.0, 1.0 ); 
    
    renderFixedMarkers();				// Draw the fixed markings at identity translation
//    renderData();
//    renderWindArrow();
    renderHeadingBug( bugHeading, bugColor );
    
    if( gpsCourseActive ) renderCourse( gpsCourse, gpsDeviation, gpsColor, TRUE, TRUE );
    if( navCourseActive ) renderCourse( navCourse, navDeviation, navColor, TRUE, TRUE );
    if( ilsPathActive ) renderCourse( (( navCourse - 90 ) % 360), ilsDeviation, ilsColor, FALSE, TRUE );
    renderCompassRose();				// Draw the rose

}

void GLHSI::renderFixedMarkers()
{	
    GLint i, textBottomPix, textLeftPix;
    GLfloat z, sinI, cosI;
    QString t;

    glLineWidth( 2 );
    z = 3.0;
    qglColor( QColor( "white" ) );
    
    // The little airplane
    glBegin(GL_QUADS);
#ifdef HSI_AIRPLANE_SYMBOL_CONVENTIONAL
        // Fuse
        glVertex3f(  0.01,  0.05, z);
        glVertex3f( -0.01,  0.05, z);
        glVertex3f( -0.02,  0.00, z);
        glVertex3f(  0.02,  0.00, z);
        glVertex3f(  0.02,  0.00, z);
        glVertex3f( -0.02,  0.00, z);
        glVertex3f( -0.01, -0.12, z);
        glVertex3f(  0.01, -0.12, z);
        //Left wing
        glVertex3f(  0.00,  0.00, z);
        glVertex3f( -0.12,  0.00, z);
        glVertex3f( -0.12, -0.03, z);
        glVertex3f(  0.00, -0.05, z);
        //Right wing
        glVertex3f(  0.00,  0.00, z);
        glVertex3f(  0.00, -0.05, z);
        glVertex3f(  0.12, -0.03, z);
        glVertex3f(  0.12,  0.00, z);	
        //Left horizontal
        glVertex3f(  0.00, -0.08, z);
        glVertex3f( -0.06, -0.10, z);
        glVertex3f( -0.06, -0.12, z);
        glVertex3f(  0.00, -0.12, z);
        //Right horizontal
        glVertex3f(  0.00, -0.08, z);
        glVertex3f(  0.00, -0.12, z);
        glVertex3f(  0.06, -0.12, z);
        glVertex3f(  0.06, -0.10, z);
#endif

#ifdef HSI_AIRPLANE_SYMBOL_CANARD
        glVertex3f(  0.005,  0.05, z);
        glVertex3f( -0.005,  0.05, z);
        glVertex3f( -0.02,  0.00, z);
        glVertex3f(  0.02,  0.00, z);
        glVertex3f(  0.02,  0.00, z);
        glVertex3f( -0.02,  0.00, z);
        glVertex3f( -0.01, -0.12, z);
        glVertex3f(  0.01, -0.12, z);
        //Left wing
        glVertex3f(  0.00, -0.02, z);
        glVertex3f( -0.12, -0.08, z);
        glVertex3f( -0.12, -0.09, z);
        glVertex3f(  0.00, -0.10, z);
        //Right wing
        glVertex3f(  0.00, -0.02, z);
        glVertex3f(  0.10, -0.08, z);
        glVertex3f(  0.12, -0.09, z);
        glVertex3f(  0.00, -0.10, z);	
        //Left Canard
        glVertex3f(  0.00,  0.01, z);
        glVertex3f( -0.06,  0.01, z);
        glVertex3f( -0.06,  0.00, z);
        glVertex3f(  0.00,  0.00, z);
        //Right canard
        glVertex3f(  0.00,  0.01, z);
        glVertex3f(  0.06,  0.01, z);
        glVertex3f(  0.06,  0.00, z);
        glVertex3f(  0.00,  0.00, z);
#endif
    glEnd();
    
    // Fixed heading marker
    glBegin(GL_TRIANGLES);
        glVertex3f( 0.02, 1.10 * roseRadius, z);
        glVertex3f( -0.02, 1.10 * roseRadius, z);
        glVertex3f( 0.0, 1.05 * roseRadius, z);
    glEnd();
    
    // The fixed turn markers
    for (i = 45; i <=315; i=i+45) {
	sinI = sin( i / 57.29 );
	cosI = cos( i / 57.29 );
        glBegin(GL_LINE_STRIP);
	glVertex3f( 1.05 * roseRadius * sinI, 1.05 * roseRadius * cosI, z);
	glVertex3f( 1.10 * roseRadius * sinI, 1.10 * roseRadius * cosI, z);
        glEnd();
    }
    
    // Text is drawn using window coordinates to simplfy use of fontmetrics
    qglColor( QColor( "white" ) );
    QFont font("Fixed", 18, QFont::Bold);
    QFontMetrics fm = fontMetrics();
    // Magnetic Heading
    qglColor( QColor( "white" ) );    
    t = (QString( "%1" ).arg( (int) roundf (magHeading) ));
    QGLWidget::renderText ( pixW2 - fm.width(t)/2 - 2, fm.height()+1, t, font, 2000 ) ;    
    // GPS
    qglColor( gpsColor );    
    font.setPointSize( 10 );
    textBottomPix = fm.height() + 1;
    t = "GPS";
    textLeftPix = 2;
    QGLWidget::renderText ( textLeftPix, textBottomPix, t, font, 2000 ) ;    
    font.setPointSize( 18 );
    textBottomPix = textBottomPix + fm.height() + 3;
    if ( gpsCourseActive ) 
	t = (QString( "%1" ).arg( gpsCourse ));
        else
	t = "---";
    t = t.rightJustify(3,' ');
    QGLWidget::renderText ( textLeftPix, textBottomPix, t, font, 2000 ) ;    
    // NAV
    qglColor( navColor );    
    font.setPointSize( 10 );
    textBottomPix = fm.height() + 1;
    t = "NAV";
    textLeftPix = pixW - fm.width(t) - 5;
    QGLWidget::renderText ( textLeftPix, textBottomPix, t, font, 2000 ) ;    
    font.setPointSize( 18 );
    textBottomPix = textBottomPix + fm.height() + 3;
    if ( navCourseActive ) 
	t = (QString( "%1" ).arg( navCourse ));
        else
	t = "---";
    t = t.rightJustify(3,' ');
    QGLWidget::renderText ( textLeftPix, textBottomPix, t, font, 2000 ) ;     
    // Bug
    qglColor( bugColor );
    font.setPointSize( 10 );
    textBottomPix = fm.height() + 1;
    t = "BUG";
    textLeftPix = pixW2 + pixW2/2 - fm.width(t)/2;
    QGLWidget::renderText ( textLeftPix, textBottomPix, t, font, 2000 ) ;    
    font.setPointSize( 18 );
    textBottomPix = textBottomPix + fm.height() + 3;
    t = (QString( "%1" ).arg( bugHeading ));
    t = t.rightJustify(3,' ');
    QGLWidget::renderText ( textLeftPix, textBottomPix, t, font, 2000 ) ;    
}

void GLHSI::renderHeadingBug( int heading, QColor color )
{	
    GLfloat z;

    z = 3.0;
    qglColor( color );
    
    glPushMatrix();
    glRotatef( magHeading - heading, 0.0, 0.0, 1.0 );
    
    // Draw the bug
    glBegin(GL_QUADS);
        glVertex3f(  0.00 , 1.03 * roseRadius, z);
        glVertex3f( -0.12 , 1.03 * roseRadius, z);
        glVertex3f( -0.12 , .93 * roseRadius, z);
        glVertex3f( -0.03 , .93 * roseRadius, z);
    glEnd();
    glBegin(GL_QUADS);
        glVertex3f( 0.12 , 1.03 * roseRadius, z);
        glVertex3f( 0.00 , 1.03 * roseRadius, z);
        glVertex3f( 0.03 , .93 * roseRadius, z);
        glVertex3f( 0.12 , .93 * roseRadius, z);
    glEnd();
         
    glPopMatrix();
    
}

void GLHSI::renderCourse( int heading, float deviation, QColor color, int drawCourse, int drawDeviation )
{	
    GLfloat z;

    glLineWidth( 5 );
    z = 2.0;
    qglColor( color );
    
    glPushMatrix();
    glRotatef( magHeading - heading, 0.0, 0.0, 1.0 );
    
    // Draw the course
    if( drawCourse ) {
        glBegin(GL_LINES);
            glVertex3f( 0.0 , .9 * roseRadius, z);
            glVertex3f( 0.0 , .7 * roseRadius, z);
            glVertex3f( -0.08 , .82 * roseRadius, z);
	glVertex3f( 0.08 , .82 * roseRadius, z);
	glVertex3f( 0.0 , -.9 * roseRadius, z);
            glVertex3f( 0.0 , -.7 * roseRadius, z);
        glEnd();
    }
    
    // Draw the 2 mile markers
    
    // Draw deviation
    if( drawDeviation ) {
	glTranslatef( deviation / 10 * roseRadius, 0.0, 1.0 );
            glBegin(GL_LINES);
                glVertex3f( 0.0 , .67 * roseRadius, z);
                glVertex3f( 0.0 , -.67 * roseRadius, z);
            glEnd();
    }
    
    glPopMatrix();
    
}

void GLHSI::renderCompassRose()
{	
    GLint i, j;
    GLfloat z, sinI, cosI;
//    GLfloat sinr, cosr;  For positioning compass text
    QString t;

    glLineWidth( 3 );
    z = 1.0;
    qglColor( QColor( "white" ) );
    QFont font("Fixed", 12, QFont::Bold);
    QFontMetrics fm = fontMetrics();
    
    glRotatef( magHeading, 0.0, 0.0, 1.0 );		// Turn the card to magHeading
    
    // The rose degree tics
    for (i = 0; i <=330; i=i+30) {
	sinI = sin( (GLfloat)((450-i) % 360) / 57.29 );
	cosI = cos( (GLfloat)((450-i) % 360) / 57.29 );
//	sinr = sin( (GLfloat)((450-i+(GLint)magHeading) % 360) / 57.29 ); For positioning compass text
//	cosr = cos( (GLfloat)((450-i+(GLint)magHeading) % 360) / 57.29 ); For positioning compass text
	    qglColor( QColor( "white" ) );
	glBegin(GL_LINES);
	    glVertex3f( .9 * roseRadius * cosI, .9 * roseRadius * sinI, z);
	    glVertex3f( roseRadius * cosI, roseRadius * sinI, z);
	glEnd();
	switch( i ) {
	    case 0 : t = "N";
	    break;
	    case 90 : t ="E";
	    break;
	    case 180 : t = "S";
	    break;
	    case 270 : t ="W";
	    break;		  
                default : t = (QString( "%1" ).arg( i/10, -2 ));
	    break;
	}
	QGLWidget::renderText ( .75 * roseRadius * cosI, .75 * roseRadius * sinI,
				z, t, font, 2000 ) ;
	    for (j = 10; j <=20; j=j+10) {
		sinI = sin( (i+j) / 57.29 );
		cosI = cos( (i+j) / 57.29 );
		glBegin(GL_LINES);
		    glVertex3f( .93 * roseRadius * cosI, .93 * roseRadius * sinI, z);
		    glVertex3f( roseRadius * cosI, roseRadius * sinI, z);
		glEnd();
	    }
	    for (j = 5; j <=25; j=j+10) {
		sinI = sin( (i+j) / 57.29 );
		cosI = cos( (i+j) / 57.29 );
		glBegin(GL_LINES);
		    glVertex3f( .96 * roseRadius * cosI, .96 * roseRadius * sinI, z);
		    glVertex3f( roseRadius * cosI, roseRadius * sinI, z);
		glEnd();
	    }
    }
}

void GLHSI::setBugHeading( int degrees )
{
    bugHeading = (degrees % 360);
}

void GLHSI::decBugHeading( )
{
    bugHeading = (( bugHeading - 1 ) % 360);
    if ( bugHeading < 0 ) bugHeading = bugHeading + 360;
}

void GLHSI::incBugHeading( )
{
    bugHeading = (( bugHeading + 1 ) % 360);
}


void GLHSI::setMagHeading( int degrees )
{
    magHeading = degrees;
    while (magHeading > 360)
        magHeading -= 360;
    while (magHeading < 0)
        magHeading += 360;

}

void GLHSI::setGPSCourse( int degrees )
{
    gpsCourse = (degrees % 360);

}

void GLHSI::setGPSCourseActive( bool active )
{
    gpsCourseActive = active;
}

void GLHSI::setGPSDeviation( float nMiles )
{
    gpsDeviation = nMiles;
}

void GLHSI::setGPSDeviation( int nMiles )
{
    gpsDeviation = (GLfloat)nMiles;
}

void GLHSI::setNAVCourse( int degrees )
{
    navCourse = (degrees % 360);
}

void GLHSI::setNAVCourseActive( bool active )
{
    navCourseActive = active;
}

void GLHSI::setNAVDeviation( float nMiles )
{
    navDeviation = nMiles;
}

void GLHSI::setNAVDeviation( int nMiles )
{
    navDeviation = (GLfloat)nMiles;
}

void GLHSI::setILSDeviation( float dev )
{
    ilsDeviation = dev;
}

void GLHSI::setILSDeviation( int dev )
{
    ilsDeviation = (GLfloat)dev;
}

void GLHSI::setILSPathActive( bool active )
{
    ilsPathActive = active;
//    if (active) navCourseActive = active;
}

/*!
  Launch a test dialog for the PFD
*/

void GLHSI::testHSI()
{
    int r;
    TestHSI testhsi( this,0,TRUE,Qt::WStyle_Customize|Qt::WStyle_DialogBorder|Qt::Qt::WStyle_StaysOnTop);
    testhsi.setCaption("HSI Input Overrides");
    testhsi.resize( 333, 222 );
    r = testhsi.exec();
}

TestHSI::TestHSI( QWidget* parent, const char* name,
			  bool modal, WFlags f )
    : QDialog( parent, name, modal, f )
{
    setCaption( "PFD -- Manual Control" );
    // The following Actions & Widgets are for the test rig only, and must be replaced by a/d connections

    QSlider* magHeadingSlider = new QSlider ( 0, 720, 60, 360, QSlider::Horizontal, this, "magHeadingSlider" );
    
    magHeadingSlider->setTickmarks( QSlider::Above);
    connect( magHeadingSlider, SIGNAL(valueChanged(int)), parent, SLOT(setMagHeading(int)) );
    QLabel* magHeadingLabel = new QLabel( "Mag Heading", this, "magHeadingLabel" );
    
    QSlider* gpsCourseSlider = new QSlider ( 0, 720, 60, 390, QSlider::Horizontal, this, "gpsCourseSlider" );
    gpsCourseSlider->setTickmarks( QSlider::NoMarks );
    connect( gpsCourseSlider, SIGNAL(valueChanged(int)), parent, SLOT(setGPSCourse(int)) );
    QLabel* gpsCourseLabel = new QLabel( "GPS Course", this, "gpsCourseLabel" );
    
    QSlider* gpsDeviationSlider = new QSlider ( -15, 15, 2, 1, QSlider::Horizontal, this, "gpsDeviationSlider" );
    gpsDeviationSlider->setTickmarks( QSlider::NoMarks );
    connect( gpsDeviationSlider, SIGNAL(valueChanged(int)), parent, SLOT(setGPSDeviation(int)) );
    QLabel* gpsDeviationLabel = new QLabel( "GPS Deviation (nm)", this, "gpsDeviationLabel" );
    
    QSlider* navCourseSlider = new QSlider ( 0, 720, 60, 330, QSlider::Horizontal, this, "navCourseSlider" );
    navCourseSlider->setTickmarks( QSlider::NoMarks );
    connect( navCourseSlider, SIGNAL(valueChanged(int)), parent, SLOT(setNAVCourse(int)) );
    QLabel* navCourseLabel = new QLabel( "NAV Course", this, "navCourseLabel" );
    
    QSlider* navDeviationSlider = new QSlider ( -15, 15, 2, -3, QSlider::Horizontal, this, "navDeviationSlider" );
    navDeviationSlider->setTickmarks( QSlider::NoMarks );
    connect( navDeviationSlider, SIGNAL(valueChanged(int)), parent, SLOT(setNAVDeviation(int)) );
    QLabel* navDeviationLabel = new QLabel( "NAV Deviation (nm)", this, "navDeviationLabel" );
 
    QSlider* ilsDeviationSlider = new QSlider ( -15, 15, 2, 1, QSlider::Horizontal, this, "ilsDeviationSlider" );
    ilsDeviationSlider->setTickmarks( QSlider::NoMarks );
    connect( ilsDeviationSlider, SIGNAL(valueChanged(int)), parent, SLOT(setILSDeviation(int)) );
    QCheckBox * ilsActiveBox = new QCheckBox( "Glidepath", this, "ilsActiveBox" );
    ilsActiveBox->setChecked( FALSE );
    connect( ilsActiveBox, SIGNAL(toggled(bool)), parent, SLOT(setILSPathActive(bool)) );
    
    // Set up the hardware button actions
    QAction * toggleGPS = new QAction( this, "toggleGPS", TRUE );
    toggleGPS->setAccel( Qt::Key_F1 );
    connect( toggleGPS, SIGNAL( toggled( bool ) ), parent, SLOT( setGPSCourseActive( bool ) ) );
    
    QAction * toggleNAV = new QAction( this, "toggleNAV", TRUE );
    toggleNAV->setAccel( Qt::Key_F2 );
    connect( toggleNAV, SIGNAL( toggled( bool ) ), parent, SLOT( setNAVCourseActive( bool ) ) );
    
    QAction * leftBugHeading = new QAction( this, "leftBugHeading", FALSE );
    leftBugHeading->setAccel( Qt::Key_F5 );
    connect( leftBugHeading, SIGNAL( activated() ), parent, SLOT( decBugHeading( ) ) );

    QAction * rightBugHeading = new QAction( this, "rightBugHeading", FALSE );
    rightBugHeading->setAccel( Qt::Key_F6 );
    connect( rightBugHeading, SIGNAL( activated() ), parent, SLOT( incBugHeading( ) ) );
    
    // Put the hsi in the upper left, with the Course sliders beneath and the heading slider on the right.    
    QGridLayout* gridLayout = new QGridLayout( this, 6, 2, 10, 10, "gridLayout");
    gridLayout->addWidget( magHeadingLabel, 0, 0);
    gridLayout->addWidget( magHeadingSlider, 0, 1);
    gridLayout->addWidget( gpsCourseLabel, 1, 0);
    gridLayout->addWidget( gpsCourseSlider, 1, 1);
    gridLayout->addWidget( gpsDeviationLabel, 2, 0);
    gridLayout->addWidget( gpsDeviationSlider, 2, 1);
    gridLayout->addWidget( navCourseLabel, 3, 0);
    gridLayout->addWidget( navCourseSlider, 3, 1);
    gridLayout->addWidget( navDeviationLabel, 4, 0);
    gridLayout->addWidget( navDeviationSlider, 4, 1);
    gridLayout->addWidget( ilsActiveBox, 5, 0);
    gridLayout->addWidget( ilsDeviationSlider, 5, 1);    
}
