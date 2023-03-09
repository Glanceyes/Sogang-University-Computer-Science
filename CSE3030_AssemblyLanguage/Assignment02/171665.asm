INCLUDE irvine32.inc

.data
var1 BYTE "65", 0
count DWORD ?

.code
main PROC
	mov ecx, LENGTHOF var1; Calculate the length of string var1. (It contains the null letter.)
	dec ecx; Subtract 1 from ecx because of a null letter.
	mov esi, ecx; Use ESI register to make an indexed operand of var1.
	mov eax, 0; Set EAX register value into 0 to calculate the decimal value of the string.
	mov ebx, 1; Set EBX register value into 1 to repeat a step 7^n times. (n = 0, 1, 2, ...) 
L1: mov count, ecx; Store ECX into count memory operand since ECX register is the loop counter and need to use two loops.
	dec esi; An index must be 1 smaller than ECX value. 
	movzx edx, [var1 + esi]; [var1 + esi] means a value of a corresponding offset.
	sub edx, 48d; Subtract 48d(="0") from edx because of consideration of ASCII code.
	mov ecx, ebx; Set ECX into EBX to know how repeat an addition.
L2: add eax, edx; Add EDX to EAX to calculate decimal value of [var1 + esi].
	loop L2
	mov edx, ebx; Calculate a value of EBX * 7 by adding EBX 3 times and subtracting the first value of EBX.
	add ebx, ebx
	add ebx, ebx
	add ebx, ebx
	sub ebx, edx
	mov ecx, count; Set ECX into count to execute outer loop.
	loop L1
	call DumpRegs
	exit
main ENDP
END main