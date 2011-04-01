#!/usr/bin/perl -w

# Program get the size of the components of an avr bin

# Copyright (C) 2003 Mads Bondo Dydensborg <madsdyd@diku.dk>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Check arg 
$detailed = shift(@ARGV);
if (defined $detailed) {
    $pattern = '\s*\d+: ([0-9a-fA-F]{8})\s+(\S*)\s+(\S*)\s+\S*\s+\S*\s+\S*\s+(.[^\s]*)';
    $space = "40";
    $lines = "--------------------";
} else {
    $pattern = '\s*\d+: ([0-9a-fA-F]{8})\s+(\S*)\s+(\S*)\s+\S*\s+\S*\s+\S*\s+(.[^\$\s]*)';
    $space = "20";
    $lines = "";
}
# Define a translation table for the int vectors
$vectors{'__vector_15'} = "Timer0/Counter0 compare match";
$vectors{'__vector_18'} = "USART0, Rx Complete";
$vectors{'__vector_19'} = "USART0, Data Register Empty";
$vectors{'__vector_20'} = "USART0, Tx Complete";
$vectors{'__vector_30'} = "USART1, Rx Complete";
$vectors{'__vector_31'} = "USART1, Data Register Empty";
$vectors{'__vector_32'} = "USART1, Tx Complete";

$bin = "testCPABE.sky";

open(INPUT, "msp430-readelf -s -W $bin | grep -v 00000000 |") 
    || die "Unable to run avr-objdump -t $bin";

# We ignore no matches on the first 3 lines.
$atline = 0;

while(<INPUT>) {
    $atline++;
    # Match a line
    if (m/$pattern/) {
	$size = $2;
	$sym  = $4;
	$type = $3;
	
	next unless $size > 0;

	if ($type eq "OBJECT") {
	    if (defined $bss{$sym}) {
		$bss{$sym} += $size;
	    } else {
		$bss{$sym} = $size;
	    }
	} else {
	    if ($type eq "FUNC") {
		if (defined $text{$sym}) {
		    $text{$sym} += $size;
		} else {
		    $text{$sym} = $size;
		}
	    } else {
		# This is a hack - I think it means BSS sometimes...
		if ($type eq "NOTYPE") {
		    if (defined $bss{$sym}) {
			$bss{$sym} += $size;
		    } else {
			$bss{$sym} = $size;
		    }
		} else {
		    print STDERR "Warning: Unknown type \"$type\", size > 0\n";
		}
	    }
	}
    } else {
	if ($atline > 3) {
	    print STDERR "Warning: No match for $_";
	}
    }
}

# Always use 202 bytes for processor init
$text{"Processor_init"} = 202;

print "Size breakdown\n";
printf "% -".$space."s  % 5s  %5s\n", "Component Name", "code", "bss";
print "$lines-----------------------------------\n";
$total_text = 0;
$total_bss = 0;
foreach $key (sort(keys(%text))) {
    $total_text += $text{$key};
    if (defined $bss{$key}) {
	$bss_size = $bss{$key};
	$bss{$key} = 0;
    } else {
	$bss_size = 0;
    }
    $total_bss += $bss_size;
    if (defined $vectors{$key}) {
	$exp = $vectors{$key};
    } else {
	$exp = "";
    }
    printf "% -".$space."s  % 5i  % 5i   %s\n", $key, $text{$key}, $bss_size, $exp;
}

print "$lines-----------------------------------\n";
printf "% -".$space."s  % 5i  % 5i\n", "total", $total_text, $total_bss;

if ($total_text == 0) {
    print "Error: No code size - did you strip the binary?\n";
    exit -1;
}

print "\nAdditional bss sizes\n";
print "$lines-----------------------------------\n";
$total = 0;
foreach $key (sort(keys(%bss))) {
    if (0 == $bss{$key}) {
	next;
    }
    $total += $bss{$key};
    printf "% -".$space."s         % 5i\n", $key, $bss{$key};
}
print "$lines-----------------------------------\n";
printf "% -".$space."s         % 5i\n", "total", $total;

$total_bss += $total;

print "\n$lines-----------------------------------\n";
print "$lines-----------------------------------\n";
printf "% -".$space."s  % 5i  % 5i\n", "Grand totals", $total_text, $total_bss;

close(INPUT);

# Sanity check
print "\nCalling size on binary to sanity check\n";
open(INPUT, "avr-size -A $bin|")
    || die "Unable to run avi-size -A $bin";

while(<INPUT>) {
    if (m/\.data\s*(\d+)\s/) {
	$size_data = $1;
    }
    if (m/\.text\s*(\d+)\s/) {
	$size_text = $1;
    }
    if (m/\.bss\s*(\d+)\s/) {
	$size_bss = $1;
    }
}

printf "Assuming that whole data section belongs to main component, this is the numbers as calculated and as the size program gives them. They should match - but sometimes they dont. Sigh.\n";

printf "Totals\n";
printf "% -10s  % 5s  % 5s\n", "part", "calc", "size";
print "-----------------------------------\n";
printf "% -10s  % 5i  % 5i\n", "code", $total_text, $size_text;
printf "% -10s  % 5i  % 5i\n", "bss", $total_bss, $size_bss;
printf "% -10s  % 5i  % 5i\n", "data", $size_data, $size_data;
print "-----------------------------------\n";
printf "% -10s  % 5i  % 5i\n", "Totals", $total_text + $total_bss + $size_data, $size_text + $size_bss + $size_data;






