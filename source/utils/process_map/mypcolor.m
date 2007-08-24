function h = pcolor(x,y,c)
%PCOLOR Pseudocolor (checkerboard) plot.
%   PCOLOR(C) is a pseudocolor or "checkerboard" plot of matrix C.
%   The values of the elements of C specify the color in each
%   cell of the plot. In the default shading mode, 'faceted',
%   each cell has a constant color and the last row and column of
%   C are not used. With shading('interp'), each cell has color
%   resulting from bilinear interpolation of the color at its 
%   four vertices and all elements of C are used. 
%   The smallest and largest elements of C are assigned the first and
%   last colors given in the color table; colors for the remainder of the 
%   elements in C are determined by table-lookup within the remainder of 
%   the color table.
%   PCOLOR(X,Y,C), where X and Y are vectors or matrices, makes a
%   pseudocolor plot on the grid defined by X and Y.  X and Y could 
%   define the grid for a "disk", for example.
%   PCOLOR is really a SURF with its view set to directly above.
%   PCOLOR returns a handle to a SURFACE object.
%
%   See also CAXIS, SURF, MESH, IMAGE, SHADING.

%-------------------------------
%   Additional details:
%
%
%   PCOLOR sets the View property of the SURFACE object to directly 
%   overhead.
%
%   If the NextPlot axis property is REPLACE (HOLD is off), PCOLOR resets 
%   all axis properties, except Position, to their default values
%   and deletes all axis children (line, patch, surf, image, and 
%   text objects).  View is set to [0 90].

%   Copyright (c) 1984-96 by The MathWorks, Inc.
%   $Revision: 5.3 $  $Date: 1996/01/01 23:34:43 $

%   J.N. Little 1-5-92

if nargin < 1
    error('Too few input arguments.');
elseif nargin > 4
    error('Too many input arguments.')
end

cax = gca;
hold_state = ishold;

if nargin == 1
    hh = surface(zeros(size(x)),x);
    [m,n] = size(x);
    lims = [ 1 n 1 m];
elseif nargin == 3
    hh = surface(x,y,zeros(size(c)),c);
    lims = [min(min(x)) max(max(x)) min(min(y)) max(max(y))];
else
    error('Must have one or three input arguments.')
end
if ~hold_state
    set(cax,'View',[0 90]);
    set(cax,'Box','on');
    axis(lims);
end
if nargout == 1
    h = hh;
end
