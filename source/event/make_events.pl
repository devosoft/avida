#
# Make events_auto.cc & events_auto.hh from a simplified template file
# to handle creation of code for simple events
#
# Input file format:
# _event_name_
# :descr:
# /**
#  * Documentation of the event in the typical
#  * class definition style
#  **/
# :args:
# _arg_type _arg_name_ [= _default_value_]
# _arg_type _arg_name_ [= _default_value_]
# ...
# :body:
# _body_of_Process()_function_
# ...
#
# _event_name_...

use strict;

$/ = "\n\n";
$" = "|";  #" <- this quote sign in the comment is necessary for emacs highlighting mode

my @event_lists = @ARGV; #("cPopulation", ...);

my ( $event_dir, $fname, $fname_root );

# Clobber the events_list
foreach $fname (@event_lists)
{
  # separate filename in root name and directory
  if ( $fname =~ m/^-x/ )
  {
    # handle paths from Xcode
    # added by David 2004-12-05
    
    ($fname, $fname_root, $event_dir) = $fname =~ /^-x\[(.*\/)(.*)\]\[(.*)\]/;
    $fname .= $fname_root;
    $event_dir .= "/";
  }
  elsif ( $fname =~ /\// )
  {
    $fname =~ /(.*\/)(.*)/;
    ##
    ## change by kaben...
    ##
    ## this change places all generated files
    ## in the build directory
    ## rather than in the source directory.
    ##
    #$event_dir = $1;
    $event_dir = "./";
    ##
    $fname_root = $2;
    
  }
  else
  {
    $event_dir = "./";
    $fname_root = $fname;
  }

  printf " --- make_events.pl: opening ".$fname.".events.\n";
  open(IN,$fname.".events") || die $!;
  
  # open the output files
  open(LIST,"> ".$event_dir.$fname_root."_event_list");
  open(NCI, "> ".$event_dir.$fname_root."_construct_event_auto.ci");
  open(PCI, "> ".$event_dir.$fname_root."_process_auto.ci");
  open(DEFS,"> ".$event_dir.$fname_root."_enums_auto.ci");
  open(N2E,"> ".$event_dir.$fname_root."_name2enum_auto.ci");
  open(DESCR_DECL,"> ".$event_dir.$fname_root."_descr.hi");
  open(DESCR_DEF,"> ".$event_dir.$fname_root."_descr.ci");


  print LIST "#### ".$fname_root."-Events ####\n\n";

  print DEFS "  enum eEvent { EVENT_undefined = 0,\n";

  print N2E "int ".$fname_root."EventFactory::EventNameToEnum(const cString & name) const{\n";
  print N2E "  if( name == \"\" ){\n";
  print N2E "    return ".$fname_root."EventFactory::EVENT_NO_EVENT;\n";

  print DESCR_DECL "class cEventEntry{
  private:
    const char* m_name;
    const char* m_description;
  public:
      cEventEntry( const char* name, const char* description )
      : m_name( name ), m_description( description ) {;}
    ~cEventEntry() {;}
    /**
      * \@return The name of the event.
     **/
    const char* GetName() const { return m_name; }
    
    /**
      * \@return The description of the event.
     **/
    const char* GetDescription() const { return m_description; }
  };


  class cEventDescrs{
  public:
    const static int num_of_events;
    const static cEventEntry entries[];
  };

  ";

  print DESCR_DEF "const cEventEntry cEventDescrs::entries[] = {
    ";

  my $no_of_events = 0;

  while( <IN> ){
    my @line = split(/\n/);
    my $line_string = "";
    my $i;
    
  # Filter out blank lines and comments (#)
    for $i (0..$#line){
      $_ = $line[$i];
      s/\#.*$//;
        $line_string .= "$_\n"  unless( /^\s*$/ );
    }
    my ( $name, $tmp_string ) = split(/\:descr\:\n/, $line_string);
    my ( $descrstring, $tmp_string2 ) = split(/\:args\:\n/, $tmp_string);
    my ( $argstring, $body ) = split(/\:body\:\n/, $tmp_string2);
    my @args = split(/\n/,$argstring);
    $name =~ /^\s*([a-z_0-9]*)\s*$/i; $name = $1; # Trim ws off of name
  # XXX the perl expression below stopped working on
  # rodan.css.msu.edu; I don't know why; the expression above is a
  # quick workaround. @kgn 21 September 2004
  #$name =~ /\s*([^\s]*)\s*$/; $name = $1; # Trim ws off of name
      
      
      if( $name ){
  # print warning if no documentation has been written
        chomp( $descrstring );
        if ( $descrstring eq "" ){
          print "Warning: Event $name comes without documentation!\nPlease change!\n";
        }
        
  ##### Output Class Declaration
        print PCI "///// $name /////\n\n";
          print PCI $descrstring."\n\n";
          print PCI "class ".$fname_root."Event$name : public ".$fname_root."Event {\nprivate:\n";
          for $i (0..$#args){
            my ($type, $vname, $default) = split(/\s+/, $args[$i]);
            print PCI "  $type $vname;\n";
          }
          $" = ", ";
            print PCI "public:\n";
            print PCI "  ".$fname_root."Event$name(const cString & in_args):\n";
            print PCI "   ".$fname_root."Event(\"$name\", in_args) {\n";
            
  # Argument intialization
            if( @args ){
              print PCI "\n";
              print PCI "    cString args(in_args);\n";
              for $i (0..$#args){
                my ($type, $vname, $default) = split(/\s+/, $args[$i]);
  # @TCC--- SOMEHOW THIS NEEDS TO TAKE MULTI_WORD DEFAULTS
                my $default_provided_flag = 0;  # for checking syntax
                  my $pop_cmd = "args.PopWord()";
                if( $type eq "cString" ){
  # This is the default type
                }elsif( $type eq "int" ){
                  $pop_cmd .= ".AsInt()";
                }elsif( $type eq "double" ){
                  $pop_cmd .= ".AsDouble()";
                }
                if( defined $default ){
                  $default_provided_flag = 1;
                  print PCI "    if (args == \"\") $vname=$default; else $vname=$pop_cmd;\n";
                }else{
                  die "Must Provide Defaults for all variables after first default value\n"  if( $default_provided_flag == 1 );
                  print PCI "    $vname = $pop_cmd;\n";
                }
              }
              print PCI "  }\n";
              
            }else{
              print PCI " (void)in_args; }\n";
            }
            
  ##### the process command
            print PCI "///// $name /////\n";
              print PCI "  void Process(){\n";
              my @body_line = split(/\n/,$body);
              for $i (0..$#body_line){
                print PCI "    $body_line[$i]\n";
              }
              print PCI "  }\n";
              print PCI "};\n\n";
              
              
              
  ##### CC File stuff... Name to class instantiation
              print NCI "    case ".$fname_root."EventFactory::EVENT_$name :\n";
              print NCI "      event = new ".$fname_root."Event$name(arg_list);\n";
              print NCI "      break;\n";
              
  ##### Name to Enum #########
              print N2E "  }else if (name == \"$name\") {\n";
              print N2E "    return ".$fname_root."EventFactory::EVENT_$name;\n";
              
  ##### List of Events #####
              print LIST "$name ";
              for $i (0..$#args){
                my ($type, $vname, $default) = split(/\s+/, $args[$i]);
                if( defined $default ){
                  print LIST " [$type $vname=$default]";
                }else{
                  print LIST " <$type $vname>";
                }
              }
              print LIST "\n";
              
  ###### DEFS ######
              print DEFS "  EVENT_$name,\n";
              
  ###### DESCR ######
  #
  # The description needs some reg-expression magic to
  # remove the c-style like comments.
  #
  ###################
              $descrstring =~ s/\"/\\\"/g;  #" escape all quote characters
                
                
  # now tokenize and remove comment characters ('\','*')
                my ( $descr_line, $descr, $finaldescr, @tokens );	
              @tokens = split /\n/, $descrstring;
              
              $finaldescr = "";
              
              foreach $descr_line (@tokens){
                if ( $descr_line =~ /\s*[\S]+\s(.*)/ ){
                  $descr = $1;
                }else{
                  $descr = "";
                }
                $finaldescr = $finaldescr."\n".$descr;
              }
              
  # remove leading or trailing '\n's
              if ( $finaldescr =~ /\n*(.*\n)\n*/s ){
                $finaldescr = $1;
              }
  else {
                $finaldescr = "\n";
  }

  # finally, escape all \n's.
  $finaldescr =~ s/\n/\\n/g;

  if ( $no_of_events > 0 ){
    print DESCR_DEF ",\n";
  }
  print DESCR_DEF "  cEventEntry( \"$name\", \"$finaldescr\" )";

  $no_of_events += 1;
          } # if( $name )
  }

  print DEFS "EVENT_NO_EVENT };\n";

  print N2E "  }else{\n";
  print N2E "    return ".$fname_root."EventFactory::EVENT_undefined;\n";
  print N2E "  }\n";
  print N2E "}\n";

  print DESCR_DEF " };

  const int cEventDescrs::num_of_events = $no_of_events;

  ";

} # for each ARGV to fnameroot


