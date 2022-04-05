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
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>


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
    
    
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Dark Blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
                                glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
                                glm::vec3(0,0,0), // and looks at the origin
                                glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                           );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model      = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

    
    do{
        
        // Clear the screen and use the shader program
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);
        
//        Dislay Hollow Cube
        drawCube(box);
        
//        Display Spheres
//        drawSpheres();
        
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
        
        centerx+ hLength, centery + hWidth, centerz + hDepth, // top right
        centerx + hLength, centery - hWidth, centerz + hDepth, // bottom right
        
        centerx + hLength, centery - hWidth, centerz + hDepth, // bottom right
        centerx - hLength, centery - hWidth, centerz + hDepth, // bottom left
        
        centerx - hLength, centery - hWidth, centerz + hDepth, // bottom left
        centerx - hLength, centery + hWidth, centerz + hDepth, // top left
        
        
        // back face
        centerx - hLength, centery + hWidth, centerz - hDepth, // top left
        centerx + hLength, centery + hWidth, centerz - hDepth, // top right
        
        centerx + hLength, centery + hWidth, centerz - hDepth, // top right
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right

        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        centerx - hLength, centery - hWidth, centerz - hDepth, // bottom left
        
        centerx - hLength, centery - hWidth, centerz - hDepth, // bottom left
        centerx - hLength, centery + hWidth, centerz - hDepth, // top left

        
        
        // left face
        centerx - hLength, centery + hWidth, centerz+ hDepth, // top left
        centerx - hLength, centery + hWidth, centerz - hDepth, // top right
        
        centerx - hLength, centery + hWidth, centerz - hDepth, // top right
        centerx - hLength, centery - hWidth, centerz- hDepth, // bottom right
        
        
        centerx - hLength, centery - hWidth, centerz- hDepth, // bottom right
        centerx - hLength, centery - hWidth, centerz + hDepth, // bottom left
        
        centerx - hLength, centery - hWidth, centerz + hDepth, // bottom left
        centerx - hLength, centery + hWidth, centerz+ hDepth, // top left

        
        // right face
        centerx + hLength, centery + hWidth, centerz + hDepth, // top left
        centerx + hLength, centery + hWidth, centerz - hDepth, // top right
        
        centerx + hLength, centery + hWidth, centerz - hDepth, // top right
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        centerx + hLength, centery - hWidth, centerz + hDepth, // bottom left
        
        centerx + hLength, centery - hWidth, centerz + hDepth, // bottom left
        centerx + hLength, centery + hWidth, centerz + hDepth, // top left

        
        // top face
        centerx - hLength, centery + hWidth, centerz + hDepth, // top left
        centerx - hLength, centery + hWidth, centerz - hDepth, // top right
        
        centerx - hLength, centery + hWidth, centerz - hDepth, // top right
        centerx + hLength, centery + hWidth, centerz - hDepth, // bottom right
        
        centerx + hLength, centery + hWidth, centerz - hDepth, // bottom right
        centerx + hLength, centery + hWidth, centerz + hDepth, // bottom left
        
        centerx + hLength, centery + hWidth, centerz + hDepth, // bottom left
        centerx - hLength, centery + hWidth, centerz + hDepth, // top left

        
        // top face
        centerx - hLength, centery- hWidth, centerz+ hDepth, // top left
        centerx - hLength, centery - hWidth, centerz - hDepth, // top right
        
        centerx - hLength, centery - hWidth, centerz - hDepth, // top right
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        
        centerx + hLength, centery - hWidth, centerz - hDepth, // bottom right
        centerx + hLength, centery - hWidth, centerz + hDepth,  // bottom left
        
        centerx + hLength, centery - hWidth, centerz + hDepth,  // bottom left
        centerx - hLength, centery- hWidth, centerz+ hDepth, // top left
        
    };
    
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    
    // Draw the lines
    glDrawArrays(GL_LINES, 0, 24);
        
    glDisableVertexAttribArray(0);
    
    glutWireCube()
}

void drawSpheres() {
    glColor3f(1,0,0);
    GLUquadric *quad;
//    quad = gluNewQuadric();
    glTranslatef(2,2,2);
//    gluSphere(quad,25,100,20);
}

