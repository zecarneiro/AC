.data
bitmap: .word 0x10010000
list2: .word 0,0,255


.text
main:

	la $a0, list2

	jal clear
		
	j exit

#$a0 -> argumento com cor
#$t0 -> tabela com cor
#$t6 -> bitmap
#$t1 -> cor temporária
#$t2 -> cor final
#$t3 -> variável auxiliar para calcular o endereço final do bitmap
#$t4 -> armazena endereço final do bitmap
#
#

clear:

	la $t0,($a0) #load da cor a usar no clear para o registo $t0
	la, $t6, bitmap #load do endereço do bitmap para $t6
	
	#Calculo RGB
	lw $t1, 0($t0) #load red	
	add $t2, $t2, $t1
	sll $t2, $t2, 8 #shift left 8 casas
	lw $t1, 4($t0) #load green
	add $t2, $t2, $t1
	sll $t2, $t2, 8#shift left 8 casas
	lw $t1, 8($t0) #load blue
	add $t2, $t2, $t1 #cor final

	#Calculo do tamanho do bitmap
	add $t3,$t3,64
	mul $t3,$t3,$t3
	mul $t3,$t3,4
	#Calculo do endereço final do bit map
	add $t4,$t4,$t6
	add $t4,$t4,$t3 # -> endereço final

	loop:
		sw $t2,0($t6) #armazena a cor no pixel designado por 0($t6)
		add $t6,$t6,4 #desloca o ponteiro que aponta para os pixeis individuais do bitmap
		beq $t6,$t4,exit # verifica se todos os pixeis estão preenchidos
		j loop
	
#=========================== Fim ===========================

exit:

	li $v0, 10
	syscall	
