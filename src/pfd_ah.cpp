/*   
    pfd_ah.cpp - Horizon Members for OpenGL PFD Avionics Widget
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

#if defined(Q_CC_MSVC)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

void GLPFD::renderFixedHorizonMarkers()
{	
    GLint i;
    GLfloat z, pixPerDegree, sinI, cosI;

    glLineWidth( 2 );
    z = zfloat;
    pixPerDegree = pixH2 / pitchInView;
    
    // The lubber line
    glBegin(GL_LINE_STRIP);
    qglColor( QColor( "lightgrey" ) );
        glVertex3f( -3.0 * pixPerDegree, 0.0, z);
        glVertex3f( -2.0 * pixPerDegree, 0.0, z);
        glVertex3f( -1.0 * pixPerDegree, -1.0 * pixPerDegree, z);
        glVertex3f( 0.0 * pixPerDegree, 0.0, z);
        glVertex3f( 1.0 * pixPerDegree, -1.0 * pixPerDegree, z);
        glVertex3f( 2.0 * pixPerDegree, 0.0, z);
        glVertex3f( 3.0 * pixPerDegree, 0.0, z);
    glEnd();
    GLUquadricObj* lubberCircle = gluNewQuadric();
    gluPartialDisk( lubberCircle, 3 * pixPerDegree, 3 * pixPerDegree + 1, 360, 2, 0.0, 360.0);
    glBegin(GL_LINES);
        glVertex3f( -1.0 * pixW2, 0.0, z);
        glVertex3f( -0.9 * pixW2, 0.0, z);
        glVertex3f( 0.9 * pixW2, 0.0, z);
        glVertex3f( 1.0 * pixW2, 0.0, z);
    glEnd();
    
    // The fixed roll marker (roll circle marker radius is 15 degrees of pitch, with fixed markers on the outside)
    glBegin(GL_TRIANGLES);
    qglColor( QColor( "white" ) );
        glVertex3f( 0.02 * pixW2, 16 * pixPerDegree, z);
        glVertex3f( -0.02 * pixW2, 16 * pixPerDegree, z);
        glVertex3f( 0.0, 15 * pixPerDegree, z);
    glEnd();
    
    // The fixed roll circle (glu uses north = 0 degrees, increasing clockwise)
    qglColor( QColor( "grey" ) );
    GLUquadricObj* rollCircle = gluNewQuadric();
    gluPartialDisk( rollCircle, 15 * pixPerDegree - 1 , 15 * pixPerDegree + 1, 360, 2, 300.0, 120.0);
    // and the roll tics
    for (i = 5; i <=10; i=i+5) {
	sinI = sin( i / 57.29 );
	cosI = cos( i / 57.29 );
        glBegin(GL_LINE_STRIP);
	glVertex3f( 15 * pixPerDegree * sinI, 15 * pixPerDegree * cosI, z);
	glVertex3f( 16 * pixPerDegree * sinI, 16 * pixPerDegree * cosI, z);
        glEnd();
        glBegin(GL_LINE_STRIP);
	glVertex3f( 15 * pixPerDegree * -sinI, 15 * pixPerDegree * cosI, z);
	glVertex3f( 16 * pixPerDegree * -sinI, 16 * pixPerDegree * cosI, z);
        glEnd();
    }
    for (i = 15; i <=45; i=i+15) {
	sinI = sin( i / 57.29 );
	cosI = cos( i / 57.29 );
        glBegin(GL_LINE_STRIP);
	glVertex3f( 15 * pixPerDegree * sinI, 15 * pixPerDegree * cosI, z);
	glVertex3f( 16 * pixPerDegree * sinI, 16 * pixPerDegree * cosI, z);
        glEnd();
        glBegin(GL_LINE_STRIP);
	glVertex3f( 15 * pixPerDegree * -sinI, 15 * pixPerDegree * cosI, z);
	glVertex3f( 16 * pixPerDegree * -sinI, 16 * pixPerDegree * cosI, z);
        glEnd();
    }
    
}

void GLPFD::renderRollMarkers()
{	
    GLfloat z, pixPerDegree;

    z = zfloat;
    pixPerDegree = pixH2 / pitchInView;  // Put the markers in open space at zero pitch
    
    glBegin(GL_TRIANGLES);
    qglColor( QColor( "white" ) );
        glVertex3f( -0.02  * pixW2, 14 * pixPerDegree, z);
        glVertex3f( 0.02 * pixW2, 14 * pixPerDegree, z);
        glVertex3f( 0.0, 15 * pixPerDegree, z);
    glEnd();

}

void GLPFD::renderPitchMarkers()
{	

    GLint i;
    GLfloat innerTic, outerTic, z, pixPerDegree, iPix;

    glLineWidth( 2 );
    pixPerDegree = pixH2/pitchInView;
    z = zfloat;

    QFont fn("Helvetica", 10, QFont::Bold);
    QFontMetrics fm = fontMetrics();
    
    qglColor( QColor( "white" ) );
    innerTic = 0.1 * pixW2;
    outerTic = 0.13 * pixW2;
    
    for (i = 270; i >0; i=i-10) {
	iPix = (float) i * pixPerDegree;
	QString t (QString( "%1" ).arg( i ));

	glBegin(GL_LINE_STRIP);
	    glVertex3f( -innerTic, iPix, z);
	    glVertex3f( -outerTic, iPix, z);
	    glVertex3f( -outerTic, iPix - .03 * pixW2, z);
	glEnd();
	QGLWidget::renderText ( -0.13 * pixW2 - fm.width(t) - cos(rollRotation/57.29) * fm.width(t)/2,					iPix - fm.ascent() / 2 + sin(rollRotation/57.29) * fm.ascent()/2,
				z, t, fn, 2000 ) ;     
	
	glBegin(GL_LINE_STRIP);
	    glVertex3f( 0.1 * pixW2, iPix, z);
	    glVertex3f( outerTic, iPix, z);
	    glVertex3f( outerTic, iPix - .03 * pixW2, z);
	glEnd();
	QGLWidget::renderText ( 0.13 * pixW2 + fm.width(t) - cos(rollRotation/57.29) * fm.width(t)/2,						iPix - fm.ascent() / 2 + sin(rollRotation/57.29) * fm.ascent()/2,
				z, t, fn, 2000 ) ;     		
    }
    
    for (i = 9; i >=6; i=i-1) {
	iPix = (float) i * pixPerDegree;
	glBegin(GL_LINES);
	    glVertex3f( -0.03  * pixW2, iPix, z);
	    glVertex3f( 0.03  * pixW2, iPix, z);
	glEnd();
    }
    
    glBegin(GL_LINES);
        glVertex3f( -0.1  * pixW2, 5.0*pixPerDegree, z);
        glVertex3f( 0.1  * pixW2, 5.0*pixPerDegree, z);
    glEnd();
    
    for (i = 4; i >=1; i=i-1) {
	iPix = (float) i * pixPerDegree;
	glBegin(GL_LINES);
	    glVertex3f( -0.03  * pixW2, iPix, z);
	    glVertex3f( 0.03  * pixW2, iPix, z);
	glEnd();
    }
    
    glBegin(GL_LINES);
        glVertex3f( -0.1  * pixW2, 0.0, z);
        glVertex3f( 0.1  * pixW2, 0.0, z);
    glEnd();

    qglColor( QColor( "white" ) );
        
    for (i = -1; i>=-4; i=i-1) {
	iPix = (float) i * pixPerDegree;
	glBegin(GL_LINES);
	    glVertex3f( -0.03  * pixW2, iPix, z);
	    glVertex3f( 0.03  * pixW2, iPix, z);
	glEnd();
    }
    
    glBegin(GL_LINES);
        glVertex3f( -0.1  * pixW2, -5.0*pixPerDegree, z);
        glVertex3f( 0.1  * pixW2, -5.0*pixPerDegree, z);
    glEnd();
    
    for (i = -6; i>=-9; i=i-1) {
	iPix = (float) i * pixPerDegree;
	glBegin(GL_LINES);
	    glVertex3f( -0.03  * pixW2, iPix, z);
	    glVertex3f( 0.03  * pixW2, iPix, z);
	glEnd();
    }
    
    for (i = -10; i>=-270; i=i-10) {
	iPix = (float) i * pixPerDegree;
	QString t (QString( "%1" ).arg( i ));
	
	glBegin(GL_LINE_STRIP);
	    glVertex3f( -0.1 * pixW2, iPix, z);
	    glVertex3f( -0.13 * pixW2, iPix, z);
	    glVertex3f( -0.13 * pixW2, iPix + .03 * pixW2, z);
	glEnd();
	QGLWidget::renderText ( -0.13 * pixW2 - fm.width(t) - cos(rollRotation/57.29) * fm.width(t)/2,					iPix - fm.ascent() / 2 + sin(rollRotation/57.29) * fm.ascent()/2,
				z, t, fn, 2000 ) ;     
	
	glBegin(GL_LINE_STRIP);
	    glVertex3f( 0.1 * pixW2, iPix, z);
	    glVertex3f( 0.13 * pixW2, iPix, z);
	    glVertex3f( 0.13 * pixW2, iPix  + .03 * pixW2, z);
	glEnd();
	QGLWidget::renderText ( 0.13 * pixW2 + fm.width(t) - cos(rollRotation/57.29) * fm.width(t)/2,						iPix - fm.ascent() / 2 + sin(rollRotation/57.29) * fm.ascent()/2,
				z, t, fn, 2000 ) ;     
    }
}

void GLPFD::renderTerrain()
{	
    GLfloat pixPitchViewMultiplier, pixOverWidth, z;
    
/*!
  The ModelView has units of +/- 1 about the center.  In order to keep the gyro edges outside of the edges of
  the ViewPort, it is drawn wide to deal with affect of the aspect ratio scaling and the corners during roll
  
  The pitch range in degrees to be viewed must fit the ModelView units of 1. To accomodate this, the gyro must
  be ovesized, hence the multiplier 90/ pitchInView.
*/
    
    pixPitchViewMultiplier = 90.0 / pitchInView * pixH2;
    pixOverWidth = pixW2 * 1.42;
    z = zfloat;

    // Earth
    // Level to -90 pitch

    glBegin(GL_POLYGON);
        qglColor( QColor( 64, 50, 25 ) );
            glVertex3f( -pixOverWidth, -1.0 * pixPitchViewMultiplier, z);
            glVertex3f( pixOverWidth, -1.0 * pixPitchViewMultiplier, z);
        qglColor( QColor( 194, 150, 75  ) );
            glVertex3f( pixOverWidth, 0.0 + 1.0/pixH2, z); // add one pixel to center the horicon line
            glVertex3f( -pixOverWidth, 0.0 + 1.0/pixH2, z); // compensates for TFT rgb pixel offsets
    glEnd();
    
    //  -90 pitch to -180 pitch
    glBegin(GL_POLYGON);
        qglColor( QColor( 194, 150, 75 ) );
            glVertex3f( -pixOverWidth, -2.0 * pixPitchViewMultiplier, z);
            glVertex3f( pixOverWidth, -2.0 * pixPitchViewMultiplier, z);
        qglColor( QColor( 64, 50, 25 ) );
            glVertex3f( pixOverWidth, -1.0 * pixPitchViewMultiplier, z);
            glVertex3f( -pixOverWidth, -1.0 * pixPitchViewMultiplier, z);
    glEnd();
    
    //Once more to fill the ViewPort when pitch approaches 180
    glBegin(GL_POLYGON);
        qglColor( QColor( 194, 150, 75 ) );
            glVertex3f( -pixOverWidth, 2.0 * pixPitchViewMultiplier, z);
            glVertex3f( pixOverWidth, 2.0 * pixPitchViewMultiplier, z);
        qglColor( QColor( 64, 50, 25 ) );
            glVertex3f( pixOverWidth, 3.0 * pixPitchViewMultiplier, z);
            glVertex3f( -pixOverWidth, 3.0 * pixPitchViewMultiplier, z);
    glEnd();
    
    // Sky
    // Level to 90 pitch
    glBegin(GL_POLYGON);
        qglColor( QColor( 0, 0, 255 ) );
            glVertex3f( -pixOverWidth, 0.0 + 1.0/pixH2, z); // add one pixel to center the horizon line
            glVertex3f( pixOverWidth, 0.0 + 1.0/pixH2, z);
        qglColor( QColor( 0, 0, 0 ) );
            glVertex3f( pixOverWidth, 1.0 * pixPitchViewMultiplier, z);
            glVertex3f( -pixOverWidth, 1.0 * pixPitchViewMultiplier, z);
    glEnd();
    
    // 90 pitch to 180 pitch
    glBegin(GL_POLYGON);
        qglColor( QColor( 0, 0, 0 ) );
            glVertex3f( -pixOverWidth, 1.0 * pixPitchViewMultiplier, z);
            glVertex3f( pixOverWidth, 1.0 * pixPitchViewMultiplier, z);
        qglColor( QColor( 0, 0, 255 ) );
            glVertex3f( pixOverWidth, 2.0 * pixPitchViewMultiplier, z);
            glVertex3f( -pixOverWidth, 2.0 * pixPitchViewMultiplier, z);
    glEnd();
    
    //Once more to fill the ViewPort when pitch approaches -180
    glBegin(GL_POLYGON);
        qglColor( QColor( 0, 0, 0 ) );
            glVertex3f( -pixOverWidth, -3.0 * pixPitchViewMultiplier, z);
            glVertex3f( pixOverWidth, -3.0 * pixPitchViewMultiplier, z);
        qglColor( QColor( 0, 0, 255 ) );
            glVertex3f( pixOverWidth, -2.0 * pixPitchViewMultiplier, z);
            glVertex3f( -pixOverWidth,  -2.0 * pixPitchViewMultiplier, z);
    glEnd();

}

/*!
  Set the pitch
*/

void GLPFD::setPitch( int degrees )
{
    pitch = degrees;
    if (degrees > 180) pitch = (degrees - 360);
    if (degrees < -180) pitch = (360 + degrees);
    pitchTranslation = pitch / pitchInView * pixH2;
}


/*!
  Set the roll angle
*/

void GLPFD::setRoll( int degrees )
{
    roll = degrees;
    while (roll > 360)
        roll -= 360;
    while (roll < 0)
        roll += 360;
    rollRotation = roll;
}

