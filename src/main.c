#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <main.h>
#include <shader.h>
#include <time.h>
#include <cglm/cglm.h>

// CAN DISABLE //
#define SPEED_LIMIT 1.0f
#define BARRIER 0.8f
//

#define SOFTENING 5e-4f
#define PARTICLES 20
#define MASS_C (3e8f/pow(PARTICLES, pow(1-SOFTENING, 50)))
#define G 6.6743e-11f

typedef struct
{
    vec3 pos;
    vec3 vel;
    float m;
} Particle;

void calcForces(Particle *particles, const double dt)
{
#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < PARTICLES; i++)
    {
        float Fx = 0.0f; float Fy = 0.0f;

        for (int j = 0; j < PARTICLES; j++)
        {
            float dx = particles[j].pos[0] - particles[i].pos[0];
            float dy = particles[j].pos[1] - particles[i].pos[1];

            if (dx == 0.0f && dy == 0.0f)
            {
                continue;
            }
            // if (pythag(dx, dy) < 0.4*(particles[i].m + particles[j].m)/30)
            // {
            //     continue;
            // }
            float dist2 = dx*dx + dy*dy + SOFTENING;
            float rdist = Q_rsqrt(dist2);
            float rdist3 = rdist*rdist*rdist;
            float force = G * pow(particles[j].m, 1 + SOFTENING) * rdist3;

            Fx += force * dx;
            Fy += force * dy;
        }

        // printf("%f Fx, %f Fy\n", Fx, Fy);

        particles[i].vel[0] += Fx*dt;
        particles[i].vel[1] += Fy*dt;

    }
}

void applyForce(Particle *p, const vec3 F, const double dt)
{
    for (int i = 0; i < 3; i++)
    {
        p->vel[i] += F[i]/p->m * dt;
    }
}

void updateVertexArray(GLfloat *vertices, const Particle *particles)
{
    for (int i = 0; i < PARTICLES; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vertices[i*3+j] = particles[i].pos[j];
        }
    }
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

    Particle particles[PARTICLES];
    GLuint indices[PARTICLES];
    const size_t vertexCount = 3*PARTICLES;
    GLfloat vertices[vertexCount], masses[PARTICLES];
    for (unsigned int i = 0; i < PARTICLES; i++)
    {
        indices[i] = i;
        for (int j = 0; j < 3; j++)
        {
            particles[i].pos[j] = 0.4f*randf();
            particles[i].vel[j] = 0.1f*randf();
        }
        masses[i] = u_randf();
        particles[i].m = masses[i]*MASS_C;
        printf("Particle %d Mass: %f\n", i+1, particles[i].m);
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

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    GLdouble dt = 0.01f;
    while (!glfwWindowShouldClose(window))
    {
        const GLdouble t = glfwGetTime();
        calcForces(particles, dt);

        for (int i = 0; i < PARTICLES; i++)
        {
            // printf("%d: (%f, %f), %fx + %fy\n", i, particles[i].pos[0], particles[i].pos[1], particles[i].vel[0], particles[i].vel[1]);

#ifdef BARRIER
            if (_abs(particles[i].pos[0]) > BARRIER)
            {
                const vec3 F = {
                    particles[i].m/3 * 1/_abs(1-_abs(particles[i].pos[0])) * -sign(particles[i].pos[0]),
                    0,
                    0
                };
                applyForce(&particles[i], F, dt);
            }
            if (_abs(particles[i].pos[1]) > BARRIER)
            {
                const vec3 F = {
                    0,
                    particles[i].m/3 * 1/_abs(1-_abs(particles[i].pos[1])) * -sign(particles[i].pos[1]),
                    0
                };
                applyForce(&particles[i], F, dt);
            }
#endif
#ifdef SPEED_LIMIT
            if (_abs(particles[i].vel[0]) > SPEED_LIMIT)
            {
                const vec3 F = {
                    particles[i].m/3 * 1/_abs(1-_abs(particles[i].pos[0])) * -sign(particles[i].pos[0]),
                    0,
                    0
                };
                applyForce(&particles[i], F, dt);
            }
            if (_abs(particles[i].vel[1]) > SPEED_LIMIT)
            {
                const vec3 F = {
                    0,
                    particles[i].m/3 * 1/_abs(1-_abs(particles[i].pos[1])) * -sign(particles[i].pos[1]),
                    0
                };
                applyForce(&particles[i], F, dt);
            }
#endif
            for (int j = 0; j < 3; j++)
            {
                particles[i].pos[j] += particles[i].vel[j]*dt;
            }
        }

        updateVertexArray(vertices, particles);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        use(shader->ID);
        glBindVertexArray(VAO);
        glDrawElements(GL_POINTS, PARTICLES, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        dt = glfwGetTime() - t;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

float Q_rsqrt( float number )
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}