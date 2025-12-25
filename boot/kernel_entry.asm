[bits 32]
[extern kernel_main] ; Make sure this matches 'void kernel_main()' in C
call kernel_main
jmp $