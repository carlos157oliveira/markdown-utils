#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long qw(GetOptions :config posix_default no_ignore_case);
use Pod::Usage;

use base 'Exporter';
our @EXPORT_OK = qw(main);


my $max_line_length = 80;
my $last_inside_code_block = 0;


# Try to split a line at the last whitespace before $max_line_length is reached.
# If it couldn't, then try to split at a latter whitespace and leaves the text line
# alone in case this also fails.
sub split_line {
    my ($line, $inside_code_block) = @_;
    # Store the value for invocations of this function on later lines
    # from the file since a code block can span multiple lines.
    # The next line of input will have the value of $inside_code_block 
    # of the last activation of &split_line on the current line.
	$last_inside_code_block = $inside_code_block;

    # Verification to see if a markdown code block is beginning or ending.
    if ($line =~ /((```)\w*)(?:(\s+)(\S))?/) {
	    my (@left, @right);
	    my ($left_end, $right_start);
        my $right_offset = 0;
	    my $after_delim = $4;
	    my $middle = $2;
	    if ($inside_code_block) {
	        # We were inside a code block and the delimiter is currently ending it.
	        # Just get the position of the delimiter to divide the string and continue
	        # the text processing after it where the text may be formatted again.
	        ($left_end, $right_start)= ($-[2], $+[2]);
            # Avoid spurious newlines by checking the left part contains something.
	        # An empty string entry will generate a spurious newline when the strings are joined.
	        @left = $` unless $` =~ /^\s*$/;
	    }
	    else {
	        # We were out of code block and the delimiter will start it.
	        # In this case the is an extra care because we want to preserve at the same
	        # line any word that is glued to delimiter and identifies the programming
	        # language of the block.
	        # The spaces after them should be discarded and not be passed with the text
	        # that will be inside the code block.
	        ($left_end, $right_start) = ($-[1], $+[1]);
            $right_offset = length $3 if defined $3;
	        $middle = $1;
	        @left = split_line($`, $inside_code_block);
	    }
	    @right = split_line(substr($line, $right_start + $right_offset), not $inside_code_block);
	    return (@left, $middle, @right);
    }

    return () if $line =~ /^\s*$/;

    # The line already has an adequated size
    # or the code block does not finish on this line,
    # leaving its format alone.
    return $line if length($line) <= $max_line_length or $inside_code_block;
    
    # Iterate through certain markdown elements:
    # links, inline code blocks, and the beginning of headers and list items.
    # If they don't match, the last alternative in the pattern is whitespace
    # that can be used as breakpoint because it isn't inside of the other elements,
    # otherwise they would have matched.
	my $breakpoint;
    my $breakpoint_space_length = 0;
    while ($line =~ /
        \[[^\]]*?\]\([^\)]*?\)
        |
        (?<!`)`.*?`(?!`)
        |
        ^\s*\#+\s+.+
        |
        ^\s*(?:-|>)\s+\S+
        |
        (\s+)
    /xg) {
	    if (defined $1) {
	        if ($-[0] <= $max_line_length or not defined $breakpoint) {
                $breakpoint = $-[0];
                $breakpoint_space_length = length $1;
	        }
            last if $-[0] > $max_line_length;
	    }
    }
    
    my $split_start = $breakpoint + $breakpoint_space_length;
	return $line unless defined $breakpoint or $split_start == length ($line);

    my @new_lines = split_line(substr($line, $split_start), 0);
    my $current_line = substr($line, 0, $breakpoint);
    return ($current_line =~ /^\s*$/ ? () : $current_line, @new_lines);
}


sub process_input_line {
    my $input_line = shift;
    # See if line has an indentation which we preserve in its parts
    # when splitted.
    $input_line =~ /^\s+/;
    my $prefix = $&;
    my @lines = split_line($input_line, $last_inside_code_block);

    s/\s+$// for @lines;
    if (defined $prefix) {
        $lines[$_] = $prefix . $lines[$_] for 1..$#lines;
    }
    return @lines;
}


sub main {
    my $output_filename = '';    
    GetOptions(
        'max-line-length|m=i' => \$max_line_length,
        'output|o=s' => \$output_filename,
        'help|h' => sub { pod2usage(-exit => 0, -verbose => 2); }
    );
    if ($output_filename) {
        open my $output, '>', $output_filename
            or die "Can't write to file: $output_filename !";
        select $output;
        print 'oi';
    }
    while (my $file = shift @ARGV) {
        die "An input file ($file) is also the output!" if ($file eq $output_filename);
        open my $fh, '<', $file or die "File not found: $file !";

        while (<$fh>) {
            print join ("\n", process_input_line $_), "\n";
        }

        close $fh;
    }
}


main() unless caller;

1;

__END__
 
=head1 Markdown Formatter
 
Break long lines in your markdown file.
 
=head1 Synopsis
 
program-name [options] file1 file2 ...
 
 Options:
   --max-line-length=number|-m number     desired maximum length of a line
   --output=filename|-o filename          where it should output the result, default is stdout
   --help|-h                              display this help message    
 
=cut