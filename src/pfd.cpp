/*   
    pfd.cpp - A Primary Flight Display OpenGL Avionics Widget
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
    - add dive/climb arrows
    - add standard rate turn indicators
    - add hnav & vnav indicators
    - add crab orientation
    - add flight director bars
    - add 3d terrain mapping
    - convert to designer plugin
*/

#include <math.h>

#include "pfd.h"
#include <qstring.h>
#include <qfont.h>
#include <qgl.h>

#include <qlayout.h>
#include <qslider.h>
#include <qlabel.h>
#include <qpushbutton.h>


#if defined(Q_CC_MSVC)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

/*!
  Create a GLPFD widget
*/

GLPFD::GLPFD( QWidget* parent, const char* name, const QGLWidget* shareWidget )
    : QGLWidget( parent, name, shareWidget )
{
    pitchTranslation = rollRotation = 0.0;	// default object translation and rotation
    
    IASTranslation = 0.0;			// default IAS tape translation
    IASValue = 0;				// The default to show if no IAS calls come in    
    
    MSLTranslation = 0.0;			// default MSL tape translation
    MSLValue = 0;			// The default to show if no MSL calls come in    
    
    VSIValue = 0;				// The default vertical speed
    
    // These should come from a cal file
    
    Vs0 = 59;
    Vs1 = 70;
    Vx = 86;
    Vy = 103;
    Vfe = 122;
    Va = 126;
    Vno = 174;
    Vne = 226;
    IASMaxDisp = 400;
    MSLMinDisp = -5000;
    MSLMaxDisp = 35000;
}


/*!
  Create a GLPFD widget
*/

GLPFD::GLPFD( const QGLFormat& format, QWidget* parent, const char* name, 
	      const QGLWidget* shareWidget )
    : QGLWidget( format, parent, name, shareWidget )
{
    pitchTranslation = rollRotation = 0.0;	// default object translation and rotation
    
    IASTranslation = 0.0;			// default object translation
    IASValue = 0;				// The default to show if on IAS calls come in    
    
    MSLTranslation = 0.0;			// default MSL tape translation
    MSLValue = 0;			// The default to show if no MSL calls come in    
    
    VSIValue = 0;				// The default vertical speed
    
    // These should come from a cal file
    
    Vs0 = 59;
    Vs1 = 70;
    Vx = 86;
    Vy = 103;
    Vfe = 122;
    Va = 126;
    Vno = 174;
    Vne = 226;
    IASMaxDisp = 400;
    MSLMinDisp = -5000;
    MSLMaxDisp = 35000;
}


/*!
  Release allocated resources
*/

GLPFD::~GLPFD()
{
    makeCurrent();
}


/*!
  Set up the OpenGL rendering state, and define display list
*/

void GLPFD::initializeGL()
{
    qglClearColor( QColor( eraseColor()) );		// Let OpenGL clear to background color
    glEnable( GL_DEPTH_TEST );
}

/*!
  Set up the OpenGL view port, matrix mode, etc.
*/

void GLPFD::resizeGL( int w, int h )
{
    // Capture the window scaling for use by the rendering functions
    pixW = w;
    pixH = h;
    pixW2 = w/2;
    pixH2 = h/2;
    
    // Set the window size specific scales, positions and sizes (nothing dynamic yet...)
    pitchInView = 25.0;  			// degrees to display from horizon to top of viewport
    IASInView = 25.0;  			// IAS units to display from center to top of viewport
    MSLInView  = 250.0;  			// IAS units to display from center to top of viewport
    
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(-pixW2, pixW2, -pixH2, pixH2, 0.0, 10.0);
}


void GLPFD::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0, 0.0, 0.0 );
    glScalef( 1.0, 1.0, 1.0 ); 
    
    zfloat = -1.0;					// Set the z position for ortho drawing
    renderFixedASIMarkers();			// Draw the ASI fixed markings
    renderFixedALTMarkers();			// Draw the ALT fixed markings
    renderVSIValue();				// Draw the VSI digital display
    glTranslatef( 3.7 * pixH2, 0.0, 0.0);		// Slide over to draw the VSI gauge  
    renderFixedVSIMarkers();
    
    glTranslatef( - 3.7 * pixH2, -IASTranslation, 0.0);	// Slide ASI to current value
    renderIASMarkers();				// Draw the ASI tape

    glTranslatef( 0.0, IASTranslation-MSLTranslation, 0.0);	// Slide ALT to current value
    renderMSLMarkers();				// Draw the ASI tape
    
    zfloat = -2.0;					// Put the horizon behind the tapes
    glTranslatef( 0.0, MSLTranslation, 0.0);		// Slide back,
    renderFixedHorizonMarkers();			// draw the horizon fixed markings
    glRotatef( rollRotation, 0.0, 0.0, 1.0 );		// then Roll,
    renderRollMarkers();				// and draw the markers that roll but do not  pitch
    
    glTranslatef( 0.0, pitchTranslation, 0.0);		// Pitch while rolled,
    renderPitchMarkers();				// draw the pitch markers,
    zfloat = -5.0;					// Put the terrain behind everything else
    renderTerrain();				// draw the terrain
}

/*!
  Launch a test dialog for the PFD
*/

void GLPFD::testPFD()
{
    int r;
    TestPFD testpfd( this,0,TRUE,Qt::WStyle_Customize|Qt::WStyle_DialogBorder|Qt::Qt::WStyle_StaysOnTop);
    testpfd.setCaption("PFD Input Overrides");
    testpfd.resize( 333, 222 );
    r = testpfd.exec();
}

TestPFD::TestPFD( QWidget* parent, const char* name,
			  bool modal, WFlags f )
    : QDialog( parent, name, modal, f )
{
    setCaption( "PFD -- Manual Control" );
    
    // Define the Controls
    QSlider* pitchSlider = new QSlider ( -360, 360, 60, 0, QSlider::Horizontal, this, "pitchSlider" );
    pitchSlider->setTickmarks( QSlider::Above );
    QLabel* pitchSliderLabel = new QLabel( "Pitch", this, "pitchSliderLabel" );
    connect( pitchSlider, SIGNAL(valueChanged(int)), parent, SLOT(setPitch(int)) );

    QSlider* rollSlider = new QSlider ( -360, 360, 60, 0, QSlider::Horizontal, this, "rollSlider" );
    QLabel* rollSliderLabel = new QLabel( "Roll", this, "rollSliderLabel" );
    connect( rollSlider, SIGNAL(valueChanged(int)), parent, SLOT(setRoll(int)) );
    
    QSlider* IASSlider = new QSlider ( 0, 360, 6, 0, QSlider::Horizontal, this, "IASSlider" );
    QLabel* IASSliderLabel = new QLabel( "IAS", this, "IASSliderLabel" );
    connect( IASSlider, SIGNAL(valueChanged(int)), parent, SLOT(setIAS(int)) );
    
    QSlider* MSLSlider = new QSlider ( -3000, 12000, 60, 0, QSlider::Horizontal, this, "MSLSlider" );
    QLabel* MSLSliderLabel = new QLabel( "MSL", this, "MSLSliderLabel" );
    connect( MSLSlider, SIGNAL(valueChanged(int)), parent, SLOT(setMSL(int)) );
 
    QSlider* VSISlider = new QSlider ( -5000, 5000, 50, 0, QSlider::Horizontal, this, "VSISlider" );
    QLabel* VSISliderLabel = new QLabel( "VSI", this, "VSISliderLabel" );
    connect( VSISlider, SIGNAL(valueChanged(int)), parent, SLOT(setVSI(int)) );
    
    QPushButton* cancelPushButton = new QPushButton( "&Close Dialog", this );
    connect( cancelPushButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    
    // Put manual PFD controls in the window
    QGridLayout* PFDControlBox = new QGridLayout( this, 6, 2, 5, 5, "gridLayout");
    PFDControlBox->addWidget( pitchSliderLabel, 0, 0 );
    PFDControlBox->addWidget( pitchSlider, 0, 1 );
    PFDControlBox->addWidget( rollSliderLabel, 1, 0 );
    PFDControlBox->addWidget( rollSlider, 1, 1 );
    PFDControlBox->addWidget( IASSliderLabel, 2, 0 );
    PFDControlBox->addWidget( IASSlider, 2, 1 );
    PFDControlBox->addWidget( MSLSliderLabel, 3, 0 );
    PFDControlBox->addWidget( MSLSlider, 3, 1 );
    PFDControlBox->addWidget( VSISliderLabel, 4, 0 );
    PFDControlBox->addWidget( VSISlider, 4, 1 );
    PFDControlBox->addWidget( cancelPushButton, 5, 1 );

}
