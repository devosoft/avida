#!/usr/bin/env python
#
# Fix the copyright lines in the file headers

import re, os

# Precompile some regexes
dateRE = re.compile( r"(^//.*Copyright.*)(\d.*\d)(.*California.*Technology.*$)" );
licRE = re.compile( r"(^//.*)((?:LICENSE)|(?:COPYING))(.*)('.*',)(.*$)" );
sourceRE = re.compile( \
    r"(\.cc\b)|(\.hh\b)|(\.ii\b)|(\.c\b)|(\.h\b)|(\.java\b)" )

# The canonical copyright
cprString = [\
"//////////////////////////////////////////////////////////////////////////////\n",\
"// Copyright (C) 1993 - 2000 California Institute of Technology             //\n",\
"//                                                                          //\n",\
"// Read the COPYING and README files, or contact 'avida@alife.org',         //\n",\
"// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //\n",\
"//////////////////////////////////////////////////////////////////////////////\n"]

def modFiles( arg, dirname, name_list ):
    print "Entering %s\n" % dirname
    for ifile in name_list:
        dMatch = 0
        licMatch = 0
        filename = os.path.join(dirname, ifile)
        print filename,
        if ( os.path.isdir(filename) ):
            print "DIR"
            continue
            
        # Ignore non-source files
        if not sourceRE.search( ifile ):
            print "NOT SOURCE"
            continue
            
        lines = open( filename ).readlines()
        for iLine in range(len(lines)):
            # First try matching with the date line
            if dateRE.match( lines[iLine] ):
                lines[iLine] = cprString[1]
                dMatch = 1
            
            # Now try matching with the License line
            if licRE.match( lines[iLine] ):
                lines[iLine] = cprString[3]
                licMatch = 1
        
        nMatch = licMatch + dMatch
        print "%s %s" % (licMatch, dMatch),
        if( nMatch == 0 ):
            # The (C) lines aren't there, so add them
            lines[:0] = cprString
        elif (nMatch == 1):
            # If we only found one line skip this file (too weird)
            print
            continue
                    
        # write out the altered version
        print "fixed"
        open( filename, 'w' ).writelines( lines )
            
print 'Starting'
os.path.walk( ".", modFiles, 1 )
