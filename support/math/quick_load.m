function R = quick_load(file_pattern, dir, data_pattern, fieldnames)
%QUICK_LOAD    Intelligently load the specified Avida data files.
%
%    R = QUICK_LOAD(DIR,FILE_PATTERN) examines the files returned
%    by the given directory and file pattern, and returns a struct
%    containing filenames, field names, updates, and data from
%    each file returned.  The directory is searched recursively.
%    Gzip'ed files are decompressed on-the-fly.
%      dir(string): Absolute or relative path.
%      file_pattern(regex): Pattern used to match files.
%      R(struct): See below.
%    
%    RETURNS: A struct with one of two formats depending on the
%    data files that are being loaded.
%
%      Format 1 (preferred): This format is used if each of the
%      header lines in the data files contains a naming tag.  The
%      naming tag, which must appear at the end of the header, has
%      the following (regex) format: '.*\[(\w+)\]$'.  When each
%      header has such a naming tag, the returned struct R has the
%      following format:
%        R.updates(1 x n double array): Updates - Avida time.
%        R.filenames(cell array of chars): Names of the loaded files.
%        R.fieldnames(cell array of chars): Header lines from files.
%        R.<tag>(m x n matrix of doubles): Rows are data from different
%          files, columns are values at different updates.  The <tag>
%          specifies the measurement (column from the data files).
%
%      Example: If the following header lines occur in each Avida
%      data file:
%        # 1: updates
%        # 2: mean value of foo [foo]
%        # 3: max value of bar [bar]
%      Then the following struct will be returned:
%        R.updates = [1...]
%        R.fieldnames = {{'# 1: mean value of foo [foo]'},
%          {'# 2: max value of bar [bar]'}}
%        R.filenames = {filename1, filename2...}
%        R.foo = [[foo from file1]; [foo from file2]...]
%        R.bar = [[bar from file1]; [bar from file2]...]
%
%      Format 2: This format is used if the header lines in the
%      data files do not have a naming tag.
%        R.filenames(cell array of chars): Names of the loaded files.
%        R.fieldnames(cell array of chars): Header lines from files.
%        R.data(m x n x p matrix of doubles): Rows are data from
%          different files, columns are values at different updates,
%          pages are different measurements (column from the data
%          files).
%
%      Example: If the following header lines occur in at least one
%      Avida data file:
%        # 1: updates
%        # 2: mean value of foo
%        # 3: max value of bar
%      Then the following struct will be returned:
%        R.fieldnames = {{'# 1: mean value of foo'},
%          {'# 2: max value of bar'}}
%        R.filenames = {filename1, filename2...}
%        R.data(:,:,1) = [[updates from file1]; [updates from file2]...]
%        R.data(:,:,2) = [[foo from file1]; [foo from file2]...]
%        R.data(:,:,3) = [[bar from file1]; [bar from file2]...]
%
%    See also: header_lines, load_all

if nargin < 2
    dir = './';
end

filenames = find_files(dir, file_pattern);
lines = header_lines(filenames);
lines = lines(~cellfun('isempty',regexp(lines,'^#\s+\d+:\s')));

if nargin <= 2    
    data_pattern = arrayfun(@(x){'%n'},lines);
    data_pattern = cat(2,data_pattern{:});
end

[ldata fnames] = load_files(filenames, data_pattern);

tokens=regexp(lines,'.*\[(\w+)\]$','tokens');
if isempty(tokens) || any(cellfun('isempty',tokens))
    R.filenames=fnames;
    R.fieldnames=lines(1:end);
    R.data=ldata;
else
    R.filenames=fnames;
    R.fieldnames=lines;
    for i=1:size(ldata,3);
        R.(char(tokens{i}{:})) = ldata(:,:,i);
    end
end
