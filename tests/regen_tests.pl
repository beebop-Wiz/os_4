use strict;
open LISTFILE, ">cmake_tests.txt";
open TESTFILE, ">cmake_add_test.txt";
my @files = split /\n/, `ls src`;
foreach my $file (@files) {
    $file =~ s/\.c//;
    print LISTFILE "add_executable($file.bin test_wrapper.c src/$file.c \${KERNEL_SRCS})\n";
    print LISTFILE "target_link_libraries($file.bin \"-T ../src/linker.ld\" -ffreestanding -nostdlib -O2 gcc)\n";
    print TESTFILE "add_test($file run_test $file)\n"
}
