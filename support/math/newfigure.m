function [f a] = newfigure()
%NEWFIGURE Create a new figure, and return a handle to the figure and
%          its axis.
f = figure('XVisual',...
    '0x22 (TrueColor, depth 24, RGB mask 0xff0000 0xff00 0x00ff)',...
    'InvertHardcopy','off',...
    'Color',[1 1 1]);
a = axes('Parent',f,'Position',[0.13 0.1952 0.7771 0.7229]);
box('off');
hold('all');