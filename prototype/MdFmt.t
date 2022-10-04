use strict;
use warnings;

use File::Basename;
use File::Spec;
use Test::More;
use MdFmt;

my $dirname = dirname(__FILE__);

chdir File::Spec->join($dirname, '..', 'test-data') or die 'Not possible to change directory!';
my $output;
open my $outputFH, '>', \$output;
my $oldFH = select $outputFH;
@ARGV = ('input.md');
main;
select $oldFH;
close $outputFH;


open my $expectedFH, '<', 'expected.md';
my @expected = <$expectedFH>;
is $output, join ('', @expected), 'Should match formatted output with expected';

done_testing;
