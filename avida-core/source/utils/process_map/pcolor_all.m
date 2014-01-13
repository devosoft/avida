function pcolor_all(matrix)
%PCOLOR_ALL  Make a psudo-color plot of 'matrix' including all columns and rows
%  Works by appending a row and column of zeros 'matrix'
%
%  By default, does 'shading flat'

[tmp1, tmp2] = size(matrix);
matrix = [ matrix zeros(tmp1,1) ; zeros(1,tmp2+1) ];
mypcolor(matrix)
shading flat
