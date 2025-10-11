mov size 5
mov i 0
mov temp 0
mov sum 0

alloc arr_ptr size int

; [1, 2, 3, 4, 5]
mov i 0
mov temp 1
fill_loop:
    mov [arr_ptr] temp
    inc temp
    add arr_ptr 1
    inc i
    cmp i size
    jl fill_loop

sub arr_ptr size
mov i 0
mov sum 0
sum_loop:
    add sum [arr_ptr]
    add arr_ptr 1
    inc i
    cmp i size
    jl sum_loop

sub arr_ptr size
msg 'Array:'
mov i 0
print_loop:
    msg 'arr[' 'i' ']' ' = '  [arr_ptr]
    add arr_ptr 1
    inc i
    cmp i size
    jl print_loop

msg 'Sum: ' sum

sub arr_ptr size
free arr_ptr
end