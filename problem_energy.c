/**
 * Kirkwood Gaps
 *
 * This example integrates Sun-Mars-Jupyter and 10000 asteroids.
 * The data comes from the NASA HORIZONS system. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rebound.h"

double ss_pos[3][3] = 
{
    {3.256101656448802E-03  , -1.951205394420489E-04 , -1.478264728548705E-04},  
    // {-1.927589645545195E-01 , 2.588788361485397E-01  , 3.900432597062033E-02 }, 
    // {-5.976537074581466E-01 , 3.918678996109574E-01  , 3.990356741282203E-02 }, 
    // {-7.986189029000561E-01 , -6.086873314992410E-01 , -1.250824315650566E-04}, 
    {7.897942807177173E-01  , 1.266671734964037E+00  , 7.092292179885432E-03 }, 
    {-4.314503046344270E+00 , 3.168094294126697E+00  , 8.331048545353310E-02 }, 
    // {-4.882304833383455E+00 , -8.689263067189865E+00 , 3.453930436208210E-01 }, 
    // {1.917757033372740E+01  , 5.671738750949031E+00  , -2.273858614425555E-01},  
    // {2.767031517959636E+01  , -1.150331645280942E+01 , -4.008018419157927E-01},  
    // {7.765250227278298E+00  , -3.190996242617413E+01 , 1.168394015703735E+00 }, 

};
double ss_vel[3][3] = 
{
    {3.039963463108432E-06 ,  6.030576499910942E-06 ,  -7.992931269075703E-08}, 
    // {-2.811550184725887E-02,  -1.586532995282261E-02,  1.282829413699522E-03 }, 
    // {-1.113090630745269E-02,  -1.703310700277280E-02,  4.089082927733997E-04 },
    // {1.012305635253317E-02 ,  -1.376389620972473E-02,  3.482505080431706E-07 }, 
    {-1.135279609707971E-02,  8.579013475676980E-03 ,  4.582774369441005E-04 }, 
    {-4.555986691913995E-03,  -5.727124269621595E-03,  1.257262404884127E-04 }, 
    // {4.559352462922572E-03 ,  -2.748632232963112E-03,  -1.337915989241807E-04}, 
    // {-1.144087185031310E-03,  3.588282323722787E-03 ,  2.829006644043203E-05 }, 
    // {1.183702780101068E-03 ,  2.917115980784960E-03 ,  -8.714411604869349E-05}, 
    // {3.112825364672655E-03 ,  1.004673400082409E-04 ,  -9.111652976208292E-04},
};

double ss_mass[3] =
{
    1.988544e30,
    // 3.302e23,
    // 48.685e23,
    // 6.0477246e24,
    6.4185e23,
    1898.13e24,
    // 5.68319e26,
    // 86.8103e24,
    // 102.41e24,
    // 1.4639248e+22,
};

void heartbeat(struct reb_simulation* r);
double tmax;
double e_init;

int main(int argc, char* argv[]){
    struct reb_simulation* r = reb_create_simulation();
    // Setup constants
    r->dt                      = 4;                    // 4 days
    r->integrator              = REB_INTEGRATOR_WHFAST;
    r->ri_whfast.coordinates   = REB_WHFAST_COORDINATES_DEMOCRATICHELIOCENTRIC;
    r->heartbeat               = heartbeat;
    tmax                       = (7.3 * 1e8 / 2) * 1;            // 1 My
    r->G                       = 1.4880826e-34;        // in AU^3 / kg / day^2.

    // Initial conditions
    for (int i = 0; i < 3; i++){
        struct reb_particle p = {0};
        p.x  = ss_pos[i][0];         p.y  = ss_pos[i][1];         p.z  = ss_pos[i][2];
        p.vx = ss_vel[i][0];         p.vy = ss_vel[i][1];         p.vz = ss_vel[i][2];
        p.m  = ss_mass[i];
        reb_add(r, p);
    }
    r->N_active = r->N;

    while (r->N < 10002) {
        // here we need to randomly generated particles between 2 and 4 au
        double a = reb_random_uniform(r, 2., 4);
        // and eccentricity between 0 and 0.5
        double e = reb_random_uniform(r, 0., 0.5);
        reb_add_fmt (r, "a e", a, e);
    }
    reb_move_to_com(r);
    e_init = reb_tools_energy(r);
    system("rm -rf orbits_energy.txt");
    reb_integrate(r, tmax);
}

void heartbeat(struct reb_simulation* r){
    if (reb_output_check(r, 7.3 * 1e3 / 2)){
        reb_output_timing(r, tmax);
    }
    if (reb_output_check(r, 7.3 * 1e7 / 2)){
        FILE* f = fopen("orbits_energy.txt", "a");
        reb_integrator_synchronize(r);
        double e = reb_tools_energy(r);
        fprintf(f, "%e %e\n", r->t, fabs((e - e_init) / e_init));
        fclose(f);
    }
}
