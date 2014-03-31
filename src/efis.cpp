/*   
    efis.cpp - an application window launch program for OpenGL Avionics Widgets
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

#include <qslider.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qaction.h>
#include <qframe.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qdatetime.h>

#include "efis.h"
#include "pfd.h"
#include "hsi.h"
#include "eis/eis.h"
#include "stamp_sensors.h"
#include "constants.h"

EFIS::EFIS( QWidget* parent, const char* name, WFlags f)
	    : QWidget( parent, name, f )
{
    QAction * actionQuit = new QAction( this, "actionQuit", FALSE );
    actionQuit->setAccel( Key_Escape );
    connect( actionQuit, SIGNAL( activated() ), qApp, SLOT(quit() ) );

    this->setPaletteForegroundColor( "white" );
    this->setPaletteBackgroundColor( "black" );
    
    // Set up the frames for a 1024x768 screen.  Frame names are based on intended
    // content, where known.   The intended order is 
    // first row:	 frameAnnunLeft, frameAnnunCenter, frameAnnunRight
    // second row:	frameAirframe, framePFD, framePower
    // third row:	frameButtonLeft, frameInfoLeft, frameNav, frameInfoRight, frameButtonRight
    
    QFrame* frameAnnunLeft = new QFrame( this, "frameAnnunLeft" );
    frameAnnunLeft->setMinimumSize( 237, 68 );	// Force a specific size for inspection
    frameAnnunLeft->setMaximumSize( 237, 68 );
    frameAnnunLeft->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameAnnunLeft->setLineWidth( 1 );
    frameAnnunLeft->setPaletteBackgroundColor( "black" );    
    frameAnnunLeft->setPaletteForegroundColor( "white" );  
    
    QFrame* frameAnnunCenter = new QFrame( this, "frameAnnunCenter" );
    frameAnnunCenter->setMinimumSize( 550, 68 );	// Force a specific size for inspection
    frameAnnunCenter->setMaximumSize( 550, 68 );
    frameAnnunCenter->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameAnnunCenter->setLineWidth( 1 );
    frameAnnunCenter->setPaletteBackgroundColor( "black" );    
    frameAnnunCenter->setPaletteForegroundColor( "white" );  
    
    QFrame* frameAnnunRight = new QFrame( this, "frameAnnunRight" );
    frameAnnunRight->setMinimumSize( 237, 68 );	// Force a specific size for inspection
    frameAnnunRight->setMaximumSize( 237, 68 );
    frameAnnunRight->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameAnnunRight->setLineWidth( 1 );
    frameAnnunRight->setPaletteBackgroundColor( "black" );    
    frameAnnunRight->setPaletteForegroundColor( "white" );    
    
    QFrame* frameAirframe = new QFrame( this, "frameAirframe" );
    frameAirframe->setMinimumSize( 237, 350 );	// Force a specific size for inspection
    frameAirframe->setMaximumSize( 237, 350 );
    frameAirframe->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameAirframe->setLineWidth( 1 );
    frameAirframe->setPaletteBackgroundColor( "black" );    
    frameAirframe->setPaletteForegroundColor( "white" );  
    
    QFrame *framePFD = new QFrame( this, "framePFD" );
    framePFD->setMinimumSize( 550, 350 );	// Force a specific size for inspection
    framePFD->setMaximumSize( 550, 350 );
    framePFD->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    framePFD->setLineWidth( 1 );
    framePFD->setPaletteBackgroundColor( "black" );    
    framePFD->setPaletteForegroundColor( "white" );  
    
    QFrame* framePower = new QFrame( this, "framePower" );
    framePower->setMinimumSize( 237, 350 );	// Force a specific size for inspection
    framePower->setMaximumSize( 237, 350 );
    framePower->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    framePower->setLineWidth( 1 );
    framePower->setPaletteBackgroundColor( "black" );    
    framePower->setPaletteForegroundColor( "white" );  
    
    QFrame* frameButtonLeft = new QFrame( this, "frameButtonLeft" );
    frameButtonLeft->setMinimumSize( 87, 350 );	// Force a specific size for inspection
    frameButtonLeft->setMaximumSize( 87, 350 );
    frameButtonLeft->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameButtonLeft->setLineWidth( 1 );
    frameButtonLeft->setPaletteBackgroundColor( "black" );    
    frameButtonLeft->setPaletteForegroundColor( "white" );  
    
    QFrame* frameInfoLeft = new QFrame( this, "frameInfoRight" );
    frameInfoLeft->setMinimumSize( 150, 350 );	// Force a specific size for inspection
    frameInfoLeft->setMaximumSize( 150, 350 );
    frameInfoLeft->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameInfoLeft->setLineWidth( 1 );
    frameInfoLeft->setPaletteBackgroundColor( "black" );    
    frameInfoLeft->setPaletteForegroundColor( "white" );  
    
    QFrame* frameNav = new QFrame( this, "frameNav" );
    frameNav->setMinimumSize( 550, 350 );	// Force a specific size for inspection
    frameNav->setMaximumSize( 550, 350 );
//    frameNav->setFrameStyle( QFrame::Sunken | QFrame::Panel );
//    frameNav->setLineWidth( 1 );
    frameNav->setPaletteBackgroundColor( "black" );    
    frameNav->setPaletteForegroundColor( "white" );  
        
    QFrame* frameInfoRight = new QFrame( this, "frameInfoRight" );
    frameInfoRight->setMinimumSize( 150, 350 );	// Force a specific size for inspection
    frameInfoRight->setMaximumSize( 150, 350 );
    frameInfoRight->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameInfoRight->setLineWidth( 1 );
    frameInfoRight->setPaletteBackgroundColor( "black" );    
    frameInfoRight->setPaletteForegroundColor( "white" );  
    
    QFrame* frameButtonRight = new QFrame( this, "frameButtonRight" );
    frameButtonRight->setMinimumSize( 87, 350 );	// Force a specific size for inspection
    frameButtonRight->setMaximumSize( 87, 350 );
    frameButtonRight->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    frameButtonRight->setLineWidth( 1 );
    frameButtonRight->setPaletteBackgroundColor( "black" );    
    frameButtonRight->setPaletteForegroundColor( "white" );  

    // Box up the frames
    QVBoxLayout* mainLayout = new QVBoxLayout( this );
        QHBoxLayout* topLayout = new QHBoxLayout( mainLayout );    
            topLayout->addWidget( frameAnnunLeft );
	topLayout->addWidget( frameAnnunCenter );
            topLayout->addWidget( frameAnnunRight );
        QHBoxLayout* centerLayout = new QHBoxLayout( mainLayout );    
            centerLayout->addWidget( frameAirframe );
	centerLayout->addWidget( framePFD );
            centerLayout->addWidget( framePower );
        QHBoxLayout* bottomLayout = new QHBoxLayout( mainLayout );  
            bottomLayout->addWidget( frameButtonLeft );
	bottomLayout->addWidget( frameInfoLeft );
            bottomLayout->addWidget( frameNav );
	bottomLayout->addWidget( frameInfoRight );
            bottomLayout->addWidget( frameButtonRight );
    
// Create the Annunciators
    QLabel* labelAnnunLeft = new QLabel( "Air Data", frameAnnunLeft, "labelAnnunLeft" ); //None yet
    // Load into the frame
    QHBoxLayout* boxAnnunLeft = new QHBoxLayout( frameAnnunLeft, 15, 5, "boxAnnunLeft");
    boxAnnunLeft ->addWidget( labelAnnunLeft );
	    
    QLabel* labelAnnunCenter = new QLabel( "Alerts & Warnings", frameAnnunCenter, "labelAnnunCenter" ); 
    // Load into the frame
    QHBoxLayout* boxAnnunCenter = new QHBoxLayout( frameAnnunCenter, 15, 5, "boxAnnunCenter");
    boxAnnunCenter ->addWidget( labelAnnunCenter );
	    
    QLabel* labelAnnunRight = new QLabel( "Flight Data", frameAnnunRight, "labelAnnunRight" );
    
    // Load into the frame
    QHBoxLayout* boxAnnunRight = new QHBoxLayout( frameAnnunRight, 15, 5, "boxAnnunRight");
    boxAnnunRight ->addWidget( labelAnnunRight );
	  
// Create the Airframe Data
    QLabel* labelAirframe = new QLabel( "Airframe Data\n- Electrical\n- G meter\n"
					"- Trims\n- Flaps", frameAirframe, "labelAirframe" ); 
    // Load into the frame
    QHBoxLayout* boxAirframe = new QHBoxLayout( frameAirframe, 15, 5, "boxAirframe");
    boxAirframe ->addWidget( labelAirframe );
    
// Create the PFD
    PFD = new GLPFD( framePFD, "PFD");
    // Load into the frame
    QHBoxLayout* boxPFD = new QHBoxLayout( framePFD, 5, 0, "cvbox");
    boxPFD->addWidget( PFD );   
    // Set up the PFD test dialog activator
    QAction * actionPFDTest = new QAction( this, "actionPFDTest", FALSE );
    actionPFDTest->setAccel( CTRL+Key_P );
    connect( actionPFDTest, SIGNAL( activated() ), PFD, SLOT(testPFD() ) );    

// Create the Engine Information System panel
    EIS = new GLEIS(framePower,"eis");
    QHBoxLayout* boxPower = new QHBoxLayout( framePower, 15, 5, "boxPower");
    boxPower ->addWidget(EIS);

    // Setup the EIS test dialog activator
    QAction * actionEISTest = new QAction( this, "actionEISTest", FALSE );
    actionEISTest->setAccel( CTRL+Key_E );
    connect( actionEISTest, SIGNAL( activated() ), EIS, SLOT(testEIS() ) );
    
// Create the Left Buttons
    QLabel* labelButtonLeft = new QLabel( "Left Button\nFrame\n\n"
	                                                "Controls\nbuttons are\nmapped to\nthe following\nkeys:\n" 				            "F1: GPS\nF2: NAV\nF5: + Bug\nF6: - Bug", 
					      frameButtonLeft, "labelButtonLeft" );
    // Load into the frame
    QHBoxLayout* boxButtonLeft = new QHBoxLayout( frameButtonLeft, 5, 5, "boxButtonLeft");
    boxButtonLeft ->addWidget( labelButtonLeft );
    
// Create the Checklist
    QLabel* labelInfoLeft = new QLabel( "Checklist Frame\n\nThe following keys\n"
					"will activate the test\n"
					"dialogs:\n\n"
					"- ctrl-P PFD\n"
					"- ctrl-N HSI\n"
					"- ctrl-E Poweplant\n"
//					"- ctrl-A Airframe\n"
//					"- ctrl-F Fuel\n"
					"\n"
					"Close all dialogs and\n"
					"press Esc from this\n"
					"screen to quit", frameInfoLeft, "labelInfoLeft" ); 
    // Load into the frame
    QHBoxLayout* boxInfoLeft = new QHBoxLayout( frameInfoLeft, 15, 5, "boxInfoLeft");
    boxInfoLeft ->addWidget( labelInfoLeft );
    
// Create the HSI
    HSI = new GLHSI( frameNav, "HSI");
    // Load into the frame
    QHBoxLayout* boxNav = new QHBoxLayout( frameNav, 5, 0, "boxNav");
    boxNav->addWidget( HSI );     
    // Set up the HSI test dialog activator
    QAction * actionHSITest = new QAction( this, "actionHSITest", FALSE );
    actionHSITest->setAccel( CTRL+Key_N );
    connect( actionHSITest, SIGNAL( activated() ), HSI, SLOT(testHSI() ) );
    // Set up the HSI hardware button actions
    QAction * toggleGPS = new QAction( HSI, "toggleGPS", TRUE );
    toggleGPS->setAccel( Qt::Key_F1 );
    connect( toggleGPS, SIGNAL( toggled( bool ) ), HSI, SLOT( setGPSCourseActive( bool ) ) );
    //
    QAction * toggleNAV = new QAction( HSI, "toggleNAV", TRUE );
    toggleNAV->setAccel( Qt::Key_F2 );
    connect( toggleNAV, SIGNAL( toggled( bool ) ), HSI, SLOT( setNAVCourseActive( bool ) ) );
    //
    QAction * leftBugHeading = new QAction( HSI, "leftBugHeading", FALSE );
    leftBugHeading->setAccel( Qt::Key_F5 );
    connect( leftBugHeading, SIGNAL( activated() ), HSI, SLOT( decBugHeading( ) ) );
    //
    QAction * rightBugHeading = new QAction( HSI, "rightBugHeading", FALSE );
    rightBugHeading->setAccel( Qt::Key_F6 );
    connect( rightBugHeading, SIGNAL( activated() ), HSI, SLOT( incBugHeading( ) ) );

// Create the Fuel Info
    QLabel* labelInfoRight = new QLabel( "Fuel Data\n"
					 "- Fuel Qtys\n"
					 "- Fuel Flow\n"
					 "- Fuel Pressure", frameInfoRight, "labelInfoRight" );
    // Load into the frame
    QHBoxLayout* boxInfoRight = new QHBoxLayout( frameInfoRight, 15, 5, "boxInfoRight");
    boxInfoRight ->addWidget( labelInfoRight );
    
// Create the Right Buttons
    QLabel* labelButtonRight = new QLabel( "Right Button\nFrame\n\n"
	                                                "Controls\nbuttons are\nmapped to\nthe following\nkeys:\n"
                                                        "h: -500ft\nj: -100ft\nk: +100ft\nl: +500ft", 
					      frameButtonRight, "labelButtonRight" );
    // Load into the frame
    QHBoxLayout* boxButtonRight = new QHBoxLayout( frameButtonRight, 15, 5, "boxButtonRight");
    boxButtonRight->addWidget( labelButtonRight );
    
    
    // Frame Rate Timing Routine, comment this for normal operation    
   //QTimer *timer = new QTimer( this );
    //connect( timer, SIGNAL(timeout()), this, SLOT(frameRateTest()) );
    // time = QTime::currentTime();   
    //time.start();
    //timer->start( 0, FALSE );
    //End of Frame Rate Routine
    
    // Frame Rate Fixed Time Routine, comment this for normal operation    
    rpm=0;
    EIS->setMAP((GLfloat)0.0);
    //sensors = new StampSensors();  // Basic Stamp AHRS sim 
//     QTimer *timer = new QTimer( this );
//     connect( timer, SIGNAL(timeout()), this, SLOT(frameRateTest()) );
//     time = QTime::currentTime();
//     time.start();
//     timer->start( 10, FALSE );
    //End of Frame Rate Routine
       
    QTimer *update_timer = new QTimer( this );
    connect( update_timer, SIGNAL(timeout()), this, SLOT(UpdateInstruments()) );
    time = QTime::currentTime();
    time.start();
    update_timer->start( UPDATE_INTERVAL / 1000 /* Us per ms */ );

}

void EFIS::frameRateTest( )
{
    //Exercise all of the display items, and place the frames/second on the airspeed tape
    float ms=0.0f;
    ms=time.restart();
    if (ms > 1000) 
        ms=1000.0f;
    float heading=ms/5;
    if (heading >359)
        heading=0;
    rpm+=10;
    HSI->setMagHeading(heading);
    PFD->setIAS( (1000 / ms)+100 );
    EIS->setRPM(rpm);
    EIS->setMAP((float) rpm/75);
    //PFD->setIAS( 1000 / 8); //TBD
}





