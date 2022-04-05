// Libraries include
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<vector>
#include<iostream>
// Include GLEW, GLFW and GLM
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

using namespace glm;
using namespace std;

#define TRJ "miniTrajectory.txt"

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

Container readContainer(ifstream &fin);
Spheres readSpheres(ifstream &fin);
void extractCurrentCoords(vector<Point> &A, int n, ifstream &fin) ;
void printArray(vector<Point> &A, int n) ;

void drawCube(Container box);
void drawSpheres();

int main() {
    
    ifstream fin;
    fin.open(TRJ);
    Container box;
    Spheres balls;
    // Extract the required Coordinates from the files.
    box = readContainer(fin);
    balls = readSpheres(fin);
    Point temp;
    temp.id = temp.x = temp.y = temp.z = 0;
    Coords.assign(balls.n, temp) ;
    extractCurrentCoords(Coords, balls.n, fin);
    
    // Initialize GLFW
    glewExperimental = true;
    if(!glfwInit()) {
        fprintf(stderr, "Initialization of GLFW failed!!");
        return -1;
    }
    
    // 4x antialiasing,OpenGL 3.3
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window;
    window = glfwCreateWindow( 1024, 1024, "HP3 Assignment 2", NULL, NULL);
    glEnable(GL_DEPTH_TEST);
    
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    do{
//        Dislay Hollow Cube
//        drawCube(box);
        
//        Display Spheres
        drawSpheres();
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );
           
      
    glfwTerminate();
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

void drawCube(Container box) {

    GLfloat hLength = box.length * 0.5f;
    GLfloat centerx, centery, centerz;
    GLfloat hWidth = box.width * 0.5f;
    GLfloat hDepth = box.depth * 0.5f;
    centerx = centery = centerz = 0;
        
    GLfloat vertices[] =
    {
        // front face
        centerx - hLength, centery + hWidth, centerz + hDepth, // top left
        centerx+ hLength, centery + hWidth, centerz + hDepth, // top right
        centerx + hLength, centery - hWidth, centerz + hDepth, // bottom right
        centerx - hLength, centery - hWidth, centerz + hDepth, // bottom left
        
        // back face
        centerx - hLength, centery + hWidth, centerz - hDepth, // top left
        centerx + hLength, centery + hWidth, centerz - hDepth, // top right
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        centerx - hLength, centery - hWidth, centerz - hDepth, // bottom left
        
        // left face
        centerx - hLength, centery + hWidth, centerz+ hDepth, // top left
        centerx - hLength, centery + hWidth, centerz - hDepth, // top right
        centerx - hLength, centery - hWidth, centerz- hDepth, // bottom right
        centerx - hLength, centery - hWidth, centerz + hDepth, // bottom left
        
        // right face
        centerx + hLength, centery + hWidth, centerz + hDepth, // top left
        centerx + hLength, centery + hWidth, centerz - hDepth, // top right
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        centerx + hLength, centery - hWidth, centerz + hDepth, // bottom left
        
        // top face
        centerx - hLength, centery + hWidth, centerz + hDepth, // top left
        centerx - hLength, centery + hWidth, centerz - hDepth, // top right
        centerx + hLength, centery + hWidth, centerz - hDepth, // bottom right
        centerx + hLength, centery + hWidth, centerz + hDepth, // bottom left
        
        // top face
        centerx - hLength, centery- hWidth, centerz+ hDepth, // top left
        centerx - hLength, centery - hWidth, centerz - hDepth, // top right
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        centerx + hLength, centery - hWidth, centerz + hDepth  // bottom left
    };
    
    
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f( 1, 1, 1 );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );

    glDrawArrays( GL_LINES, 0, 24 );
    
    glDisableClientState( GL_VERTEX_ARRAY );
    
}

void drawSpheres() {
    glColor3f(1,0,0);
    GLUquadric *quad;
//    quad = gluNewQuadric();
    glTranslatef(2,2,2);
//    gluSphere(quad,25,100,20);
}

