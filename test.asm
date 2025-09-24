mov input 10
mov result 1
call factorial
msg input '! = ' result
end

factorial:
    mul result input     ; r *= i
    dec input            ; i--
    cmp input 1
    jg  factorial        ; if (i < 1) jump 
    ret