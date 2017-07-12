function DYN_Ex_2%%calculation of forces acting on a floating cylinder in wave
  
clc, clear all, close all
g=9.81;%%m/s^2
%%z=1;%%m - initial position
D=2;%%m - Dia of structure
To=4;%%m - draft of structure
H=6;%%m - depth or height of structure
Density=1025;%% kg/m^3
b33=1000;%%Dampening coeff
CD=0.82/2;
CA=0.2;

AP=(pi()*((D/2)^2));
AW=AP;
m0=AW*To*Density;
mb=m0;

%%wave data
a=0.5;%%m - amplitude of the wave
T=3;%%s-time period of the wave
omega=(2*pi())/T;
k=omega^2/g;
x=0;

n=3000;
t(1)=0;
del_t=0.02;
z(1)=0;
dot_z(1)=0;
ddot_z(1)=0;
for i=1:n
  theta(i)=(k*x)-(omega*t(i));
  eta(i)=a*cos(theta(i));
  uz(i)=omega*a*e^(k*z(i))*sin(theta(i));%%vertical Veloc of the structure due to wave
  dot_uz(i)=-(omega^2)*a*e^(k*(z(i)-To))*cos(theta(i));%%vertical Accel of the structure due to wave
  f_damp(i)=-(b33*dot_z(i));
  f_drag(i)=(CD*0.5*Density*AP*abs(uz(i)-dot_z(i))*(uz(i)-dot_z(i)));
  f_rest(i)=-(Density*g*AW*z(i));
  f_wave(i)=(Density*g*AW*eta(i));
  f_added2(i)=mb*(1+CA)*dot_uz(i);
  ddot_z(i+1)=(f_damp(i)+f_drag(i)+f_rest(i)+f_wave(i)+f_added2(i))/((1+CA)*m0);
  dot_z(i+1)=dot_z(i)+ddot_z(i+1)*del_t;
  z(i+1)=z(i)+(0.5*(dot_z(i)+dot_z(i+1))*del_t);
  t(i+1)=t(i)+del_t;
end
%%graphics_toolkit('gnuplot')

figure 1
grid on, hold on;
plot(t,z,t,dot_z,t,ddot_z)
legend('z','dot_z','ddot_z')