function lines = header_lines(filenames)
%HEADER_LINES    Retrieve header lines from Avida data files.
%
%    R = HEADER_LINES(dir, file_pattern) examines the first 
%    file returned by the given directory and file pattern, 
%    and returns the header lines from that file.  The search 
%    is performed recursively.
%      dir(string): Absolute or relative path.
%      file_pattern(regex): Pattern used to match files.
%
%    See also: load_all

assert(size(filenames,2) > 0, 'No files to check for header!');

f=char(filenames{1});
[result status] = unix(['file ' f]);

if regexp(status, 'gzip')
    [status result] = unix(['gzcat ' f]);
    lines = textscan(result, '%s','Delimiter','\n');
else
    fid = fopen(char(f));
    lines = textscan(fid, '%s','Delimiter','\n');
    fclose(fid);        
end

lines = lines{1}(strmatch('#',lines{1}));
