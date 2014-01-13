#!/usr/bin/perl

#########################################################################
#
#  Script to create the ../../../documentation/actions.html file based on 
#  routines in the *Action.cc files in the ../../actions/ and information
#  in the actions_source_info file in the current directory
#
#  Created by B.D.Baer 20-February-2007
#  Copyright 2008 Michigan State University. All rights reserved. 
#
#########################################################################

use strict;
use Time::Local;

my $source_dir = "../../actions/";
my $final_doc = "../../../documentation/actions.html";
my $template_file = "./actions_source_info";
my %section_hash;
my %command_line_hash;
my %action_html_hash;
my %section_header_hash;
my @section_list;
my @head_lines;
my @tail_lines;

# First get a directory of c++ files in the actions source directory

my $found_files = 1;
my $source_file_list = `ls $source_dir\*\.cc` or $found_files = 0;

if ($found_files) {
  my @source_files = split /\n/,$source_file_list;
  my $source_file;

  # If the file name is in the form xxxxxActions.cc read the action names

  foreach $source_file (@source_files) {
    if ($source_file =~ /Actions\.cc/) {
      process_action_file($source_file);
    }
  }
} else {
  print "No C++ code found in $source_dir\n\n";
  exit;
}

# Process the file that contains html information

process_template_file();

# Write the html file

print "about to write to $final_doc\n";

open (OUTPUT_FILE,">$final_doc") or die "Can't open file $final_doc";
print OUTPUT_FILE @head_lines;

# Print out table of all actions

my $num_of_actions = keys(%section_hash);
my $curr_action;
my $i = 0;
print OUTPUT_FILE "<table>\n";
print OUTPUT_FILE "  <tr>\n";
foreach $curr_action (sort sort_ic keys %section_hash) {
  if (($i == 0) or ($i == int($num_of_actions/3)) or 
      ($i == 2 *  int($num_of_actions/3))) {
    print OUTPUT_FILE "    <td valign=\"top\">\n";
  }
  print OUTPUT_FILE "      <a href=\"\#$curr_action\">$curr_action</a><br>\n";
  $i++;
}
print OUTPUT_FILE "  </tr>\n";
print OUTPUT_FILE "</table>\n";

# Print out each section header and actions in that section

my $curr_section;
foreach $curr_section (@section_list) {
  print OUTPUT_FILE $section_header_hash{$curr_section};
  print OUTPUT_FILE "<UL>\n";
  foreach $curr_action (sort sort_ic keys %section_hash) {
    if ($section_hash{$curr_action} eq $curr_section) {
      print OUTPUT_FILE "<li><p>\n";
      print OUTPUT_FILE "  <strong><a name=\"$curr_action\">$curr_action</a>" .
                        "</strong>\n";
      if ($command_line_hash{$curr_action}) {
        print OUTPUT_FILE "  <i>$command_line_hash{$curr_action}</i>\n";  
      }
      print OUTPUT_FILE "  </p>\n";
      print OUTPUT_FILE "  <p>\n";
      print OUTPUT_FILE "  $action_html_hash{$curr_action}\n";  
      print OUTPUT_FILE "  </p>\n";
      print OUTPUT_FILE "</li>\n";
    }
  }
  print OUTPUT_FILE "</UL>\n";
}

print OUTPUT_FILE @tail_lines;
close OUTPUT_FILE;
exit;

############################################################################

# Routine to process C++ code the defines actions

sub process_action_file {
  my $file_name = pop;
  my $section_name = $file_name;
  $section_name =~ s/Actions\.cc//;
  $section_name =~ s/$source_dir//;
  push (@section_list, $section_name);
  my $curr_class = "";
  my $line_finished = 1;
  my %action_to_class_h;
  my %class_to_command_line_h;

  print "Processing $file_name\n";
  open (SOURCE, $file_name);
  while (<SOURCE>) {

    # find class names of actions

    if (/^class/) {
      my @tmp = split;
      $curr_class = $tmp[1];

    # find argument list of actions

    } elsif (/GetDescription/ or !($line_finished)) {
      if (/;/) {
        $line_finished = 1;
      } else {
        $line_finished = 0;
      }
      s/\\\"/'/g;
      my @tmp = split /\"/;
      $class_to_command_line_h{$curr_class} .= $tmp[1];
      $class_to_command_line_h{$curr_class} =~ s/\'/\"/g;

    # find where action names are linked to their class

    } elsif (/Register/ and !(/void/)) {
      s/ ?action_lib->Register<//;
      s/>\(//;
      my @tmp = split/\"/;
      $tmp[0] =~ s/^ *//;
      $action_to_class_h{$tmp[1]} = $tmp[0];

    # Handle special cases in PrintActions.cc where a number of actions
    # are defined using macro expension

    } elsif (/^ *STATS_OUT_FILE/ or /^ *POP_OUT_FILE/) {
      s/ //g;
      my @tmp = split/[(,)]/;
      $class_to_command_line_h{"cAction$tmp[1]"} = "[string fname=\"$tmp[2]\"]";
    }
  }
  close (SOURCE);
  my $class_item;
  foreach $class_item (sort keys %class_to_command_line_h) {
    $class_to_command_line_h{$class_item} =~ s/Arguments: //i;
  }

  # Connect the action to information about the class

  my $action;
  foreach $action (sort keys %action_to_class_h) {
    $section_hash{$action} = $section_name;
    my $ActClass = $action_to_class_h{$action};
    $command_line_hash{$action} = $class_to_command_line_h{$ActClass};
    $command_line_hash{$action} =~ s/</&lt;/g;
    $command_line_hash{$action} =~ s/>/&gt;/g;
  }
}

# Procedure process the file that contains html information

sub process_template_file{

  my $state = "";
  my $action = "";
  my $section = "";

  open (TEMPLATE, $template_file) or die "Couldn't open file $template_file\n";
  while (<TEMPLATE>) {

    # Find the header (=== xxxx ===) lines
  
    if (s/^=== //) {
      s/ ===$//;
      chomp;
      $state = $_;
      if (s/^Action //i) {
        $action = $_;
      } elsif (s/^section //i) {
        $section = $state;
        $section =~ s/^section //i;
        $section =~ s/ actions$//i;
        $section =~ s/ //;
      }
    } elsif ($state =~ /^action/i) {
      $action_html_hash{$action} .= $_;
    } elsif ($state =~/^section/i) {
      $section_header_hash{$section} .= $_;
    } elsif ($state =~/^head/i) {
      
      # Check for a line with the date template and replace it 
      
      if (/xxxx-xx-xx/) {
        $_ = "<br>Generated " . localtime(time) . " by make_actions_html";
      }
      push (@head_lines, $_);
    } elsif ($state =~ /^tail/i) {
      push (@tail_lines, $_);
    }
  }
  close (TEMPLATE);
}

# Create sort that ignores case and underscores

sub sort_ic {
 my $c = $a; my $d = $b;
 $c =~ s/_//; $d =~ s/_//;
 lc($c) cmp lc($d);
} 
