#!/usr/bin/perl

use strict;
use warnings;

use File::Fetch;

my $source_path = 'https://github.com/carlos157oliveira/markdown-utils/releases/latest/download/md';
my $dest_path = '/usr/bin';
my $remote_file = File::Fetch->new(uri => $source_path);
my $installed_file = $remote_file->fetch(to => $dest_path);

# Obtain file permission and add only the executable bit
# https://perldoc.perl.org/functions/chmod
my $perm = (stat $installed_file)[2] & 07777;
chmod($perm | 0001, $installed_file);