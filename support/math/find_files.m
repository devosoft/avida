function filenames = find_files(dir, file_pattern)
%FIND_FILES    Recursively finds matching files.
%
%    FILENAMES = FIND_FILES(DIR,PATTERN) examines the passed in directory,
%    and recursively descends searching for files whose complete name
%    matches the specified pattern.
%      dir(string): The root directory at which recursive search begins.
%      file_pattern(regex): The pattern used to find matching files.
%      filenames(cell array of strings): The completely specified filenames
%        of files that matched the file_pattern, relative to dir.

% For right now, we're going to cheat.  No need to reinvent the wheel.
[s r] = unix(['find ' dir ' | grep ' file_pattern]);
assert(s==0, 'Invalid dir or file_pattern.');
filenames = regexp(r,'([\.\w\d-/]*)','tokens');

% sanity checking:
cellfun(@(x)assert(exist(char(x),'file')==2), filenames);


% % DIRR
% % Lists all files in the current directory and sub directories
% % recursively.
% % 
% % [LIST] = DIRR(PATH)
% % Returns a structure LIST with the same fieldnames as returned 
% % by LIST = DIR(PATH)
% % PATH can contain wildcards * and ? after the last \ or / (filename
% % filter)
% % The content of each directory in PATH is listed inside its 'isdir'
% % field with the same format. The 'bytes' field is NOT zero but the
% % sum of all filesizes inside the directory.
% % 
% % [LIST,BYTES] = DIRR(PATH)
% % BYTES is a structure with fields 'total' and 'dir'. 'total' is the total
% % size of PATH. 'dir' is a recursive substructure that contains the
% % same fields ('total' and 'dir') for the subdirectories.
% % 
% % [...] = DIRR(PATH,FILTER)
% % Lists only files matching the string FILTER (non case sensitive
% % regular expression).
% % N.B.: FILTER is optional and must not be equal to a fieldname
% % ('name' or 'date' ... will never be interpreted as filters)
% % 
% % [LIST,BYTES,FIELDOUT] = DIRR(PATH,FIELDIN, ...)
% % FIELDIN is a string specifying a field (of the structure LIST) that
% % will be listed in a separate cell array of strings in FIELDOUT for
% % every file with absolute path at the begining of the string.
% % Multiple fields can be specified.
% % 
% % [LIST,BYTES,FIELDOUT] = DIRR(PATH,FIELDIN,FILTER, ...)
% % Only files for which FIELDIN matches FILTER will be returned.
% % Multiple [FIELDIN, FILTER] couples may be specified.
% % Recursion can be avoided here by setting 'isdir' filter to '0'.
% % For bytes, numeric comparison will be performed.
% % 
% % 
% % EXAMPLES :
% % 
% % DIRR
% % Lists all files (including path) in the current directory and it's
% % subdirectories recursively.
% % 
% % DIRR('c:\matlab6p5\work\*.m')
% % Lists all M-files in the c:\matlab6p5\work directory and it's
% % subdirectories recursively.
% % 
% % Music = DIRR('G:\Ma musique\&Styles\Reggae\Alpha Blondy')
% % Returns a structure Music very similar to what DIR returns 
% % but containing the information on the files stored in
% % subdirectories of 'G:\Ma musique\&Styles\Reggae\Alpha Blondy'.
% % The structure Music is a bit difficult to explore though.
% % See next examples.
% % 
% % [Files,Bytes,Names] = DIRR('c:\matlab6p5\toolbox','\.mex\>','name')
% % Lists all MEX-files in the c:\matlab6p5\toolbox directory in the cell
% % array of strings Names (including path).
% % Note the regexp syntax of the filter string.
% % Bytes is a structure with fields "total" and "dir". total is the
% % total size of the directory, dir is a recursive substructure with
% % the same fields as bytes for the subdirectories. 
% % 
% % [Files,Bytes,Names] = DIRR('c:\toto'...
% %       ,'name','bytes','>50000','isdir','0')
% % Lists all files larger than 50000 bytes NOT recursively.
% % 
% % [Files,Bytes,Dates] = DIRR('c:\matlab6p5\work','date','2005')
% % Lists all dates of files from year 2005. (With path in front of
% % date in the cell array of strings Dates)
% % 
% % 
% % 
% %       v1.02        
% %       Maximilien Chaumon
% %       maximilien.chaumon@chups.jussieu.fr
% %       2006 06 16
% 
% 
% verbose = 0;
% % set to 1 to get folders list in command window
% 
% if nargin == 0
%     chemin = cd;
% end
% if nargout == 0
%     dum = varargin;
%     varargin{1} = 'name';
%     varargin = [varargin(1) dum];
% end
% 
% fields = {'name' 'date' 'bytes' 'isdir'};
% 
% if regexp(chemin,'[\*\?]') % if chemin contains any ? or *
%     filt = regexprep(chemin,'.*[\\/](.*\>)','$1');% get filter
%     filt = regexprep(filt,'\.','\.');% in regexp format
%     filt = regexprep(filt,'\*','.*');
%     filt = regexprep(filt,'\?','.');
%     filt = regexprep(filt,'(.*)','\\<$1');
%     chemin = regexprep(chemin,'(.*)[\\/].*\>','$1');% and chemin
% end
% 
% if not(isempty(varargin)) % if additional fields were provided after chemin
%     for i = 1:length(fields)
%         if strcmp(varargin{1},fields{i})% if first varargin matches a fieldname,
%             % assume no filter was provided,
%             
%             if not(exist('filt','var'))% or it was in chemin and was set just before
%                 filt = '.*';% set it to wildcard
%                 break
%                 
%             end
%         end
%     end
%     if not(exist('filt','var'))% else
%         filt = varargin{1};% first varargin is the filter
%         varargin(1) = [];
%     end
% else% if no additional fields were provided and filter was not in chemin
%     if not(exist('filt','var'))
%         filt = '.*';
%     end
% end
% % determine which varargin are fieldnames
% whicharefields = zeros(1,length(varargin));
% for i = 1:length(varargin)
%     for j = 1:length(fields)
%         if strcmp(varargin{i},fields{j})
%             whicharefields(i) = 1;
%             break
%         end
%     end
% end
% % set f2out and f2outfilt
% f2out = {}; f2outfilt = {};
% idx = 0;
% if not(isempty(varargin))
%     for i = 1:length(varargin)
%         if whicharefields(i)
%             idx = idx + 1;
%             f2out{idx} = varargin{i};
%             f2outfilt{idx} = '';
%         else % if nargin{i} is not a fieldname, assume it's a filter
%             f2outfilt{idx} = varargin{i}; 
%         end
%     end
% end
% 
% %%%%%%%%%%%%%%%%%%%% START
% if verbose
%     disp(chemin);
% end
% 
% list = dir(chemin);
% if isempty(list)
%     disp([chemin ' not found']);
%     if nargout == 0
%         clear list
%     else
%         for i = 1:nargout - 2
%             varargout{i} = [];
%         end
%         sumbytes = 0;
%     end
%     return
% end
% % remove . and ..
% i_file = 1;
% while i_file <= length(list)
%     if strcmp(list(i_file).name,'.')|strcmp(list(i_file).name,'..')
%         list(i_file) = [];
%     else
%         i_file = i_file + 1;
%     end
% end
% 
% % set sumbytes
% sumbytes = struct('total',0,'dir',{});
% sumbytes(1).total = 0;
% i_dir = 0;
% % and all output fields
% for i = 1:size(f2out,2)
%     f2out{2,i} = {};
% end
% filenames = {};
% todel = 0;
% r = 1;
% for i_out = 1:size(f2out,2)
%     if strcmp(f2out{1,i_out},'isdir')
%         if strcmp(f2outfilt{i_out},'0') % check if no recursion is wanted
%             r = 0;
%         end
%     end
% end
% 
% % for each item in list
% for i_file = 1:length(list)
%     for i_out = 1:size(f2out,2) % for every output field
%         if not(isempty(f2outfilt{i_out}))% if there is a filter
%             if strcmp(f2out{1,i_out},'bytes') % if field is 'bytes'
%                 line = [num2str(list(i_file).(f2out{1,i_out})) f2outfilt{i_out} ';']; % compare with filter numerically
%                 if eval(line)% if passes the filter
%                     continue % continue to next field
%                 else
%                     todel(end+1) = i_file; % else set to be deleted
%                 end
%             elseif not(strcmp(f2out{1,i_out},'isdir'))% if field is 'name' or 'date'
%                 if regexpi(list(i_file).(f2out{1,i_out}),f2outfilt{i_out}) % apply filter
%                     continue % continue to next field
%                 else
%                     todel(end+1) = i_file; % else set to be deleted
%                 end
%             end
%         end
%     end
%     % once checked for every field's filter
%     if todel(end) == i_file % if one didn't pass,
%         if not(list(i_file).isdir) % and it's not a directory
%             continue % skip this file and continue
%         end
%     else
%         if regexpi(list(i_file).name,filt) % else, check for general filter on filename
%             sumbytes(1).total = sumbytes(1).total + list(i_file).bytes; % sum bytes of that level
%             for i_out = 1:size(f2out,2)% and assign all output fields with the values of that file
%                 f2out{2,i_out}{end+1} = [chemin filesep num2str(list(i_file).(f2out{1,i_out}))];
%             end
%         else
%             todel(end+1) = i_file; % else the file will be removed from the list structure
%         end
%     end
%     if list(i_file).isdir % if it's a directory
%         if not(r)
%             continue
%         end
%         i_dir = i_dir + 1;
%         cheminext = strcat(chemin,filesep,list(i_file).name);
%         % get it's content by recursion
%         % write the line to enter eval
%         line = '[list(i_file).isdir,sumbytes.dir(i_dir)';
%         for i_out = 1:size(f2out,2)% with all the requested fields as temporary variables
%             line = [line ',f2outtemp{' num2str(i_out) '}'];
%         end
%         line = [line '] = dirr(cheminext,filt'];
%         for i_out = 1:size(f2out,2)
%             line = [line ',f2out{1,' num2str(i_out) '}'];
%             if f2outfilt{i_out}
%                 line = [line ',f2outfilt{' num2str(i_out) '}'];
%             end
%         end
%         line = [line ');'];
%         eval(line);
%         
%         for i_out = 1:size(f2out,2)
%             f2out{2,i_out} = [f2out{2,i_out} f2outtemp{i_out}]; % catenate temporary variables with f2out
%         end
%         % sum bytes 
%         sumbytes(1).total = sumbytes(1).total + sumbytes(1).dir(i_dir).total; % that level + the next one
%         list(i_file).bytes = sumbytes(1).dir(i_dir).total; % and set list(i_file).bytes to that value
%         if list(i_file).bytes & todel(end) == i_file
%             todel(end) = [];
%         end
%     end
% end
% todel(1) = [];
% list(todel) = [];
% 
% 
% for i_out = 1:size(f2out,2)
%     varargout{i_out} = f2out{2,i_out};
% end
% if nargout == 0
%     clear list
%     disp(char(f2out{2,1}));
% end
