load('imgXRAY.mat');
load('points.mat');
img = imgXRAY;
sig = mean2(img);
noi = std2(img);
sig_noi_r = sig/noi;
    snrdb = 20*log10(sig_noi_r)
    cnr = (max(max(img))-min(min(img)))/noi
    imagesc(img);
    A = [x(1) y(1)] ; 
    B = [x(2) y(2)] ; 
    C = [x(3) y(3)] ; 
  %  [x,y] = ginput(3);
  
%   theta1 = acos( (x(1)*x(2) + y(1)*y(2))/(sqrt(x(1)^2+y(1)^2)*sqrt(x(2)^2+y(2)^2)));
%   theta2 = acos( (x(3)*x(2) + y(3)*y(2))/(sqrt(x(3)^2+y(3)^2)*sqrt(x(2)^2+y(2)^2)));
%   ang = (theta2+theta1)*180
theta = acosd(dot(B-A, B-C)/(norm(A-B)*norm(C-B)))
