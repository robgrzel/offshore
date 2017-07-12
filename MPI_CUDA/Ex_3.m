function DYN_Ex_3 %% Force and Moment on Vertical Monopile fixed to sea bed.
clear all, close all;
clc, clear all, close all

g=9.81;%%m/s^2

a=4.5;%%m - wave amplitude
T=11.3;%%s - time period of wave
D=6.5;%%dia of columnn
d=40;%%m - water depth
Density=1025;%%kg/m^3
CD=1.2;
CM=2.0;

omega=(2*pi())/T;
k(1)=(omega^2)/g
for i=1:15
  k(i+1)=omega^2/(g*tanh(k(i)*d));
end
k(1:15)

k=k(14);%% k value taken after the values starting to become constant
x=0;

n=3000;
t(1)=0;
del_t=0.1;
%%since structture is fixed at bottom we are assuming z as constant values
z(1)=-35;%%m - from waterline
z(2)=-25;%%m - from waterline
z(3)=-15;%%m - from waterline
z(4)=-05;%%m - from waterline
del_z=z(4)-z(3);
dot_z(1)=0;
ddot_z(1)=0;
for i=1:n;
  theta(i)=(k*x)-(omega*t(i));
  for j=1:4;
  uz(i,j)=omega*a*(cosh(k*(z(j)+d))/(sinh(k*d)))*cos(theta(i));%%horizontal Veloc from airy wave theory
  dot_u(i,j)=(omega^2)*a*(cosh(k*(z(j)+d))/(sinh(k*d)))*sin(theta(i));%%horizontal Accel;
  q(i,j)=(CD*0.5*Density*D*(abs(uz(i,j)))*uz(i,j))+(CM*Density*pi()*D*D*(dot_u(i,j))/4);
  end
      
  forces(i)=(((q(i,1))*del_z)+((q(i,2))*del_z)+((q(i,3))*del_z)+((q(i,4))*del_z));
  fmoments(i)=(((q(i,1))*(z(1)+40))+((q(i,2))*(z(2)+40))+((q(i,3))*(z(3)+40))+((q(i,4))*(z(4)+40)));
  t(i+1)=t(i)+del_t;
 end
  max(forces)
  max(fmoments)
  l=2;
  p=4;
  t(l)
  theta(l)
  uz(l,p)
  dot_u(l,p)
  q(l,p)
  