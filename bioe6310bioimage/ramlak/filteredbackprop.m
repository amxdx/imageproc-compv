clear all; clc;close all; 
load sinograms.mat;
imagesc(sinogram(:,:,1));
figure;
imagesc(sinogram(:,:,2));
figure;
imagesc(sinogram(:,:,3));
figure;
imagesc(sinogram(:,:,4));

colsums = zeros(512,512,4);

for i = 1:4
     for j = 1:360 
         ray_sum = repmat(sinogram(:,j,i),[1 512]);
         ray_sum = imrotate(ray_sum,j-1,'bilinear','crop');
         colsums(:,:,i) = colsums(:,:,i)+ray_sum ;
         
     end
figure; 
imagesc(colsums(:,:,i));
title (['Sinogram Image # ',num2str(i)]);
end

         