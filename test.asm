mov input 10
mov result 1
mov i input 
call factorial
msg input '! = ' result
end

factorial:
    mul result i         ; r *= i
    dec i                ; i--
    cmp i 1
    jg  factorial        ; if (i < 1) jump 
    ret