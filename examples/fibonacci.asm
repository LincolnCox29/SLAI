mov input 7
mov a 0
mov b 1
mov i 2
mov result 0 
call fibonacci
msg 'Fibonacci sequence ' input ' = ' result
end

fibonacci:
    call sum
    mov a b
    mov b result
    inc i
    cmp i input
    jle fibonacci
    ret

sum:
    mov result a
    add result b
    ret