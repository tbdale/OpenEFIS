#ifndef EID_H
#define EID_H

#include <qgl.h>

class GLEID : public QGLWidget
{
    Q_OBJECT
        public:
        
        GLEID( QWidget* parent, const char* name, const QGLWidget* shareWidget );

        GLEID( const QGLFormat& format, QWidget* parent, const char* name, 
           const QGLWidget* shareWidget=0 );
        ~GLEID();
        


} ;
#endif EID_H

