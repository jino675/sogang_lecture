clear all; close all; clc;
n = [10 20 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900 1000];
time = 1e-6.*[1 1 2 3 5 8 10 12 16 18 80 147 276 428 625 842 1060 1399 1650];
p = polyfit(n,time, 2)
y = polyval(p,n);
plot(n,time);
hold on;
title('n-time gragh');
plot(n,y, '--r');
legend('n-time','0.002n^2+0.0063n-0.2539');
grid on;
xlabel('n');
ylabel('time (s)');
axis([10 1000 0.000001 0.001650]);
