// Libraries include
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<vector>
#include<iostream>
#include<stdbool.h>

// OpenGL graphics libraries.
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include<GL/gl.h>
#include<GL/glu.h>

using namespace std;

#define TRJ "Trajectory.txt"
#define TIMESTEPS 1

//Input and output functions
typedef struct  {
    int id;
    float x, y, z;
} Point;

typedef struct{
    int n;
    int mass;
    float radius;
    float time;
} Spheres;

typedef struct{
    int length, width, depth;
} Container;

vector<Point> Coords;
Container box;
Spheres balls;
bool readBox = true;
ifstream fin;
int steps = 0;

Container readContainer(ifstream &fin);
Spheres readSpheres(ifstream &fin);
void extractCurrentCoords(vector<Point> &A, int n, ifstream &fin) ;
void printArray(vector<Point> &A, int n) ;

// Graphics Functions
void display(void);
void drawCube(Container box);
void drawSpheres(void);
void init(void);
void timer(int);
void reshape(GLint, GLint);

int main(int argc, char** argv)
{
    // initialize and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("HP3 OpenMP TermProject");

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutReshapeFunc(reshape);

    init();

    glutMainLoop();
    return 0;
}

Container readContainer(ifstream &fin){
    string line, text;
    int count, size;
    count = 0;
    Container box;
    while(getline(fin, line)) {
        if (line.find("Container length (in m) = ") != string::npos) {
            text = "Container length (in m) = ";
            size = text.size();
            line.erase(line.begin()+0, line.begin()+size);
            box.length = stoi(line);
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
    }

    return box;
}

Spheres readSpheres(ifstream &fin) {
    string line, text;
    int count, size;
    count = 0;
    Spheres ball;
    while(getline(fin, line)) {
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
    }

    return ball;
}

void extractCurrentCoords(vector<Point> &A, int n, ifstream &fin) {
    string line;
    int i;
    Point temp;
    getline(fin, line);
    for(i=0; i<n; i++) {
       fin>>temp.x>>temp.y>>temp.z;
       temp.id = i;
       A[i] = temp;
    }
    getline(fin, line);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
     glPushMatrix();
    glLoadIdentity();

    //drawCube(box);

    cout<<"#:"<<Coords[0].x<<"\n";

    drawSpheres();

    glPopMatrix();
    glutSwapBuffers();
}

// Enables depth testing and sets background colors
void init(void)
{
    glEnable(GL_DEPTH_TEST);
    //glClearColor(1.0, 1.0, 0.0, 0.0);
    fin.open(TRJ);
    box = readContainer(fin);
    balls = readSpheres(fin);
    // Assign space to store coordinates of the sphere
    Point temp;
    temp.id = temp.x = temp.y = temp.z = 0;
    Coords.assign(balls.n, temp) ;
    steps = 0;
}

// For animation
void timer(int v)
{

    // ? Change the frame persecond
    glutTimerFunc(1000/60, timer, v);
    glutPostRedisplay();
    // At every instant of time extract coordinates
    if(steps < TIMESTEPS)
    {
        cout<<"$$: ";
        extractCurrentCoords(Coords, balls.n, fin);
        cout<< Coords[0].x  <<"\n";
    }
    steps++;

}

// This function is called whenever window is resized, fixes the projection
void reshape(GLint w, GLint h)
{
    glViewport(0, 0, w, h); // set the viewing area
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glhPerspectivef2(45, GLfloat(w)/GLfloat(h), 0, 500);
    glOrtho(0,500,0,500,0,500);
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(Container box)
{
    // This cube should have depth effect
}


void drawSpheres(void)
{
    // draw n number of spheres at coordinates specified in the array
    int i, n;


    cout<<balls.radius<<":"<<balls.n<<"\n";
    n = balls.n;
    for(i=0; i<n; i++)
    {
        glPushMatrix();
        glColor3f(1.0,1.0,0.0);
        glTranslatef( Coords[i].x, Coords[i].y, Coords[i].z );
        glutSolidSphere(balls.radius, 30, 30 );
        glPopMatrix();
    }


}
