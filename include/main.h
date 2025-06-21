//
// Created by nbigi0 on 06/06/2025.
//
#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifndef MAIN_H
#define MAIN_H

#define PI 3.14159265358979323846
#define RAD(deg) ((deg)*PI/180)
#define rand_range(min, max) (rand()%(int)(max-min)+(min))
#define randf() (2*(rand() - rand())/(float)RAND_MAX)
#define u_randf() (rand()/(float)RAND_MAX)
#define pythag(a, b) sqrt(a*a + b*b)
#define max(a, b) ((a>b)?a:b)
#define mag(vec) (sqrtf(vec[0]*vec[0]+vec[1]*vec[1]))
#define _abs(a) ((a>0)?a:-a)
#define sign(a) (_abs(a)/a)

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
float Q_rsqrt(float number);

#endif //MAIN_H
