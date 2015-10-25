use strict;
open SRC_LIST, "../src/CMakeLists.txt";
`cp CMakeLists.txt CMakeLists.orig`;
open NEW_LIST, ">CMakeLists.txt";
open OLD_LIST, "CMakeLists.orig";
my $srcs;
while(<SRC_LIST>) {
    if(/add_executable\(os_4\.bin ((?:[a-zA-Z0-9_.]+ ?)+)\)/) {
	$srcs = $1;
    }
}
while(<OLD_LIST>) {
    s/set\(KERNEL_SRCS ((?:[a-zA-Z0-9_.]+ ?)+)\)/set(KERNEL_SRCS $srcs)/g;
    print NEW_LIST;
}   
`rm CMakeLists.orig`;
open LISTFILE, ">cmake_tests.txt";
open TESTFILE, ">cmake_add_test.txt";
my @files = split /\n/, `ls src`;
foreach my $file (@files) {
    $file =~ s/\.c//;
    print LISTFILE "add_executable($file.bin test_wrapper.c src/$file.c \${KERNEL_SRCS})\n";
    print LISTFILE "target_link_libraries($file.bin \"-T ../src/linker.ld\" -ffreestanding -nostdlib -O2 gcc)\n";
    print TESTFILE "add_test($file run_test $file)\n"
}
