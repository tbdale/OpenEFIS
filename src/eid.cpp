#include "eid.h"


#if defined(Q_CC_MSVC)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif



GLEID::GLEID( QWidget* parent, const char* name, const QGLWidget* shareWidget )
 : QGLWidget( parent, name, shareWidget )
{

} 
