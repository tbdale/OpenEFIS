#ifndef CUBE_H
#define CUBE_H

#include <qgl.h>
#include <qdialog.h>

class Cube : public QGLWidget
{
    Q_OBJECT
public:
    Cube(QWidget *parent = 0, const char *name = 0,const QGLWidget* = 0);

public:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    void draw();
    int faceAtPosition(const QPoint &pos);

    GLfloat rotationX;
    GLfloat rotationY;
    GLfloat rotationZ;
    QColor faceColors[6];
    QPoint lastPos;
};

#endif
