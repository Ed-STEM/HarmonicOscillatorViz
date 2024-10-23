#include "datafile.h"
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>


struct params{
    double K;
    double M;
};

int func (double t, const double y[], double f[], void *params){
    struct params *p = (struct params *)params;
    //int y[4] = {0,1,2,3};
    //int v[4] = {3,4,5,6}; 
    f[0] = (2*y[0]-y[1])*-p->K/p->M;
    f[1] = (-y[0]+2*y[1]-y[2])*-p->K/p->M;
    f[2] = (-y[1]+2*y[2])*-p->K/p->M;
    return GSL_SUCCESS;
}

int main(){
     
    double t = 0.0, t1 = 100.0;
    bool pause = 0;
    struct params p = {1,1};

    double y[2] = { 1.0, 0.3 };

    gsl_odeiv2_system odesys = {func, NULL, 3, &p};
    gsl_odeiv2_driver *odedrive = gsl_odeiv2_driver_alloc_y_new(&odesys, gsl_odeiv2_step_rkf45, 1e-6,1e-6,0.0);
   
    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 720;

    // Initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Plot Graphs");

    SetTargetFPS(60);

    // Main Graphics Loop
    while(!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, BLACK);
        DrawLine(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT /2, BLACK);

        // Plot Runge-Kutte 45 ODE solution curve for r1
        if (!pause){
            for ( int i = 1; i <= 100; i++){
                double ti = i * t1 / 100.0;
                //double dt = ti -t;
                //int status = gsl_odeiv2_driver_apply_fixed_step(odedrive, &t, dt, 1, y);

                int status = gsl_odeiv2_driver_apply_fixed_step(odedrive, &t, ti,1, y);

                if (status != GSL_SUCCESS)
                {
                    DrawText(TextFormat("error, gsl return value=%d ", status), SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + 100, 20, RED);
                    pause = !pause;
                    break;
                } 
                
                int screenX = (int)((t / t1) * SCREEN_WIDTH);
                int screenY1 = SCREEN_HEIGHT / 2 - (int)((y[0] + 5)* SCREEN_HEIGHT/ 10 ); // Adjust scaling factor as needed
                int screenY2 = SCREEN_HEIGHT / 2 - (int)((y[1] + 5) * SCREEN_HEIGHT / 10 ); // Adjust scaling factor as needed
                
                //Not drawing the loop...
                DrawCircle(screenX, screenY1, 2, GREEN); // For y[0]
                DrawCircle(screenX, screenY2, 2, BLUE);  // For y[1]
                DrawCircle((SCREEN_WIDTH / 2)+(i*5), SCREEN_HEIGHT / 2, 20, YELLOW);

            }
        }
        EndDrawing();

    }

    CloseWindow(); 
    gsl_odeiv2_driver_free(odedrive);
    return 0;

}