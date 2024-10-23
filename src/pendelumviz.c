#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>
#include <time.h>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <math.h>

struct params{
    double g;
    double length;
    double damping;
    double damping2;
    double omega;
    double startPosition;
    int startVelocity;
};

int func(double t, const double y[], double f[], void *params){
    //(void) (t);
    struct params *init  = (struct params*)params;
    // Starting velocity
    f[0] = y[1];
    // Free Fall
    f[1] = -(init->damping)*y[1] - (init->g / init->length)*sin(y[0]) - (init->damping2)*sin(init->omega)*t;
    return GSL_SUCCESS;
}

int main(){
    // Paramter initialization
    struct params *inputs = malloc(sizeof(struct params));

    if (inputs != NULL){
        inputs->g=9.8;
        inputs->length=2;
        inputs->damping=0.5;
        inputs->damping2=0.0;
        inputs->omega=0.7;
        inputs->startPosition = 0.78;
        inputs->startVelocity = 50;
    }

    int i;
    double t = 0.0, t1 = 100;
    double dt = 0.01;
    const int step_total = 500;

    double* timeX = malloc(step_total*sizeof(double));
    double* yOne = malloc(step_total*sizeof(double));
    double* yTwo = malloc(step_total*sizeof(double));

    clock_t start_time = clock(); //Record Start Time
    double timeout = 5.0; // Timeout in seconds

    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 720;

    // System initialization
    gsl_odeiv2_system system = {func, NULL, 2, inputs};

    // ODE solver initialization
    const gsl_odeiv2_step_type *type = gsl_odeiv2_step_rk8pd;
    gsl_odeiv2_step *step = gsl_odeiv2_step_alloc(type, 2);
    gsl_odeiv2_control *control = gsl_odeiv2_control_y_new(1e-6,0.0);
    gsl_odeiv2_evolve *evolve = gsl_odeiv2_evolve_alloc(2);

    double y[2] = {inputs->startPosition, inputs->startVelocity};

    //printf ( " Step method is '%s'\n", gsl_odeiv2_step_name(step));
    i = 0;
    while(t < t1){

        int status = gsl_odeiv2_evolve_apply(evolve, control, step, &system, &t, t1, &dt, y);

        if (status != GSL_SUCCESS){
            printf("error, return value=%d\n", status);
            break;
        }
        timeX[i] = t;
        yOne[i] = y[0];
        yTwo[i] = y[1];
        i++;


        clock_t current_time = clock();
        double elapsed_time = (double)(current_time - start_time) / CLOCKS_PER_SEC;
        if (elapsed_time > timeout){
            printf(" Infinite loop timeout reached. \n");
            break;
        }

    }

    gsl_odeiv2_evolve_reset(evolve);
    gsl_odeiv2_control_free(control);
    gsl_odeiv2_step_free(step);
    gsl_odeiv2_evolve_free(evolve);
    free(inputs);

    int total = i-1;
    i = 0;

    while (i < total){
    printf("At time t=%.3f s, position=%.3f m, velocity=%.3f m/s\n", timeX[i], yOne[i], yTwo[i]);
    i++;
    }

    i=0;
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, " Pendelum ODE Oscilation Plot. ");

    SetTargetFPS(60);

    while(!WindowShouldClose()){

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){

            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);

            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            camera.offset = GetMousePosition();

            camera.target = mouseWorldPos;

            const float zoomIncrement = 0.125f;

            camera.zoom += (wheel*zoomIncrement);
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        }

        BeginDrawing();

            BeginMode2D(camera);

                ClearBackground(RAYWHITE);

                DrawLine(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT /2, BLACK);

                while(i <= total){
                    int screenX = timeX[i]*5 ;
                    int screenY1 = (SCREEN_HEIGHT/5) + yOne[i]*5;
                    int screenY2 = (SCREEN_HEIGHT/5) + yTwo[i]*5;

                    DrawCircle(screenX, screenY1, 2, GREEN);
                    DrawCircle(screenX, screenY2, 2, BLUE);
                    i++;

                }
                i = 0;

            EndMode2D();

        EndDrawing();

    }

    CloseWindow();

    free(timeX);
    free(yOne);
    free(yTwo);


    return 0;

}