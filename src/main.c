#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <main.h>
#include <shader.h>
#include <time.h>
#include <cglm/cglm.h>

#define SPEED_LIMIT .5f
#define PARTICLES 500
#define GRAV_DISTANCE 3.f
#define MASS_C 1000.0f
#define G (6.6743*exp(-11))

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

float absf(float x)
{
    return (x > 0) ? x : -x;
}

int main(void)
{
    const FileData vertexShaderSource = read_file("../shaders/vertexShader.vert");
    const FileData fragmentShaderSource = read_file("../shaders/fragmentShader.frag");

    srand(ceil(time(NULL)));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int* w_dim = (ivec2){1000, 1000};

    GLFWwindow *window = glfwCreateWindow(w_dim[0], w_dim[1], "Gravitation Simulation", NULL, NULL);
    // GLFWwindow *window = glfwCreateWindow(400, 400, "Hello World", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    struct ShaderInfo* shader = malloc(sizeof(struct ShaderInfo));
    initShader(shader, vertexShaderSource, fragmentShaderSource);

    glEnable(GL_PROGRAM_POINT_SIZE);


    GLuint indices[PARTICLES];
    GLfloat masses[PARTICLES];
    for (unsigned int i = 0; i < PARTICLES; i++)
    {
        indices[i] = i;

        srand(rand());
        masses[i] = (rand_range((int)(MASS_C/5), (int)MASS_C))/MASS_C;
    }

    const size_t vertexCount = 3*PARTICLES;

    GLfloat vertices[vertexCount], velocities[vertexCount], accellerations[vertexCount];

    for (int i = 0; i < vertexCount; i++)
    {
        srand(rand());

        vertices[i] = .1f * randf();

        velocities[i] = SPEED_LIMIT * randf();
        accellerations[i] = 0.0f;
    }

    GLuint VBO, VBO2, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(masses), masses, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_TRUE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    GLdouble dt = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        const GLdouble prev_dt = glfwGetTime();

        GLfloat prev_part[3];
        for (size_t i = 3; i < vertexCount; i += 3)
        {
            for (size_t j = 0; j < 3; j++)
            {
                if (i == 3)
                {
                    for (size_t k = 0; k < 3; k++)
                    {
                        prev_part[k] = vertices[k];
                    }
                }
                prev_part[j] = vertices[i + j];
            }
            if (sqrt(pow(vertices[i]-prev_part[0], 2) + pow(vertices[i+1] - prev_part[1], 2)) < GRAV_DISTANCE)
            {
                if (sqrt(
                    pow(vertices[i]-prev_part[0], 2) + pow(vertices[i+1] - prev_part[1], 2)
                    ) < 0.05)
                {
                    // accellerations[i] = (absf(vertices[i])/vertices[i]) * 1/pow(MASS_C, 1.5);
                    // accellerations[i+1] = (absf(vertices[i+1])/vertices[i+1]) * 1/pow(MASS_C, 1.5);
                }
                accellerations[i] = -(
                        absf(vertices[i])/vertices[i]) *
                        absf(G*pow(MASS_C, 2) *
                        masses[i/3]*masses[(i-3)/3] /
                        pow(absf(vertices[i]) - absf(vertices[i-3]),2)
                        );
                accellerations[i+1] = -(
                        absf(vertices[i+1])/vertices[i+1]) *
                        absf(G*pow(MASS_C, 2) *
                        masses[i/3]*masses[(i-3)/3] /
                        pow(absf(vertices[i+1]) - absf(vertices[i-3+1]),2)
                        );
            }
        }

        for (size_t i = 0; i < vertexCount; i++)
        {
            const float absf_vertex = absf(vertices[i]);
            if (absf_vertex > 0.9f)
            {
                accellerations[i] = -(absf_vertex/vertices[i]) * absf(1.0f/pow(1-absf_vertex,2));
            }
            velocities[i] += accellerations[i] * dt;

            (absf(accellerations[i]) > 0) ? accellerations[i] -= 0.05f*(absf(accellerations[i])/accellerations[i]) : (void)0;

            (velocities[i] > SPEED_LIMIT) ? velocities[i] = SPEED_LIMIT : (void)0;

            // (absf(vertices[i]) > 1) ? velocities[i] *= -1 : (void)0;

            vertices[i] += velocities[i] * dt;

            // for (unsigned int j = 0; j < 3; j++)
            // {
            //     unsigned int index = i/3 + j;
            //     vertices[index] += velocities[index] * dt;
            //
            //     if (vertices[index] > 1.0f || vertices[index] < -1.0f)
            //     {
            //         velocities[index] *= -1;
            //     }
            // }
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        // glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(masses), masses, GL_DYNAMIC_DRAW);

        use(shader->ID);
        glBindVertexArray(VAO);
        glDrawElements(GL_POINTS, sizeof(indices)/sizeof(GLuint), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        dt = glfwGetTime() - prev_dt;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader->ID);

    free(shader);

    glfwTerminate();
    return 0;
}