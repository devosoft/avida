function r = stderr(x)
%STDERR    Calculate the standard error of the given array.
%
%    Standard error calculated as sqrt(variance/size).
r = sqrt(var(x)/size(x,1));