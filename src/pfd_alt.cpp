/*   
    pfd_alt.cpp - Altimeter Members for an OpenGL PFD Avionics Widget
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

void GLPFD::renderFixedALTMarkers()
{	
    GLfloat z;

    QString t;

    glLineWidth( 2 );
    z = zfloat;
    
    // The tapes are positioned left & right of the roll circle, occupying the space based
    // on the vertical dimension, from .6 to 1.0 pixH2.  This makes the basic display
    // square, leaving extra space outside the edges for terrain which can be clipped if reqd.
    
    qglColor( QColor( "white" ) );
    font = QFont("Fixed", 18, QFont::Bold);
    QFontMetrics fm = fontMetrics();
    // Altimeter Display
    qglColor( QColor( "white" ) );    
    t = (QString( "%1" ).arg( 10 * ( (MSLValue + 5) / 10 )  ));
    QGLWidget::renderText ( (GLint) ( 1.05 * pixH2) - fm.width(t), -fm.height()/2 + 1, z+.1, t, font, 2000 ) ;  
    // Mask over the moving tape for the value display box
    qglColor( QColor( "black" ) );
    glBegin(GL_QUADS);
        glVertex3f( 1.15 * pixH2, -fm.height(), z+.1);
        glVertex3f( 1.15 * pixH2, fm.height(), z+.1);
        glVertex3f( 0.80 * pixH2, fm.height(), z+.1);
        glVertex3f( 0.80 * pixH2, -fm.height(), z+.1);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex3f( 0.80 * pixH2, fm.height()/2, z+.1);
        glVertex3f( 0.75 * pixH2, 0.0, z);
        glVertex3f( 0.80 * pixH2, -fm.height()/2, z+.1);
    glEnd();
    
    // The value display box line
    glBegin(GL_LINE_STRIP);
    qglColor( QColor( "white" ) );
        glVertex3f( 1.15 * pixH2, -fm.height(), z+.2);
        glVertex3f( 1.15 * pixH2, fm.height(), z+.2);
        glVertex3f( 0.80 * pixH2, fm.height(), z+.2);
        glVertex3f( 0.80 * pixH2, fm.height()/2, z+.2);
        glVertex3f( 0.75 * pixH2, 0.0, z+.2);
        glVertex3f( 0.80 * pixH2, -fm.height()/2, z+.2);
        glVertex3f( 0.80 * pixH2, -fm.height(), z+.2);
        glVertex3f( 1.15 * pixH2, -fm.height(), z+.2);
    glEnd();
   
}


void GLPFD::renderMSLMarkers()
{	

    GLint i, j;
    GLfloat innerTic, midTic, outerTic, z, pixPerUnit, iPix;

    glLineWidth( 2 );
    pixPerUnit = pixH2/MSLInView ;
    z = zfloat;

    font = QFont("Fixed", 12, QFont::Bold);
    QFontMetrics fm = fontMetrics();
    
    innerTic = 0.70 * pixH2;	// inner & outer are relative to the vertical scale line
    outerTic = 0.80 * pixH2;
    midTic = 0.77 * pixH2;
    
    // The numbers & tics for the tape
    qglColor( QColor( "white" ) );
    for (i = MSLMaxDisp; i >=MSLMinDisp; i=i-100) {
	iPix = (float) i * pixPerUnit;
	t = QString( "%1" ).arg( i );
            glBegin(GL_LINE_STRIP);
	    glVertex3f( innerTic, iPix, z);
	    glVertex3f( outerTic, iPix, z);
	glEnd();
    QGLWidget::renderText ( outerTic + fm.width(t)/2, iPix - fm.ascent() / 2, z, t, font, 2000 ) ;     
	for (j = i + 20; j < i+90; j=j+20){
	    iPix = (float) j * pixPerUnit;
	    glBegin(GL_LINE_STRIP);
	        glVertex3f( innerTic, iPix, z);
	        glVertex3f( midTic, iPix, z);
	    glEnd();
            }
    }
    // The vertical scale bar
    glBegin(GL_LINE_STRIP);
        glVertex3f( innerTic,MSLMinDisp, z);
        glVertex3f( innerTic, (MSLMaxDisp + 100) * pixPerUnit, z);
    glEnd();
    
}

/*!
  Set the indicator
*/

void GLPFD::setMSL( int value )
{
    MSLValue = value;
    MSLTranslation = MSLValue / MSLInView  * pixH2;
    updateGL();
}

/*!
  Set the barometric pressure
*/

void GLPFD::setBaro( float inHg )
{
    baroPressure = inHg;
}
