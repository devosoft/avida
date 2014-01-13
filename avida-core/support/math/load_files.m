function [data,filenames] = load_files(filenames,data_pattern)
%LOAD_ALL    Loads multiple data files into a single matrix.
%
%    LOAD_ALL(filenames,data_pattern) loads data from the specified files
%    according to the data pattern.  Automatically handles gzipped files.
%      filesnames(string): Absolute or relative filenames.
%      data_pattern(format): Format used to scan data (see textscan).
%
%    The expected format of the data files is:
%      # == comment
%      m (rows==data points) x n (columns==measurements)
%
%    The format of the output matrix is:
%      m (rows==files) x n (columns==data points) x p (pages==measurements)
%
%    See also: field_names, textscan
%[files, bytes, names] = dirr(dir,file_pattern,'name');

for i=1:size(filenames,2), j=char(filenames{i});
    [result status] = unix(['file ' j]);
    if regexp(status, 'gzip')
        [status result] = unix(['gzcat ' j]);
        x{i} = textscan(result, data_pattern, 'CommentStyle', '#');
    else
        fid = fopen(char(j));
        x{i} = textscan(fid, data_pattern, 'CommentStyle', '#');
        fclose(fid);        
    end
end

m=size(x,2); % each row belongs to a different trial.
n=size(x{1}{1},1); % each column is a different data point.
p=size(x{1},2); % each page is a different variable.

data=zeros(m,n,p);

% for each datafile (rows)
row=1;
for i=1:m
    % if the datafile has a different number of columns
    if size(x{i}{1},1) ~= n
        % remove a row from the returned data
        data(end,:,:)=[];
        % and complain:
        disp(['Warning: killing row ' int2str(i) ' from ' char(filenames{i})]);
    else
        % otherwise, copy all the data from that datafile
        for j=1:p;
            data(row,:,j)=x{i}{j};
        end
        row=row+1;
    end
end