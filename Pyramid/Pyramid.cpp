// Pyramid.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//@author Shawn Frye

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//***********************GLOBAL VARIABLES*****************************

//init the textures
GLuint textureID;
GLuint globeTextureID;

bool isOrtho = false; //to change between orthographic and perspective views
float mouse_sensitivity = 0.2f;
float cylinder_height = 3.0f;
// Camera position and movement variables
float camera_x = 2.3f;
float camera_y = 3.2f;
float camera_z = -10.5f;
float camera_speed = 0.1f;
float camera_pitch = -5.2f;
float camera_yaw = 93.999f;
int last_mouse_x, last_mouse_y;
bool mouse_dragging = false;


//setup lights
void setupLights() {
    // Enable lighting
    glEnable(GL_LIGHTING);

    // White Key light
    GLfloat keyLightPosition[] = { 2.0f, 2.0f, 2.0f, 1.0f };
    GLfloat keyLightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat keyLightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat keyLightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, keyLightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, keyLightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, keyLightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, keyLightSpecular);

    glEnable(GL_LIGHT0);

    // White Fill light
    GLfloat fillLightPosition[] = { -2.0f, -2.0f, 2.0f, 1.0f };
    GLfloat fillLightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat fillLightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat fillLightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, fillLightPosition);
    glLightfv(GL_LIGHT1, GL_AMBIENT, fillLightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, fillLightDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, fillLightSpecular);

    glEnable(GL_LIGHT1);
}

//load texture from images
GLuint loadTexture(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "Error: Unable to load texture image." << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (nrChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void drawGlobe(float radius, int num_lat_segments, int num_lon_segments) {
    globeTextureID = loadTexture("globe2.jpg");
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, globeTextureID);

    for (int lat_segment = 0; lat_segment < num_lat_segments; ++lat_segment) {
        float theta1 = lat_segment * 3.14159f / num_lat_segments;
        float theta2 = (lat_segment + 1) * 3.14159f / num_lat_segments;

        glBegin(GL_QUAD_STRIP);
        for (int lon_segment = 0; lon_segment <= num_lon_segments; ++lon_segment) {
            float phi = lon_segment * 2 * 3.14159f / num_lon_segments;

            float x = radius * sin(theta1) * cos(phi);
            float y = radius * cos(theta1);
            float z = radius * sin(theta1) * sin(phi);
            float u = phi / (2 * 3.14159f);
            float v = theta1 / 3.14159f;

            glTexCoord2f(u, v);
            glVertex3f(x, y, z);
            x = radius * sin(theta2) * cos(phi);
            y = radius * cos(theta2);
            z = radius * sin(theta2) * sin(phi);
            v = theta2 / 3.14159f;
            glTexCoord2f(u, v);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}


//draw pencil
void drawPencil(int num_segments, float cylinder_radius, float pyramid_base_radius, float pyramid_height) {

    textureID = loadTexture("wood2.jpg");
    //rotate pencil
    float rotation_angle = -45.0f;
    glRotatef(rotation_angle, 0.0f, 1.0f, 0.0f);

    //tilt pencil
    glRotatef(30.0f, 1.0f, .0f, 0.0f);

    GLfloat mat_ambient_cylinder[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    GLfloat mat_diffuse_cylinder[] = { 1.0f, 1.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_cylinder);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_cylinder);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Draw the cylinder
    float angle_step = 2 * 3.14159 / num_segments;
    float current_angle;
    for (int i = 0; i < num_segments; ++i) {
        current_angle = i * angle_step;
        float next_angle = (i + 1) * angle_step;
        glBegin(GL_QUADS);
        glTexCoord2f(current_angle / (2 * 3.14159), 0);
        glVertex3f(cylinder_radius * cos(current_angle), 0, cylinder_radius * sin(current_angle));
        glTexCoord2f(current_angle / (2 * 3.14159), 1);
        glVertex3f(cylinder_radius * cos(current_angle), cylinder_height, cylinder_radius * sin(current_angle));
        glTexCoord2f(next_angle / (2 * 3.14159), 1);
        glVertex3f(cylinder_radius * cos(next_angle), cylinder_height, cylinder_radius * sin(next_angle));
        glTexCoord2f(next_angle / (2 * 3.14159), 0);
        glVertex3f(cylinder_radius * cos(next_angle), 0, cylinder_radius * sin(next_angle));

        glEnd();
    }

    GLfloat mat_ambient_pyramid[] = { 0.6f, 0.3f, 0.0f, 1.0f };
    GLfloat mat_diffuse_pyramid[] = { 0.6f, 0.3f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_pyramid);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_pyramid);
    // Draw the pyramid

    glTranslatef(0.0f, cylinder_height, 0.0f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < num_segments; ++i) {
        current_angle = i * angle_step;
        float next_angle = (i + 1) * angle_step;
        glColor3f(0.6f, 0.3f, 0.0f);
        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(0.0f, pyramid_height, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(pyramid_base_radius * cos(current_angle), 0.0f, pyramid_base_radius * sin(current_angle));
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(pyramid_base_radius * cos(next_angle), 0.0f, pyramid_base_radius * sin(next_angle));

    }
    //glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnd();

}

void drawPencilHolder(float cylinder_radius, float cylinder_height, int num_segments) {

    float angle_step = 2 * 3.14159 / num_segments;
    float current_angle;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //move cylinder position
    glTranslatef(-0.3f, 0.0f, 0.75f);
    glDisable(GL_LIGHTING);

    for (int i = 0; i < num_segments; ++i) {
        current_angle = i * angle_step;
        float next_angle = (i + 1) * angle_step;
        glBegin(GL_QUAD_STRIP);
        glColor4f(0.56f, 0.93f, 0.56f, 0.5f);



        glVertex3f(cylinder_radius * cos(current_angle), 0, cylinder_radius * sin(current_angle));
        glVertex3f(cylinder_radius * cos(current_angle), cylinder_height, cylinder_radius * sin(current_angle));
        glVertex3f(cylinder_radius * cos(next_angle), 0, cylinder_radius * sin(next_angle));
        glVertex3f(cylinder_radius * cos(next_angle), cylinder_height, cylinder_radius * sin(next_angle));
        glDisable(GL_BLEND);

        glEnd();
    }
    glEnable(GL_LIGHTING);
}

void drawBottomPostit(float size) {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(6.2f, -1.7f, -3.0f);
    glColor3f(0.5f, 0.0f, 0.5f);
    glRotatef(-15.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 0.18f, 1.0f);
    glutSolidCube(size);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawMiddlePostit(float size) {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(5.7f, -1.35f, -3.0f);
    glColor3f(0.0f, 1.0f, 0.2f);
    glRotatef(-2.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 0.18f, 1.0f);
    glutSolidCube(size);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawTopPostit(float size) {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(5.2f, -1.0f, -3.0f);
    glColor3f(1.0f, 0.4f, 0.8f);
    glRotatef(11.0f, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 0.18f, 1.0f);
    glutSolidCube(size);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void updateScene(int w, int h, float pencil_cylinder_radius, float cylinder_height, int pencil_num_segments, bool isPencilBody, float pyramid_base_radius, float pyramid_height, bool isOtho, float camera_pitch, float camera_yaw) {

    // Update window viewport
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (isOtho) {
        float aspectRatio = 800.0f / 600.0f;
        glOrtho(-10 * aspectRatio, 10 * aspectRatio, -10, 10, -100, 100);

    }
    else {
        gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    }
    glMatrixMode(GL_MODELVIEW);

    // Clear buffers and reset the modelview matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float cam_target_x = camera_x + cos(glm::radians(camera_pitch)) * cos(glm::radians(camera_yaw));
    float cam_target_y = camera_y + sin(glm::radians(camera_pitch));
    float cam_target_z = camera_z + cos(glm::radians(camera_pitch)) * sin(glm::radians(camera_yaw));

    if (isOtho) {
        glTranslatef(0.0f, 0.0f, -20.0f); // Move the camera away from the scene
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }
    else {
        gluLookAt(camera_x, camera_y, camera_z, cam_target_x, cam_target_y, cam_target_z, 0.0f, 1.0f, 0.0f);
    }

    setupLights();
    glEnable(GL_LIGHTING); // Enable lighting
    glEnable(GL_NORMALIZE);

    // Draw the plane
    glPushMatrix();
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex3f(-5.0f * 3, 0.0f, -5.0f * 3);
    glVertex3f(-5.0f * 3, 0.0f, 5.0f * 3);
    glVertex3f(5.0f * 3, 0.0f, 5.0f * 3);
    glVertex3f(5.0f * 3, 0.0f, -5.0f * 3);
    glEnd();
    glPopMatrix();

    //draw  the globe
    glPushMatrix();
    glTranslatef(3.5f, 2.0f, 2.0f);
    drawGlobe(2.0f, 30, 30);
    glPopMatrix();

    //DRWA POST-ITS

    //bottom post-it
    glPushMatrix();
    glTranslatef(-3.5f, 2.0f, 2.0f);
    drawBottomPostit(2.0f);
    glPopMatrix();

    //middle post-it
    glPushMatrix();
    glTranslatef(-3.5f, 2.0f, 2.0f);
    drawMiddlePostit(2.0f);
    glPopMatrix();

    //top post it
    glPushMatrix();
    glTranslatef(-3.5f, 2.0f, 2.0f);
    drawTopPostit(2.0f);
    glPopMatrix();

    //draw pencil
    glPushMatrix();
    drawPencil(pencil_num_segments, pencil_cylinder_radius, pyramid_base_radius, pyramid_height);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //draw pencil holder
    glPushMatrix();
    drawPencilHolder(1.0f, 2.0f, 30);
    glPopMatrix();

    //glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    // Swap buffers
    glutSwapBuffers();
}

// Setter function for isOrtho
void setIsOrtho(bool value) {
    isOrtho = value;
}

// Getter function for isOrtho
bool getIsOrtho() {
    return isOrtho;
}

void printCameraPosition() {
    std::cout << "Camera Position: X=" << camera_x << ", Y=" << camera_y << ", Z=" << camera_z << std::endl;
    std::cout << "Camera Pitch: " << camera_pitch << ", Camera Yaw: " << camera_yaw << std::endl;
}

// Getter for mouse sensitivity
float getMouseSensitivity() {
    return mouse_sensitivity;
}

// Setter for mouse sensitivity
void setMouseSensitivity(float new_sensitivity) {
    mouse_sensitivity = new_sensitivity;
}

// Keyboard function with perspective/orthographic toggle
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        camera_z += camera_speed;
        break;
    case 's':
        camera_z -= camera_speed;
        break;

    case 'v':
        printCameraPosition();
        break;
    case 'a':
        camera_x += camera_speed;
        break;
    case 'd':
        camera_x -= camera_speed;
        break;
    case 'q':
        camera_y += camera_speed;
        break;
    case 'e':
        camera_y -= camera_speed;
        break;
    case 'o':
        setIsOrtho(true);
        break;
    case 'p':
        setIsOrtho(false);
        break;
    case '+':
        setMouseSensitivity(getMouseSensitivity() + 0.05f); //increase sensitivity
        break;
    case '-':
        setMouseSensitivity(std::max(getMouseSensitivity() - 0.05f, 0.01f)); //decrease sensitivity
        break;
    default:
        break;
    }
    // update camera position
    updateScene(800, 600, 0.2f, cylinder_height, 30, true, 0.2f, 0.7f, getIsOrtho(), camera_pitch, camera_yaw);
}


void mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        mouse_dragging = true;
        last_mouse_x = x;
        last_mouse_y = y;
    }
    else if (state == GLUT_UP) {
        mouse_dragging = false;
    }

    if (button == 3) { // Mouse wheel up
        camera_speed += 0.01f;
    }
    else if (button == 4) { // Mouse wheel down
        camera_speed -= 0.01f;
        if (camera_speed < 0.01f) {
            camera_speed = 0.01f;
        }
    }
}

void motion(int x, int y) {
    if (mouse_dragging) {
        int dx = x - last_mouse_x;
        int dy = y - last_mouse_y;

        camera_yaw += dx * getMouseSensitivity();
        camera_pitch -= dy * getMouseSensitivity();


        camera_pitch = std::max(std::min(camera_pitch, 89.0f), -89.0f);

        last_mouse_x = x;
        last_mouse_y = y;
        updateScene(800, 600, 0.2f, cylinder_height, 30, true, 0.2f, 0.7f, getIsOrtho(), camera_pitch, camera_yaw);
    }

}

// Vertex shader source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = texCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// Fragment shader source
const char* fragmentShaderSource = R"(
#version 330 core

struct Material {
    sampler2D texture_diffuse;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    // Ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse, TexCoord));

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse, TexCoord));

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.texture_diffuse, TexCoord));

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

int main(int argc, char** argv) {
    // Initialize GLUT    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Scene");

    //Init
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    // Register callbacks    
    glutDisplayFunc([]() { updateScene(800, 600, 0.2f, 3.0f, 30, true, 0.2f, 0.7f, getIsOrtho(), camera_pitch, camera_yaw); });
    glutReshapeFunc([](int w, int h) { updateScene(w, h, 0.2f, 3.0f, 30, true, 0.2f, 0.7f, getIsOrtho(), camera_pitch, camera_yaw); });
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glEnable(GL_DEPTH_TEST);

    // Enter the main loop
    glutMainLoop();

    return 0;
}
