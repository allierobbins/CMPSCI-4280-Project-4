PUSH
LOAD T0
STACKW 0
READ T0
LOAD T0
STACKW 0
STACKR 0
STORE T0
STORE T0
LOAD T0
SUB T0
SUB 0
BRPOS L1
BRNEG L1
STACKR 0
MULT -1
STORE T0
WRITE T0
L1: NOOP
STOP
T0 0
T1 1