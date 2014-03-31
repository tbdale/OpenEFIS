/*   
    vsi_ah.cpp - Horizon Members for OpenGL PFD Avionics Widget
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

#include "pfd.h"
#include <qstring.h>
#include <qfont.h>
#include <qgl.h>

#if defined(Q_CC_MSVC)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

void GLPFD::renderFixedVSIMarkers()
{	
    GLint i, j;
    GLfloat z, sinI, cosI;

    glLineWidth( 2 );
    z = zfloat;
    
    font = QFont("Fixed", 10, QFont::Bold);
    QFontMetrics fm = fontMetrics();    
    
    // The fixed VSI Arc  (glu uses north = 0 degrees, increasing clockwise)
    qglColor( QColor( "white" ) );
    GLUquadricObj* VSIArc = gluNewQuadric();
    gluPartialDisk( VSIArc, 2.5 * pixH2 - 1 , 2.5 * pixH2 + 1, 360, 2, 250.0, 40.0);
    // and the tics
    for (i = 250; i <=290; i=i+5) {
        sinI = sin( i / 57.29 );
        cosI = cos( i / 57.29 );
        glBegin(GL_LINE_STRIP);
	glVertex3f( 2.5 * pixH2 * sinI, 2.5 * pixH2 * cosI, z);
	glVertex3f( 2.44 * pixH2 * sinI, 2.44 * pixH2 * cosI, z);
        glEnd();
	if( i >= 255 && i < 270 || i >= 275 && i < 290 ) {
	    t = QString( "%1" ).arg( (i - 270) * 100 ); // Recalculating trig in next line to avoid a compile error.
	    QGLWidget::renderText ( 2.44 * pixH2 * sin(i/57.29) + 2, 2.44 * pixH2 * cos(i/57.29) - fm.ascent() / 2, z, t, font, 2000 ) ;     
	}
	if( i >= 260 && i < 280) {
	    for (j = i+1; j <= i+4; j=j+1) {
	        sinI = sin( j / 57.29 );
	        cosI = cos( j / 57.29 );
	        glBegin(GL_LINE_STRIP);
		glVertex3f( 2.5 * pixH2 * sinI, 2.5 * pixH2 * cosI, z);
		glVertex3f( 2.48 * pixH2 * sinI, 2.48 * pixH2 * cosI, z);
	        glEnd();
	    }
	}
       }
    // Draw the needle
    VSINeedleAngle = (float) VSIValue / 100.0 + 270.0;
    sinI = sin( VSINeedleAngle / 57.29 );
    cosI = cos( VSINeedleAngle / 57.29 );
    glLineWidth( 4 );
    glBegin(GL_LINE_STRIP);
        glVertex3f( 2.48 * pixH2 * sinI, 2.48 * pixH2 * cosI, z-.1);
        glVertex3f( 2.2 * pixH2 * sinI, 2.2 * pixH2 * cosI, z-.1);
    glEnd();
}

void GLPFD::renderVSIValue()
{
    GLfloat z;

    glLineWidth( 2 );
    z = zfloat;
    
    qglColor( QColor( "white" ) );
    font = QFont("Fixed", 12, QFont::Bold);
    QFontMetrics fm = fontMetrics();
    // VSI Display
    qglColor( QColor( "white" ) );    
    t = (QString( "%1" ).arg( VSIValue  ));
    QGLWidget::renderText ( (GLint) ( 1.52 * pixH2) - fm.width(t), -fm.height()/2 + 3, z+.1, t, font, 2000 ) ;  
    // Mask over the moving tape for the value display box
    qglColor( QColor( "black" ) );
    glBegin(GL_QUADS);
        glVertex3f( 1.55 * pixH2, -.8*fm.height(), z);
        glVertex3f( 1.55 * pixH2, .8* fm.height(), z);
        glVertex3f( 1.3 * pixH2, .8*fm.height(), z);
        glVertex3f( 1.3 * pixH2, -.8*fm.height(), z);
    glEnd();
    
    // The value display box line
    glBegin(GL_LINE_STRIP);
    qglColor( QColor( "white" ) );
        glVertex3f( 1.55 * pixH2, -.8*fm.height(), z+.1);
        glVertex3f( 1.55 * pixH2, .8* fm.height(), z+.1);
        glVertex3f( 1.3 * pixH2, .8*fm.height(), z+.1);
        glVertex3f( 1.3 * pixH2, -.8*fm.height(), z+.1);
        glVertex3f( 1.55 * pixH2, -.8*fm.height(), z+.1);
    glEnd();
   
}

/*!
  Set the vertical speed
*/

void GLPFD::setVSI( int speed )
{
    VSIValue = speed;
}
