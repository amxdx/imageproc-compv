close all ; clc; 
ramlak = zeros(512,512,4);
    for i=1:512
         for j=1:512
             ramlak(i,j,1)=sqrt((i-256)^2+(j-256)^2);
         end
    end
 ramlak(:,:,2) = ramlak(:,:,1);
 ramlak(:,:,3) = ramlak(:,:,1);
 ramlak(:,:,4) = ramlak(:,:,1);
figure; 
imagesc(ramlak(:,:,1));
colormap gray;
title('RamLak Filter');
colsumfft=fft2(colsums);
ramlak = fftshift(ramlak);
final=colsumfft.*ramlak;
filtered=real(ifft2(final));

figure; 
subplot(1,2,1), imagesc(colsums(:,:,1)), title('Original Image 1');
subplot(1,2,2), imagesc(filtered(:,:,1)), title('Flitered using RamLak Filter');
figure; 
subplot(1,2,1), imagesc(colsums(:,:,2)), title('Original Image 2');
subplot(1,2,2), imagesc(filtered(:,:,2)), title('Flitered using RamLak Filter');
figure; 
subplot(1,2,1), imagesc(colsums(:,:,3)), title('Original Image 3');
subplot(1,2,2), imagesc(filtered(:,:,3)), title('Flitered using RamLak Filter');
figure; 
subplot(1,2,1), imagesc(colsums(:,:,4)), title('Original Image 4');
subplot(1,2,2), imagesc(filtered(:,:,4)), title('Flitered using RamLak Filter');
