alloc ptr 5 int
mov [ptr] 10
msg '*ptr = ' [ptr]
inc [ptr]
msg 'ptr[0]++ = ' [ptr]  
free ptr
end