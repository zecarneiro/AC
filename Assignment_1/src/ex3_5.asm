# Exercicio 3.5

.data
bitmap: .space 270000
filename: .asciiz "/home/jose/Documentos/AC/Assignment_1/ht.pgm"
error_open: .asciiz "Deu Erro no open!\n"
error_read: .asciiz "Deu Erro no read!\n"
	
buffer: .space 4045
buffer_str: .asciiz "Buffer: \n" # string auxiliar ao programador
cor_str: .asciiz "\n Cor:" # string auxiliar ao programador
val: .byte
.text

main:

	jal le_para_buffer
	
	jal escreve
	
	j exit
		
le_para_buffer:		
	
	#opens file
	li   $v0, 13       # system call for open file
	la   $a0, filename  # board file name
	li   $a1, 0        # Open for reading
	li   $a2, 0
	syscall

	# Verifica se nao deu erro
	bltz $v0, error1
	move $a0,$v0	# se nao der erro, entao $a0=file descriptor do ficheiro

	#read from file
	li   $v0, 14       # system call for read from file
	la   $a1, buffer   # address of buffer to which to read
	li   $a2, 4045      # hardcoded buffer length
	syscall    
	
	# Verifica se nao deu erro
	bltz $v0, error2
	move $a1, $a0	# se nao der erro, entao $a0=file descriptor do ficheiro        # read from file
				
	# Close the file 
	li   $v0, 16       # system call for close file
	syscall            # close file

	# Imprime buffer_str
	li $v0, 4
	la $a0, buffer_str
	syscall

	# Imprime buffer
	li $v0, 4
	la $a0, buffer
	syscall

	jr $ra

escreve:
	la $t9, bitmap
	#addi	$t9, $zero, 0x10010000
	la $s0, buffer
		
	addi $t0,$zero,13		# $t0=\r=CR
	addi $t1,$zero,0		# $t1=\0=NULL
	addi $t2,$zero,10		# $t2=\n=LF Nova Linha
	addi $t3,$zero,32		# $t3 = space
	addi $t4,$zero,0  #contador1
	addi $t5,$zero,0  #contador2
	addi $t6,$zero,0  #contador3
		
	loop1:
		lb $s7, 0($s0)
		bne $s7, 0, continue
		continue:
			ble $s7, 32,inst
			addi $t5, $t5, 1
			addi $s0, $s0, 1
			j loop1
	inst:
		addi $t4, $t4, 1
		addi $t5, $t5, 1
		bge $t4, 3,inst2
		j loop1
	inst2:
		addi $s0, $s0, 1
		j loop_escreve

	loop_escreve:
		addi $t5, $t5, 1
	
		#Calculo RGB
		lb $s1, 0($s0) #load red
		and $s1, $s1, 0x000000ff	
		#add $s2, $s2, $s1
		sll $s2, $s2, 8 #shift left 8 casas
		or $s2, $s2, $s1
		#add $s2, $s2, $s1
		sll $s2, $s2, 8 #shift left 8 casas
		#add $s2, $s2, $s1 #cor final
		or $s2, $s2, $s1

		# Imprime cor_str
		li $v0, 4
		la $a0, cor_str
		syscall

		# Imprime cor
		li $v0, 1
		add $a0, $zero, $s2
		syscall

		sw $s2,0($t9)		# Pixel introduzido no bitmap
		
		addi $t6, $t6, 1
		
		#beq $t6, 63, muda_linha
			
	continue2:	
		
		addi $s0, $s0, 1
		addi $t9, $t9, 4
		beq $t5, 4045, exit_func
		
		j loop_escreve
	exit_func:
			
		jr $ra
#------------------------------------------------------------
#------------------------------------------------------------

muda_linha:
		li $t6,0  #contador3
		addi $t9, $t9, 4
		j continue2

error1:

	# Imprime pedido de indicação sobre execução do programa
	li $v0, 4
	la $a0, error_open
	syscall
	
#------------------------------------------------------------
#------------------------------------------------------------

error2:

	# Imprime pedido de indicação sobre execução do programa
	li $v0, 4
	la $a0, error_read
	syscall

#------------------------------------------------------------
#------------------------------------------------------------

exit:
	li $v0,10
	syscall
		
	
		
		
