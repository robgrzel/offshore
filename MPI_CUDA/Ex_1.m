function DYN_Ex_1%%calculation of forces acting on a floating cylinder in still water
  
clc, clear all, close all
g=9.81;%%m/s^2
%%z=1;%%m - initial position
D=2;%%m - Dia of structure
To=4;%%m - draft of structure
H=6;%%m - depth or height of structure
Density=1025;%% kg/m^3
b33=0;%%Dampening coeff
CD=0.41;
CA=0.4;

AP=(pi()*((D/2)^2));
AW=AP;
m0=AW*To*Density;
%%m0=mb;

n=3000;
t(1)=0;
del_t=0.02;
z(1)=1;
dot_z(1)=0;
ddot_z(1)=0;
for i=1:n
  ddot_z(i+1)=-((b33*dot_z(i))+(CD*0.5*Density*AP*abs(dot_z(i))*dot_z(i))+(Density*g*AW*z(i)))/((1+CA)*m0);
  dot_z(i+1)=dot_z(i)+ddot_z(i+1)*del_t;
  z(i+1)=z(i)+(0.5*(dot_z(i)+dot_z(i+1))*del_t);
  t(i+1)=t(i)+del_t;
  
end
%%graphics_toolkit('gnuplot')
figure 1
grid on, hold on;
plot(t,z,t,dot_z,t,ddot_z)
legend('z','dot_z','ddot_z')