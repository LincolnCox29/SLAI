mov input 10
mov result 1
mov d input 
call factorial
msg input '! = ' result
end

factorial:
    mul result d         ; r *= d
    dec d                ; i--
    cmp d 1
    jg  factorial        ; if (d < 1) jump 
    ret