#!/usr/bin/perl

###########################################
#
# BIOS/DMI floppy logic.
#
# Roger Dahl, 11/13/03, initial entry
#
# Global variables:
# $model (first selection made by user from menu. matches unit model sticker. key of csvs)
# Values stored in $dmi{"xxx"}:
# - the DMI values are first read from DMI,
# - then overlapped/filled in by any values that are found in models.csv,
# - then some are checked for validity and adjusted by user
# - then the rest are checked for validity and adjusted by the user
#
# MENU
# $model
# RETAIL (from DMI)
# $dmi{"serial"}
# CTO (from DMI if CTO, from models.csv if retail)
# $dmi{"stag"}
# $dmi{"hwmcode"}
# $dmi{"swmcode"}
# $dmi{"pcode"}
# $dmi{"pmcode"}
# IMPLICIT (from models.csv only)
# $dmi{"pname"}
# $dmi{"blid"}
# $dmi{"uinfo"}
#
###########################################

#
# environment and global variables
#
# get path environment variables and change \ to /
$tmp_dir = $ENV{"TMP_DIR"};
$tmp_dir =~ s/\\/\//g;
$input_dir = $ENV{"INPUT_DIR"};
$input_dir =~ s/\\/\//g;
# read the dmi variables from text files to memory
&GetDMI;
# cache models.csv
&ReadModels;
# seed rng
srand(time ^ $$);
# bios name
$bios_name = $ENV{"BIOS_NAME"};
# force BIOS name match for floppy compatibility
$bios_force = $ENV{"BIOS_FORCE"};

#
# verify bios type if bios_force set
#
if ($bios_force && $bios_name ne $dmi{"biosname"}) {
  print "- BIOS detected: >", $dmi{"biosname"}, "< Expected: >$bios_name<\n\n";
  exit 3;
}

#
# main loop
#
for (;;) {
  # print menu, get and verify user's menu selection
  $usersel = &GetMenuSelection;
  # check dmi
  if ($usersel eq "0") {
    &DisplayDMI;
    next;
  }
  # write BIOS
  if ($usersel eq "1") {
    exit 2;
  }
  $model = $usersel;
  # write DMI
  # get model info
  &GetModel($usersel) || ($! = 255, die "Fatal Error: Couldn't find the model in models.csv and/or mcodes.csv");
  # prompt user to verify and enter new DMI strings
  &VerifyDMI;
  # translate all DMI strings to upper case
  &UpperTrimDMI;
  # confirm all dmi strings
  if (&ConfirmAllDMI) {
    last;
  }
}
# use template DMI batch file to create model specific bat
&CreateBats;
exit 1;

#
# menu
#
# Print menu, get and verify user selection.
sub GetMenuSelection {
  # print menu
  $- = 0; # ensure that form top is printed each time
  $^L = ""; # prevent formfeed character from being printed

  local ($id) = 0;
  for (@modelslines) {
    $id += 1;
    if ($id == 1) {
      next; # skip header row in models.csv
    }
    @fields = split(/,/, $_);
    $mname = $fields[0];
    $cto = ($fields[4] eq "?" ? "CTO" : "");
    write;
  }
  # forms don't have support for footers so print one manually
    print "ÈÍÍÍÍÏÍÍÍÍÍÍÍÏÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼\n";
  # get selection
  for (;;) {
    local ($usersel) = @_;
    # get user's selection
    $usersel = &GetUserNumber("- Please type a number from the menu");
    # verify
    if ($usersel == 0) {
      print "- Display DMI...\n\n";
      return $usersel;
    }
    if ($usersel == 1) {
      print "- Write BIOS...\n\n";
      return $usersel;
    }
    $id = 1;
    for (@modelslines) {
      @fields = split(/,/, $_);
      if ($id == $usersel) {
        print "- Write DMI for model: $fields[1]\n\n";
        return $fields[0];
      }
      $id += 1;
    }
  }
}

#
# DMI / CSV
#
# Prompt user to verify and enter new DMI strings.
sub VerifyDMI {
  # for retail and cto
  $dmi{"serial"} = &GetOrVerifyDMI("Please use unit sticker to confirm the serial number", $dmi{"serial"}, "serial number", 7, 7, 1, 1, 0);
  # stag logic:
  # if retail: ask if sticker is there. if sticker, let user type stag.
  # if no sticker: set to 01.
  # if cto: always ask.
  if ($dmi{"swmcode"} eq "?") { # swmcode eq ? is safe check for CTO
    $dmi{"stag"} = &GetOrVerifyDMI("Please use unit sticker to confirm service tag", $dmi{"stag"}, "service tag", 8, 8, 0, 0, 0);
  }
  else {
    if (&YesOrNo("- Does this unit have a service tag sticker")) {
      $dmi{"stag"} = &GetOrVerifyDMI("Please use unit sticker to confirm service tag", $dmi{"stag"}, "service tag", 8, 8, 0, 0, 0);
    }
    else {
      $dmi{"stag"} = "01";
    }
  }
  # for cto
  if ($dmi{"pcode"}   eq "?") { $dmi{"pcode"}  = &GetOrVerifyDMI("Please use unit sticker to confirm the product code", $dmi{"pcode"}, "product code", 7, 8, 1, 0, 0); }
  if ($dmi{"pmcode"}  eq "?") { $dmi{"pmcode"} = &GetOrVerifyDMI("Please use database to confirm product mcode", $dmi{"pmcode"}, "product mcode", 4, 4, 0, 0); }
  if ($dmi{"hwmcode"} eq "?") { $dmi{"hwmcode"} = &GetOrVerifyDMI("Please use database to confirm hardware mcode", $dmi{"hwmcode"}, "hardware mcode", 8, 8, 0, 0, 1); }
  if ($dmi{"swmcode"} eq "?") { $dmi{"swmcode"} = &GetOrVerifyDMI("Please use database to confirm software mcode", $dmi{"swmcode"}, "software mcode", 12, 12, 0, 0, 1); }
  if ($dmi{"uinfo"}   eq "?") { $dmi{"uinfo"} = &GetOrVerifyDMI("Please use database to confirm universal info", $dmi{"uinfo"}, "universal info", 8, 8, 0, 0); }
  if ($dmi{"blid"}    eq "?") { $dmi{"blid"} = &GetOrVerifyDMI("Please use database to confirm BIOS lock ID", $dmi{"blid"}, "blid", 8, 8, 0, 0, 0); }
}

# Prompt user to verify and/or enter a single DMI string.
sub GetOrVerifyDMI {
  local ($msg, $str, $strname, $minlen, $maxlen, $is_numeric, $is_sn, $is_hex) = @_;
  # print guideline for this string
  print "- *** $msg ***\n\n";
  # if string looks sane, let user accept or reject with Y/N
  if (&SaneCheck($str, $minlen, $maxlen, $is_numeric, $is_sn, $is_hex)) {
    if (&YesOrNo("- Is this the correct $strname: $str")) {
      return $str;
    }
  }
  # string does not look sane or was rejected by user
  for (;;) {
    # generate string that describes limitations
    if ($minlen == $maxlen) {
      $lim = "$minlen";
    }
    else {
      $lim = "$minlen-$maxlen";
    }
    if ($is_numeric) {
      $lim .= " digits";
    }
    if ($is_hex) {
      $lim .= " hex (0-9 and A-F) digits";
    }
#    if ($is_sn) {
#      $lim .= ", range $dmi{minserial} - $dmi{maxserial} ";
#    }
    if (!$is_numeric && !$is_hex && !$is_sn) {
      $lim .= " characters";
    }
    # get string from user
    if($is_numeric) {
      $str = &GetUserNumber("- Please type the $strname ($lim)");
    }
    else {
      $str = &GetUserString("- Please type the $strname ($lim)");
    }
    if (&SaneCheck($str, $minlen, $maxlen, $is_numeric, $is_sn, $is_hex)) {
      return $str;
    }
    else {
      print "- Error: Invalid value. Try again...\n\n";
    }
  }
}

# Check if a DMI string looks valid.
sub SaneCheck {
  local ($str, $minlen, $maxlen, $is_numeric, $is_sn, $is_hex) = @_;
  if (length($str) < $minlen) { return 0; }
  if (length($str) > $maxlen) { return 0; }
  if ($is_numeric && !&IsNumeric($str)) { return 0; }
#  if ($is_sn && !&CheckSerial($str, 0)) { return 0; }
  if ($is_hex && !&IsHex($str)) { return 0; }
  return 1;
}

# Check if serial number is within valid range.
sub CheckSerial {
  local ($serial) = @_;
  # 1234567 is sane serial value (for wrpc)
  if ($serial == 1234567 || ($serial >= $dmi{minserial} && $serial <= $dmi{maxserial})) {
    return 1;
  }
  return 0;
}

# Prompt user to confirm all DMI strings.
sub ConfirmAllDMI {
  &DisplayAllDMI;
  if (&YesOrNo("\n- Are the above values correct")) {
    return 1;
  }
  return 0;
}

# Display all DMI strings.
sub DisplayDMI {
  # get strings from txt files
  &GetDMI;
  #
  &DisplayAllDMI;
  print "\nPress any key to continue\n\n";
  &GetSingleChar;
}

sub DisplayAllDMI {
  print "Model name:            $dmi{\"mname\"}\n";
  print "Product code:          $dmi{\"pcode\"}\n";
  print "Serial number:         $dmi{\"serial\"}\n";
  print "Service tag:           $dmi{\"stag\"}\n";
  print "Product mcode:         $dmi{\"pmcode\"}\n";
  print "Hardware mcode:        $dmi{\"hwmcode\"}\n";
  print "Software mcode:        $dmi{\"swmcode\"}\n";
  print "BIOS lock ID:          $dmi{\"blid\"}\n";
#  print "Universal info: 0x$dmi{\"uinfo\"}\n";
}

# Translate all DMI strings to upper case and trim whitespace.
sub UpperTrimDMI {
  #
  while (($key, $value) = each %dmi) {
    # upper case
    $value =~ tr/a-z/A-Z/;
    # trim whitespace before and after string
    $value =~ s/^\s*//;
    $value =~ s/\s*$//;
    # store new value
    $dmi{$key} = $value;
  }
}

# Get model specific info from models.csv (cached in modelslines).
# $dmi{"pmcode"} is not filled in because it is dependent on
# serial number, which the user enters later.
sub GetModel {
  local ($model) = @_;
  for (@modelslines) {
    @fields = split(/,/, $_);
    if ($fields[0] eq $model) {
      shift(@fields);
      local (@csvfields) = ("mname", "blid", "uinfo", "pcode", "pmcode", "hwmcode", "swmcode", "minserial", "maxserial");
      for $csvfield (@csvfields) {
        $dmi{$csvfield} = shift(@fields);
      }
      return 1;
    }
  }
  return 0;
}

# Cache models.csv
sub ReadModels {
  # models.csv
  open(MDLS, "<$input_dir/models.csv") || ($! = 255, die "Fatal Error: Couldn't open $input_dir/models.csv for input");
  undef $/;
  local($models) = <MDLS>;
  $/ = "\n";
  close MDLS;
  @modelslines = split(/\n/, $models);
}

#
# batch files
#
# Create batch files from template files.
# Read all TPL files from /input and write them to /tmp BAT files.
sub CreateBats {
  &CreateBat("wrtdmi");
  &CreateBat("wrtdmi1");
  &CreateBat("wrtdmi2");
}
# Take a template batch file and fill in the variables to
# create a model specific batch file.
sub CreateBat {
  # read file
  local($bat) = @_;
  open(TPL, "<$input_dir/$bat.tpl") || return 0;
  undef $/;
  local($tpl) = <TPL>;
  $/ = "\n";
  close TPL;
  # do substitutions
  @dmifields = ("mname", "pcode", "serial", "stag", "blid", "uinfo", "pmcode", "hwmcode", "swmcode", "uuid");
  for $dmifield (@dmifields) {
    $varname = $dmifield;
    $varname =~ tr/a-z/A-Z/;
    $tpl =~ s/%$varname%/$dmi{$dmifield}/g;
  }
  # the random string used to connect floppy and unit
  $rnd = substr(rand(), 3, 15);
  $tpl =~ s/%RND%/$rnd/g;
  # write file
  open(BATOUT, ">$tmp_dir/$bat.bat") || ($! = 255, die "Fatal Error: Couldn't open $tmp_dir/$bat.bat for output");
  print BATOUT $tpl;
  close BATOUT;

  return 1;
}

#
# general utilities
#
# Get a valid number from user.
sub GetUserNumber {
  local ($msg) = @_;
  for(;;) {
    print "$msg\n> ";
    chop ($usersel = <>);
    # workaround for wrpc (they don't have serial number)
    if ($usersel eq 'wrpc') {
        $usersel = 1234567;
    }
    if (&IsNumeric($usersel)) {
      print "\n";
      return $usersel;
    }
    else {
      print "\n- Error: Invalid value. Try again...\n";
    }
    print "\n";
  }
}

# Get upper case string from user.
sub GetUserString {
  local ($msg) = @_;
  local ($str);
  print "$msg\n> ";
  chop ($str = <>);
  # translate to upper case
  tr/a-z/A-Z/;
  #
  print "\n";
  return $str;
}

# Check if a string contains only digits.
sub IsNumeric {
  local ($str) = @_;
  if ($str =~ /^(\d+)$/) {
    return 1;
  }
  return 0;
}

# check if a string contains only hex digits
sub IsHex {
  local ($str) = @_;
  if ($str =~ /^([0-9A-Fa-f]+)$/) {
    return 1;
  }
  return 0;
}

# Prompt user with a yes/no question.
sub YesOrNo {
  local ($msg) = @_;
  print "$msg? [Y/N]: ";
  for (;;) {
    $c = &GetSingleChar;
    if ($c eq "y" || $c eq "Y") {
      print "Y\n\n";
      return 1;
    }
    if ($c eq "n" || $c eq "N") {
      print "N\n\n";
      return 0;
    }
  }
}

# Get single char input from user.
sub GetSingleChar {
  local ($c) = "";
  $old_ioctl = ioctl(STDIN, 0, 0); # get device info
  $old_ioctl &= 0xff;
  ioctl(STDIN, 1, $old_ioctl | 32); # set bit 5
  sysread(STDIN, $c, 1); # read char
  ioctl(STDIN, 1, $old_ioctl); # back to cooked mode
  return $c;
}

# Read strings from DMI using files prepared by run.bat before this script
# starts. Many of these will always be overwritten with information from
# the csv files but I've left them in for completeness.
sub GetDMI {
  # mcodes
  $tmp = &GetStructureField("Model Code");
  if ($tmp =~ /(.*)-(.*)-(.*)/) {
    $dmi{"pmcode"} = $1;
    $dmi{"hwmcode"} = $2;
    $dmi{"swmcode"} = $3;
  }
  else {
    $dmi{"pmcode"} = "";
    $dmi{"hwmcode"} = "";
    $dmi{"swmcode"} = "";
  }
  # pcode and serial
  $tmp = &GetStructureField("Serial Number");
  if ($tmp =~ /(.*)-(.*)/) {
    $dmi{"pcode"} = $1;
    $dmi{"serial"} = $2;
  }
  else {
    $dmi{"pcode"} = $tmp;
    $dmi{"serial"} = $2;
  }
  #
  $dmi{"stag"} = &GetStructureField("Service Tag");
  $dmi{"mname"} = &GetStructureField("Model Name");
#  $dmi{"biosname"} = &GetStructureField("BIOS Version");
  $dmi{"blid"} = &GetStructureField("BIOS Lock No");
  $dmi{"uinfo"} = substr(&GetStructureField("Universal Info"), 2);
#  $dmi{"eid"} = substr(&GetStructureField("Extended ID"), 2);
  # convert all DMI fields to upper case and trim whitespace
  &UpperTrimDMI;
}

#
sub GetStructureField {
  ($fieldname) = @_;
  # read dmi text file
  $fname = "$tmp_dir/rominfo5.txt";
  open(DMITXT, "<$fname") || ($! = 255, die "Couldn't open $fname for input");
  for (<DMITXT>) {
    if (/$fieldname\.*(.*)/) {
      close DMITXT;
      return $1;
    }
  }
  $! = 255;
  die "Fatal Error: GetStructureField() failed: $fname, $fieldname";
}

#
# reports
#
# menu
format STDOUT_TOP =
ÉÍÍÍÍÑÍÍÍÍÍÍÍÑÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
º  # ³ Func. ³ Model                                                          º
ÇÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
º  0 ³ Check ³ ALL - Displays DMI Information                                 º
ÇÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
º  1 ³ BIOS  ³ ALL - Image: @<<<<<<<<<<<<<<< Name: @<<<<<<<<<<<<<<<           º
                            $ENV{"BIOS_IMG"},      $ENV{"BIOS_NAME"}
ÇÄÄÄÄÅÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶
.
format STDOUT =
º @> ³ DMI   ³ @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< @<<<<<                         º
  $id,         $mname,                          $cto
.
