#include <stdio.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../lib/utils.h"

//eixos da camera
float camera_x = 0.5, camera_y = 1, camera_z = 2;

/* Globals */
/** Window width. */
int win_width  = 600;
/** Window height. */
int win_height = 600;

/** Program variable. */
int program;
/** Vertex array object. */
unsigned int VC1;
/** Vertex buffer object. */
unsigned int VC2;
/** Vertex buffer object. */
unsigned int VD1;
/** Vertex buffer object. */
unsigned int VD2;

/** diamante x angle */
float px_angle = 0.0f;
/** diamante x angle increment */
float px_inc = 0.01f;
/** diamante y angle */
float py_angle = 0.0f;
/** diamante y angle increment */
float py_inc = 0.2f;

//variáveis da translação do diamante
double sobe = -1.2, altura = -0.8;
float direction = 1;



/** Vertex shader. */
const char *vertex_code = "\n"
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 normal;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"out vec3 vNormal;\n"
"out vec3 fragPosition;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(position, 1.0);\n"
"    vNormal = normal;\n"
"    fragPosition = vec3(model * vec4(position, 1.0));\n"
"}\0";

/** Fragment shader. */
const char *fragment_code = "\n"
"#version 330 core\n"
"\n"
"in vec3 vNormal;\n"
"in vec3 fragPosition;\n"
"\n"
"out vec4 fragColor;\n"
"\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 lightPosition;\n"
"uniform vec3 cameraPosition;\n"
"void main()\n"
"{\n"
"    float ka = 0.5;\n"
"    vec3 ambient = ka * lightColor;\n"
"\n"
"    float kd = 0.8;\n"
"    vec3 n = normalize(vNormal);\n"
"    vec3 l = normalize(lightPosition - fragPosition);\n"
"\n"
"    float diff = max(dot(n,l), 0.0);\n"
"    vec3 diffuse = kd * diff * lightColor;\n"
"\n"
"    float ks = 1.0;\n"
"    vec3 v = normalize(cameraPosition - fragPosition);\n"
"    vec3 r = reflect(-l, n);\n"
"\n"
"    float spec = pow(max(dot(v, r), 0.0), 3.0);\n"
"    vec3 specular = ks * spec * lightColor;\n"
"\n"
"    vec3 light = (ambient + diffuse + specular) * objectColor;\n"
"    fragColor = vec4(light, 1.0);\n"
"\n"
"}\0";

/* Functions. */
void display(void);
void reshape(int, int);
void idle(void);
void initData(void);
void initShaders(void);
void special(int key, int, int);//função para capturar as teclas


/**
 * Drawing function.
 *
 * Draws primitive.
 */
void display()
{

    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
         


    //camera   
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,1.0f,0.0f));
    view = glm::lookAt(glm::vec3(camera_x,camera_y,camera_z),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
    unsigned int loc = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

    //projeção
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (win_width/(float)win_height), 0.1f, 100.0f);
    loc = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

    //Movimentos do diamante
    glBindVertexArray(VD1);

    glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(py_angle), glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 T  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,sobe,-3.0f));

    //Definição dos limites de movimentação vertical do diamante
    if(sobe>=altura)direction=-1;
    if(sobe<=-1.2)direction=1;
    if(direction==-1)sobe-=0.005;
    else sobe+=0.005;


    glm::mat4 model =  T * Ry;

    loc = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 24);


    //Movimentação do Cubo
    glBindVertexArray(VC1);


    // Define model matrix.
    glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(1.0f,0.0f,0.0f));
    glm::mat4 S  = glm::scale(glm::mat4(1.0f), glm::vec3(0.7f,0.7f,0.7f));
    T  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-1.65f,-3.0f));
    model = T*S*Rx;



    // Retrieve location of tranform variable in shader.
    loc = glGetUniformLocation(program, "model");
    // Send matrix to shader.
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 36);
   

    //Definição das cores do diamante e do cubo
    loc = glGetUniformLocation(program, "objectColor");
    glUniform3f(loc, 0.4, 0.88, 0.87);

    // Light color.
    loc = glGetUniformLocation(program, "lightColor");
    glUniform3f(loc, 1.0, 1.0, 1.0);

    // Light position.
    loc = glGetUniformLocation(program, "lightPosition");
    glUniform3f(loc, 6.0, 19.2, 6.0);
    glutSwapBuffers();
       

}

/**
 * Reshape function.
 *
 * Called when window is resized.
 *
 * @param width New window width.
 * @param height New window height.
 */
void reshape(int width, int height)
{
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
    glutPostRedisplay();
}

//função para incrementar o angulo da rotacao e translacao do diamante
void idle()
{
    px_angle = ((px_angle+px_inc) < 360.0f) ? px_angle+px_inc : 360.0-px_angle+px_inc;
    py_angle = ((py_angle+py_inc) < 360.0f) ? py_angle+py_inc : 360.0-py_angle+py_inc;
    glutPostRedisplay();
}


/**
 * Init vertex data.
 *
 * Defines the coordinates for vertices, creates the arrays for OpenGL.
 */
void initData()
{
    // Set diamante vertices.
    float vertices[] = {
        // First triangle
        // coordinate     color
         0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.7f, 0.5f, 1.0f, 0.0f, 0.0f,
         0.5f, 0.7f, 0.5f, 1.0f, 0.0f, 0.0f,
        // Second triangle
        // coordinate     color
         0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, 0.7f, 0.5f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.7f,-0.5f, 1.0f, 0.0f, 0.0f,

         0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, 0.7f,-0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.7f,-0.5f, 1.0f, 0.0f, 0.0f,

         0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.7f,-0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.7f, 0.5f, 1.0f, 0.0f, 0.0f,
//parte de baixo
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.7f, -0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.7f, -0.5f, 1.0f, 0.0f, 0.0f,
       
         0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         -0.5f, 0.7f, -0.5f, 0.0f, 1.0f, 0.0f,
         -0.5f, 0.7f,0.5f, 1.0f, 0.0f, 0.0f,

         0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         -0.5f, 0.7f,0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.7f,0.5f, 1.0f, 0.0f, 0.0f,

         0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.7f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.7f, -0.5f, 1.0f, 0.0f, 0.0f


    };
   
    // Vertex array.
    glGenVertexArrays(1, &VD1);
    glBindVertexArray(VD1);


    // Vertex buffer
    glGenBuffers(1, &VD2);
    glBindBuffer(GL_ARRAY_BUFFER, VD2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   
    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);


float vertices2 [] = {

        //CUBO
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f
};


    // Vertex array.
    glGenVertexArrays(1, &VC1);
    glBindVertexArray(VC1);


    // Vertex buffer
    glGenBuffers(1, &VC2);
    glBindBuffer(GL_ARRAY_BUFFER, VC2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

   
    // Set attributes.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Array Object.
    glBindVertexArray(0);
   
    glEnable(GL_DEPTH_TEST);
}


/** Create program (shaders).
 *
 * Compile shaders and create the program.
 */
void initShaders()
{
    // Request a program and shader slots from GPU
    program = createShaderProgram(vertex_code, fragment_code);
}

//pega as teclas pressionadas e faz a movimentação da camera
void special(int key, int, int) {
  switch (key) {
    case GLUT_KEY_LEFT: {if(camera_x==0.5)camera_x=-1; break;}
    case GLUT_KEY_RIGHT: {if(camera_x==-1)camera_x=0.5;}
    case GLUT_KEY_UP: {if(camera_y==10)camera_y=1; break;}
    case GLUT_KEY_DOWN:{if(camera_y==1)camera_y=10; break;}
  }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
glutInit(&argc, argv);
glutInitContextVersion(3, 3);
glutInitContextProfile(GLUT_CORE_PROFILE);
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
glutInitWindowSize(win_width,win_height);
glutCreateWindow(argv[0]);
glewExperimental = GL_TRUE;
    glutSpecialFunc(special);
glewInit();

    // Init vertex data for the triangle.
    initData();
   
    // Create shaders.
    initShaders();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

glutMainLoop();
}
