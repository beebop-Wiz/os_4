my %states;
my $ns = 0;

my %actions = (
    'ignore' => 0,
    'print' => 1,
    'execute' => 2,
    'clear' => 3,
    'collect' => 4,
    'param' => 5,
    'esc_dispatch' => 6,
    'csi_dispatch' => 7,
    'hook' => 8,
    'put' => 9,
    'unhook' => 10,
    'osc_start' => 11,
    'osc_put' => 12,
    'osc_end' => 13,
    );
my @line;
my @base;
my $cur_action;
my $inv = 0;
print "#ifndef VT100_STATE_H\n#define VT100_STATE_H\n#define VT100_state {";
while(<>) {
    my @a = split /\|/;
    if($a[0] eq "in") {
	$inv = 1;
    } else {
	$states{$a[0]} = $ns;
	for($i = 0; $i < 256; $i++) {
	    $line[$i] = {'action' => 0, 'state' => $ns};
	    $line[$i]{'action'} = $base[$i]{'action'} if exists $base[$i]{'action'};
	    $line[$i]{'state'} = $base[$i]{'state'} if exists $base[$i]{'state'};
	}
	$inv = 0;
    }
    my @b = split /\s+/, $a[1];
    foreach my $s (@b) {
	if($s =~ /^([0-9A-F][0-9A-F])$/) {
	    $line[hex $1]{'action'} = $cur_action unless $inv;
	    $base[hex $1]{'action'} = $cur_action if $inv;
	} elsif($s =~ /^([0-9A-F][0-9A-F])-([0-9A-F][0-9A-F])$/) {
	    for($i = hex $1; $i <= hex $2; $i++) {
		$line[$i]{'action'} = $cur_action unless $inv;
		$base[$i]{'action'} = $cur_action if $inv;
	    }
	} else {
	    foreach my $k (keys %actions) {
		if($s eq $k) {
		    $cur_action = $actions{$k};
		}
	    }
	}
    }
    my @b = split /\s+/, $a[2];
    foreach my $s (@b) {
	if($s =~ /^([0-9A-F][0-9A-F])$/) {
	    $line[hex $1]{'state'} = $cur_action unless $inv;
	    $base[hex $1]{'state'} = $cur_action if $inv;
	} elsif($s =~ /^([0-9A-F][0-9A-F])-([0-9A-F][0-9A-F])$/) {
	    for($i = hex $1; $i <= hex $2; $i++) {
		$line[$i]{'state'} = $cur_action unless $inv;
		$base[$i]{'state'} = $cur_action if $inv;
	    }
	} else {
	    $s =~ s/s//g;
	    $cur_action = $s;
	}
    }
    unless($inv) {
	print "{";
	for($i = 0; $i < 256; $i++) {
	    if(defined $line[$i]) {
		$line[$i]{'action'} = 0 unless defined $line[$i]{'action'};
		$line[$i]{'state'} = 0 unless defined $line[$i]{'state'};
		print "0x" . sprintf("%x%x", $line[$i]{'action'}, $line[$i]{'state'}) . ",";
	    } else {
		print "{0,$ns}, ";
	    }
	    delete $line[$i];
	}
	chomp $a[3];
	print "}, /* $a[3] */ \\\n\t";
	$ns++;
    }
}

print "}\n#endif\n";
