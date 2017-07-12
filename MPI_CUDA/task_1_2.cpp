#include <iostream>

#include <math.h>
#include "UtilSources/Math/const.h"

#define GRAV 9.81
#define RHOW 1025.
#define RHOA 1.225
#define PI 3.14

void calc_tendon(int n, double x) {
    double tendonsTension = 0;
    double Ttx;
    for (int i = 0; i < n; i++) {
    }

}

class Turbine_Platform {

    double m0 = 2200000;
    //cylindrical part of platform
    double DCyl = 7.;
    double T0Cyl = 20.;
    double CdCyl = 1;
    double CaCyl = 1;
    //pontons of platform
    int nPon = 4;
    double hPon = 8;
    double wPon = 4;
    double rPon = 25;
    double CdPon = 2;
    double CaPon = 1.2;
    //legs tendons
    double LTen = 40;
    double LTenZ, LTenX;
    //tower
    double DTow = 5.5;
    double HTow = 95;
    double CdTow = 1.;
    //turbine
    double DTur = 126;
    double CdTur_11_4 = 0.73;
    //sea area, env cond
    double d = 60;
    double UcurrX = 0.8;
    double UwX = 11.4;
    //wave
    double Hw = 9;
    double T = 11.3;
    double projAnglesPontons = 45;

    //basic Params of hull
    double lPon, lPon_;
    double volPontons, volCylSub, vol;
    double dbuoyancy, buoyancy0, buoyancy, weight;
    double displacement0, displacement;
    double areaProjPontons, areaProjCyl, areaProjSub;
    double CdSub, CdAir;

    double areaProjTower, areaProjTurb, areaProjAir;

    void parameters_submerged() {
        lPon = rPon - DCyl / 2;
        lPon_ = sin(deg2rad(projAnglesPontons)) * lPon;


        volPontons = nPon * lPon * wPon * hPon;
        volCylSub = T0Cyl * PI * 0.25 * pow(DCyl, 2);
        vol = volPontons + volCylSub;

        buoyancy0 = buoyancy = RHOW * vol;
        displacement0 = displacement = buoyancy * GRAV;
        weight = GRAV * m0;

        areaProjPontons = 2 * lPon_ * hPon;
        areaProjCyl = DCyl * T0Cyl;
        areaProjSub = areaProjCyl + areaProjPontons;

        CdSub = (CdCyl * areaProjCyl + CdPon * areaProjPontons) / areaProjSub;

        printf("\n################# PARAMETERS SUBMERGED #####################\n");
        printf("lPon=%f, lPonPrim=%f, nPonPrim=%d\n", lPon, lPon_, 2);
        printf("volPontons=%f, volCylSub=%f, vol=%f\n", volPontons, volCylSub, vol);
        printf("buoyancy=%f, Displ=%f, weight=%f\n", buoyancy, displacement, weight);
        printf("areaProjPontons=%f, areaProjCyl=%f, areaProjSub=%f\n", areaProjPontons, areaProjCyl, areaProjSub);
        printf("CdSub=%f\n", CdSub);
        printf("################# PARAMETERS SUBMERGED (END) #####################\n");

    }



    void parameters_air() {
        areaProjTower = DTow * HTow;
        areaProjTurb = PI * 0.25 * pow(DTur, 2);
        areaProjAir = areaProjTower + areaProjTurb;

        CdAir = (CdTow * areaProjTower + CdTur_11_4 * areaProjTurb) / areaProjAir;

        printf("\n################# PARAMETERS AIR #####################\n");
        printf("areaProjTower=%f, areaProjTurb=%f, areaProjAir=%f\n", areaProjTower, areaProjTurb, areaProjAir);
        printf("CdAir=%f\n", CdAir);
        printf("################# PARAMETERS AIR (END) #####################\n");

    }

    double FHydroX;
    double FAeroX;

    double alphaTen;
    double x;
    double x0;

    double Fstat;

    double FrestZ;
    double FtentenX;

    double dz, dz0;

    void forces_stability() {
        FHydroX = CdSub * 0.5 * RHOW * areaProjSub * fabs(UcurrX) * UcurrX;
        FAeroX = CdAir * 0.5 * RHOA * areaProjAir * fabs(UwX) * UwX;

        printf("\n################# FORCES STABILITY #####################\n");
        printf("FHydroX=%f, FAeroX=%f\n", FHydroX, FAeroX);
        printf("################# FORCES STABLITY (END) #####################\n");
    }

    void tendon_initial_horizontal_lenght() {
        LTenX = 2.992835;//- LTen * (E - FHydroX - FAeroX) / (Displ - weight);
        printf("LTenX=%f\n", LTenX);


    }

    void tendon_vertical_lenght() {
        LTenZ = sqrt(LTen * LTen - LTenX * LTenX);
        printf("LTenZ=%f\n", LTenZ);
    }

    void vertical_movement() {
        dz = LTen - LTenZ;
        printf("dz=%f\n", dz);

    }

    void additional_buoyancy() {
        dbuoyancy = areaProjCyl * dz;
        printf("dbuoyancy=%f\n", dbuoyancy);

    }

    void total_displacement() {
        buoyancy = buoyancy0 + dbuoyancy;
        displacement = GRAV * buoyancy;
        printf("buoyancy=%f\n", buoyancy);
        printf("Displ=%f\n", displacement);

    }

    void force_restoring() {
        FrestZ = displacement - weight;
        printf("FrestZ=%f\n", FrestZ);

    }

    void force_tendons_tension() {
        FtentenX = -(LTenX / LTenZ) * FrestZ;
        printf("FtentenX=%f\n", FtentenX);

    }

    double E;

    void equilibrium() {

        E = FHydroX + FAeroX + FtentenX;
        printf("E=%f\n", E);

    }

    void tendon_alpha() {
        alphaTen = rad2deg(asin(LTenX / LTen));
        printf("alphaTen=%f\n", alphaTen);
    }

    double Tz1, Tz2, Tz3, Tz4;
   void forces_tendons(){
       Tz1 =0;
   }

    void calc() {
        forces_stability();

        tendon_initial_horizontal_lenght();
        tendon_vertical_lenght();
        vertical_movement();
        additional_buoyancy();
        total_displacement();
        force_restoring();
        force_tendons_tension();
        equilibrium();
        tendon_alpha();

    }

public:
    Turbine_Platform() {
        parameters_submerged();
        parameters_air();
        calc();
    }

};

int task_1_2() {

    Turbine_Platform();

    return 0;
}