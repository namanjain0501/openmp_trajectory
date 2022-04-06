// Libraries include
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<vector>
#include<iostream>
#include<stdbool.h>
#include<cmath>
#include<utility>
#include<tuple>

// OpenGL graphics libraries.
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include<GL/gl.h>
#include<GL/glu.h>

using namespace std;

#define TRJ "output.bin"
#define TIMESTEPS 100
#define DELTA 0.01

//Input and output functions
typedef struct  {
    int id;
    double x, y, z;
} Point;

typedef struct{
    int n;
    int mass;
    float radius;
    float time;
} Spheres;

typedef struct{
    int height, width, depth;
} Container;

vector<Point> Coords;
Container box;
Spheres balls;
bool readBox = true;
FILE *f1;
int steps = 0;

Container readContainer();
Spheres readSpheres();
void extractCurrentCoords(vector<Point> &A, int n) ;
void printArray(vector<Point> &A, int n) ;

// Graphics Functions
void display(void);
void drawCube(Container box);
void drawSpheres(void);
void init(void);
void timer(int);
void reshape(GLint, GLint);
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar );

int main(int argc, char** argv)
{
    // initialize and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("HP3 OpenMP TermProject");

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutReshapeFunc(reshape);

    init();

    glutMainLoop();
    fclose(f1);

    /*f1 = fopen(TRJ, "rb");

    box = readContainer();
    balls = readSpheres();
    // Assign space to store coordinates of the sphere
    Point temp;
    temp.id = temp.x = temp.y = temp.z = 0;
    Coords.assign(balls.n, temp) ;

    extractCurrentCoords(Coords, balls.n);
    printArray(Coords, 10);

    extractCurrentCoords(Coords, balls.n);
    printArray(Coords, 10);

    extractCurrentCoords(Coords, balls.n);
    printArray(Coords, 10);*/

    return 0;
}

Container readContainer(){
    Container box;
    /*while(getline(fin, line)) {
        if (line.find("Container length (in m) = ") != string::npos) {
            text = "Container length (in m) = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            box.height = stoi(line);
        } else if (line.find("Container width (in m) = ") != string::npos) {
            text = "Container width (in m) = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            box.width = stoi(line);
        } else if (line.find("Container depth (in m) = ") != string::npos) {
            text = "Container depth (in m) = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            box.depth = stoi(line);
            break;
        }
        count ++;
    }*/
    box.depth = 400;
    box.width  =200;
    box.height = 100;

    return box;
}

Spheres readSpheres() {
    Spheres ball;
    /*while(getline(fin, line)) {
        if (line.find("Total number of bodies = ") != string::npos) {
            text = "Total number of bodies = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            ball.n = stoi(line);
        } else if (line.find("Body radius (in m) = ") != string::npos) {
            text = "Body radius (in m) = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            ball.radius = stof(line);
        } else if (line.find("Body mass (in Kg) = ") != string::npos) {
            text = "Body mass (in Kg) = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            ball.mass = stoi(line);
        } else if (line.find("Time step for running simulation (dt) (in secs) = ") != string::npos) {
            text = "Time step for running simulation (dt) (in secs) = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            ball.time = stof(line);
            break;
        }
        count ++;
    }*/
    ball.mass = 1;
    ball.n = 1000;
    ball.radius = 0.5;
    ball.time = 0.01;

    return ball;
}

void extractCurrentCoords(vector<Point> &A, int n) {
    string line;
    int i;
    Point temp;
    //getline(fin, line);
    for(i=0; i<n; i++) {
            fread(&temp.x, sizeof(double), 1, f1);
            fread(&temp.y, sizeof(double), 1, f1);
            fread(&temp.z, sizeof(double), 1, f1);
            temp.id = i;
            A[i] = temp;
    }
}

void printArray(vector<Point> &A, int n) {
    int i;
    for(i=0; i<n; i++) {
        cout<<"("<<A[i].id<<","<<A[i].x<<","<<A[i].y<<","<<A[i].z<<") ";
    }
    cout<<"\n";
}

// Contains the content that need to be drawn on the frame
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  );
    glColor3f(0.2, 0.5, 0.4);

    glPushMatrix();

    drawCube(box);
    glPopMatrix();

    drawSpheres();

    glPopMatrix();
    glutSwapBuffers();
    glFlush();
}

// Enables depth testing and sets background colors
void init(void)
{
    f1 = fopen(TRJ, "rb");
    box = readContainer();
    balls = readSpheres();
    // Assign space to store coordinates of the sphere
    Point temp;
    temp.id = temp.x = temp.y = temp.z = 0;
    Coords.assign(balls.n, temp) ;

    steps = 0;
    // Set the current clear color to sky blue and the current drawing color to white.
    //glClearColor(0.1, 0.39, 0.88, 1.0); // background color
    glClearColor(0,0,0,1);
    //glClearDepth(box.depth);  // Leave for now
    glColor3f(1.0, 1.0, 1.0); // default color for primitives

   /*glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
   */

}

// For animation
void timer(int v)
{
    // Redraw the screen after every DELTA TIME (CONVERTED TO MILLI SECONDS)
    glutTimerFunc(1000*DELTA, timer, v);
    glutPostRedisplay();
    // At every instant of time extract coordinates
    if(steps < TIMESTEPS)
    {
        extractCurrentCoords(Coords, balls.n);
    }
    steps++;
}

void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = tan( fovY / 360 * pi ) * zNear;
    fW = fH * aspect;

    glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

// This function is called whenever window is resized, fixes the projection
void reshape(GLint w, GLint h)
{
    glViewport(0, 0, w, h); // set the viewing area
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //perspectiveGL(60, 1, -500,0.1); // For 3D visibility
    // 2D Testing
    glOrtho(0, w, 0, h, 0, -2000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(w/2-2*box.width, h/2-2*box.height, -1000+2*box.depth);
    glScalef(4.0, 4.0, 4.0);
}

void drawCube(Container box)
{
    // This cube should have depth effect. Draw Lines
    vector<pair<int, int> > edges;

    edges.assign(12, make_pair(0,0));

    edges = {
        make_pair(0,1),
        make_pair(0,3),
        make_pair(2,3),
        make_pair(2,1),

        make_pair(0,4),
        make_pair(2,7),
        make_pair(6,3),
        make_pair(6,4),
        make_pair(6,7),
        make_pair(5,1),
        make_pair(5,4),
        make_pair(5,7)
    };

    vector<tuple<float, float, float> > vertices;

    vertices.assign(8, make_tuple(0,0,0));

    vertices= {
        make_tuple(box.width, 0, 0),
        make_tuple(box.width, box.height, 0),
        make_tuple(0, box.height, 0),
        make_tuple(0,0, 0),
        make_tuple(box.width, 0, box.depth),
        make_tuple(box.width, box.height, box.depth),
        make_tuple(0,0, box.depth),
        make_tuple(0, box.height, box.depth),
    };

    int i, j;

   // glMatrixMode(GL_MODELVIEW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // this tells it to only render lines

    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glLineWidth(2);
    for(i=0; i<12; i++)
    {
        j = edges[i].first;
        glVertex3d(get<0>(vertices[j]),get<1>(vertices[j]),get<2>(vertices[j])) ;
        j = edges[i].second;
        glVertex3d(get<0>(vertices[j]),get<1>(vertices[j]),get<2>(vertices[j])) ;
    }

    glEnd();
}


void drawSpheres(void)
{
    // draw n number of spheres at coordinates specified in the array
    int i, n;
    n = balls.n;
    for(i=0; i<n; i++)
    {
        glPushMatrix();
        glColor3f(1.0,1.0,0.0);
        glTranslated(Coords[i].x, Coords[i].y, Coords[i].z );
        glutSolidSphere(balls.radius, 30, 30 );
        glPopMatrix();
    }

}
