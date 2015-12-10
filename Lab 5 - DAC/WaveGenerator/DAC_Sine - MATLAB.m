close; clear; clc;

points = 32;        %кол-во элементов в массиве
bits   = 12;        %разрядность ЦАП
offset = 75;        %смещение

t = 0:(2*pi/(points-1)):(2*pi);
y = sin(t);
y = y + 1;                              %смещение в положительную область
y = y*((2^bits-1)-2*offset)/2+offset;   %ограничиваем диапазон
y = round(y);
plot(t, y); grid

fprintf('%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, \n', y);